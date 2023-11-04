#pragma once

namespace sge {
	
struct TimingInfo_FrameTimer {
	// High level timers
	double frameTime	= 0;
	float  deltaTime	= 0;

	// CPU timers
	double frameUpdate	= 0;
	double frameRender	= 0;
	double win32Events	= 0;
	double imguiLogic	= 0;
	double imguiRender	= 0;
	double swapBuffer	= 0;

	// GPU timers
	double imguiGPU		= 0;
	double appGPU		= 0;
};

struct TimingInfo;

struct TimingInfoStage : public NonCopyable {
	using Info = TimingInfo;
	virtual ~TimingInfoStage() = default;

protected:
	TimingInfoStage(Info* info) : _info(info) {}
	Info* _info = nullptr;
};

struct TimingInfo_GPUStage : public TimingInfoStage {
	using Info = TimingInfo;
	using Base = TimingInfoStage;

	TimingInfo_GPUStage(Info* info);

	void beginAppRender();
	void endAppRender();

	void beginImGuiRender();
	void endImGuiRender();

	void snapShot();

private:

	void _addTimeDiff(double& out);

	int			_timerResultAvailable = 0;

	GLuint64	_gpuStartTime = 0;
	GLuint64	_gpuStopTime  = 0;

	GLuint		_gpuApplicationStart = 0;
	GLuint		_gpuApplicationStop	 = 0;
	GLuint		_gpuImguiStart		 = 0;
	GLuint		_gpuImguiStop		 = 0;
};

struct TimingInfo_CPUStage : public TimingInfoStage {
	using Info = TimingInfo;
	using Base = TimingInfoStage;

	TimingInfo_CPUStage(TimingInfo* info);

	void beginFrame();
	void endFrame();

	void beginUpdate(float dt);
	void endUpdate(float dt);

	void beginAppRender();
	void endAppRender();

	void beginImGuiUpdate();
	void endImGuiUpdate();

	void beginImGuiRender();
	void endImGuiRender();

	void beginSwapBuffer();
	void endSwapBuffer();

	void snapShot();

private:
	void _addTimeDiff(double& out);
	void _addFrameDiff(double& out);

	LARGE_INTEGER	_timerStart, _timerStop;
	LARGE_INTEGER	_frameStart, _frameStop;
	LARGE_INTEGER	_timerFrequency;
};

struct TimingInfo : public NonCopyable {
	using FrameTimer = TimingInfo_FrameTimer;
	using GPUStage   = TimingInfo_GPUStage;
	using CPUStage   = TimingInfo_CPUStage;

	void create(HWND hwnd);
	void profilingHouseKeeping();

	UPtr<CPUStage>	cpu;
	UPtr<GPUStage>	gpu;

	inline int    frameCounter()	const { return _frameCounter; }
	inline int    displayFrequency()const { return _displayFrequency; }

	inline double frameBudget()		const { return _frameBudget; }
	inline bool   isSlowFrame()		const { return _isSlowFrame; }
	inline bool	  isFirstSample()	const { return _isFirstSample; }

	inline double frameTime()		const { return _display.frameTime; }
	inline double deltaTime()		const { return _display.deltaTime; }
	inline double frameUpdate()		const { return _display.frameUpdate; }
	inline double frameRender()		const { return _display.frameRender; }
	inline double win32Events()		const { return _display.win32Events; }
	inline double imguiLogic()		const { return _display.imguiLogic; }
	inline double imguiRender()		const { return _display.imguiRender; }
	inline double swapBuffer()		const { return _display.swapBuffer; }
	inline double imguiGPU()		const { return _display.imguiGPU; }
	inline double appGPU()			const { return _display.appGPU; }

private:
	friend struct GPUStage;
	friend struct CPUStage;

	int				_frameCounter = 0;
	int				_displayFrequency = 0;
	double			_frameBudget = 0;

	bool			_isSlowFrame	: 1;
	bool			_isFirstSample	: 1;

	FrameTimer		_display;
	FrameTimer		_accumulator;

	void _calcDisplayFrequencyAndBudget(HWND hwnd);
};

} // namespace