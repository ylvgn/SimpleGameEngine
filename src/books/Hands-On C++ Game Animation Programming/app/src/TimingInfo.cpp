#include "TimingInfo.h"

namespace sge {

#if 0
#pragma mark ================= GPU ====================
#endif
TimingInfo_GPUStage::TimingInfo_GPUStage(Info* info)
	: Base(info)
	, _timerResultAvailable(0)
	, _gpuStartTime(0)
	, _gpuStopTime(0)
{
	glGenQueries(1, &_gpuApplicationStart);
	glGenQueries(1, &_gpuApplicationStop);
	glGenQueries(1, &_gpuImguiStart);
	glGenQueries(1, &_gpuImguiStop);
}

void TimingInfo_GPUStage::beginAppRender() {
	if (!_info->_isFirstSample) {
		glGetQueryObjectiv(_gpuApplicationStop, GL_QUERY_RESULT, &_timerResultAvailable);
		while (!_timerResultAvailable) {
			SGE_LOG("Waiting on app GPU timer!\n");
			glGetQueryObjectiv(_gpuApplicationStop, GL_QUERY_RESULT, &_timerResultAvailable);
		}
		glGetQueryObjectui64v(_gpuApplicationStart, GL_QUERY_RESULT, &_gpuStartTime);
		glGetQueryObjectui64v(_gpuApplicationStop, GL_QUERY_RESULT, &_gpuStopTime);
		_addTimeDiff(_info->_accumulator.appGPU);
	}
	glQueryCounter(_gpuApplicationStart, GL_TIMESTAMP);
}

void TimingInfo_GPUStage::endAppRender() {
	glQueryCounter(_gpuApplicationStop, GL_TIMESTAMP);
}

void TimingInfo_GPUStage::beginImGuiRender() {
	if (!_info->_isFirstSample) {
		glGetQueryObjectiv(_gpuImguiStop, GL_QUERY_RESULT, &_timerResultAvailable);
		while (!_timerResultAvailable) {
			SGE_LOG("Waiting on imgui GPU timer!\n");
			glGetQueryObjectiv(_gpuImguiStop, GL_QUERY_RESULT, &_timerResultAvailable);
		}
		glGetQueryObjectui64v(_gpuImguiStart, GL_QUERY_RESULT, &_gpuStartTime);
		glGetQueryObjectui64v(_gpuImguiStop, GL_QUERY_RESULT, &_gpuStopTime);
		_addTimeDiff(_info->_accumulator.imguiGPU);
	}
	glQueryCounter(_gpuImguiStart, GL_TIMESTAMP);
}

void TimingInfo_GPUStage::endImGuiRender() {
	glQueryCounter(_gpuImguiStop, GL_TIMESTAMP);
}

void TimingInfo_GPUStage::snapShot() {
	_info->_display.appGPU	 = _info->_accumulator.appGPU   / 60.0;
	_info->_display.imguiGPU = _info->_accumulator.imguiGPU / 60.0;
}

void TimingInfo_GPUStage::_addTimeDiff(double& out) {
	double d = static_cast<double>(_gpuStopTime - _gpuStartTime) / 1000000.0;
	out += d;
}

#if 0
#pragma mark ================= CPU ====================
#endif
TimingInfo_CPUStage::TimingInfo_CPUStage(TimingInfo* info)
	: Base(info)
{
	memset(&_timerStart, 0, sizeof(LARGE_INTEGER));
	memset(&_timerStop, 0, sizeof(LARGE_INTEGER));

	memset(&_frameStart, 0, sizeof(LARGE_INTEGER));
	memset(&_frameStop, 0, sizeof(LARGE_INTEGER));

	QueryPerformanceFrequency(&_timerFrequency);
}

void TimingInfo_CPUStage::beginFrame() {
	QueryPerformanceCounter(&_frameStart);
}

void TimingInfo_CPUStage::endFrame() {
	QueryPerformanceCounter(&_frameStop);
	_addFrameDiff(_info->_accumulator.frameTime);
}

void TimingInfo_CPUStage::beginUpdate(float dt) {
	_info->_accumulator.deltaTime += dt;
	QueryPerformanceCounter(&_timerStart);
}

void TimingInfo_CPUStage::endUpdate(float dt) {
	QueryPerformanceCounter(&_timerStop);
	_addTimeDiff(_info->_accumulator.frameUpdate);
}

void TimingInfo_CPUStage::beginAppRender() {
	QueryPerformanceCounter(&_timerStart);
}
void TimingInfo_CPUStage::endAppRender() {
	QueryPerformanceCounter(&_timerStop);
	_addTimeDiff(_info->_accumulator.frameRender);
}

void TimingInfo_CPUStage::beginImGuiUpdate() {
	QueryPerformanceCounter(&_timerStart);
}
void TimingInfo_CPUStage::endImGuiUpdate() {
	QueryPerformanceCounter(&_timerStop);
	_addTimeDiff(_info->_accumulator.imguiLogic);
}

void TimingInfo_CPUStage::beginImGuiRender() {
	QueryPerformanceCounter(&_timerStart);
}
void TimingInfo_CPUStage::endImGuiRender() {
	QueryPerformanceCounter(&_timerStop);
	_addTimeDiff(_info->_accumulator.imguiRender);
}

void TimingInfo_CPUStage::beginSwapBuffer() {
	QueryPerformanceCounter(&_timerStart);
}
void TimingInfo_CPUStage::endSwapBuffer() {
	QueryPerformanceCounter(&_timerStop);
	_addTimeDiff(_info->_accumulator.swapBuffer);
}

void TimingInfo_CPUStage::snapShot() {
	_info->_display.win32Events = _info->_accumulator.win32Events / 60.0;
	_info->_display.frameUpdate = _info->_accumulator.frameUpdate / 60.0;
	_info->_display.frameRender = _info->_accumulator.frameRender / 60.0;
	_info->_display.imguiLogic  = _info->_accumulator.imguiLogic  / 60.0;
	_info->_display.imguiRender = _info->_accumulator.imguiRender / 60.0;
	_info->_display.swapBuffer  = _info->_accumulator.swapBuffer  / 60.0;
	_info->_display.frameTime   = _info->_accumulator.frameTime   / 60.0;
	_info->_display.deltaTime   = _info->_accumulator.deltaTime   / 60.f;
}

void TimingInfo_CPUStage::_addTimeDiff(double& out) {
	LONGLONG diff = _timerStop.QuadPart - _timerStart.QuadPart;
	double d = static_cast<double>(diff) * 1000.0 / static_cast<double>(_timerFrequency.QuadPart);
	out += d;
}

void TimingInfo_CPUStage::_addFrameDiff(double& out) {
	LONGLONG diff = _frameStop.QuadPart - _frameStart.QuadPart;
	double d = static_cast<double>(diff) * 1000.0 / static_cast<double>(_timerFrequency.QuadPart);
	out += d;
}

#if 0
#pragma mark ================= TimingInfo ====================
#endif
void TimingInfo::create(HWND hwnd) {
	cpu = make_unique<CPUStage>(this);
	gpu = make_unique<GPUStage>(this);

	memset(&_display, 0, sizeof(FrameTimer));
	memset(&_accumulator, 0, sizeof(FrameTimer));

	_frameCounter	= 0;
	_frameBudget	= 0;
	_isFirstSample	= true;
	_isSlowFrame	= false;

	_calcDisplayFrequencyAndBudget(hwnd);
}

void TimingInfo::profilingHouseKeeping() {
	++_frameCounter;
	_isFirstSample = false;

	if (_frameCounter >= 60) {

		cpu->snapShot();
		gpu->snapShot();

		_isSlowFrame = _display.frameTime >= _frameCounter;

		_frameCounter = 0;
		memset(&_accumulator, 0, sizeof(FrameTimer));
	}
}

void TimingInfo::_calcDisplayFrequencyAndBudget(HWND hwnd) {
	HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);

	MONITORINFOEX monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &monitorInfo);

	DEVMODE devMode;
	devMode.dmSize = sizeof(DEVMODE);
	devMode.dmDriverExtra = 0;
	EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

	_displayFrequency = static_cast<int>(devMode.dmDisplayFrequency);
	_frameBudget = 1000.0 / static_cast<double>(_displayFrequency);

//	SGE_LOG("Display frequency: {}\n", _displayFrequency);
//	SGE_LOG("Frame budget: {:0.2f} ms\n", _frameBudget);
}

} // namespace