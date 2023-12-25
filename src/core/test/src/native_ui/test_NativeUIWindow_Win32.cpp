#include <sge_core/base/UnitTest.h>
#include <sge_core/string/UtfUtil.h>
#include <sge_core/pointer/ComPtr.h>

#include <windowsx.h>
#include <windows.h>

#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")		// include the Direct3D Library file
//#pragma comment (lib, "d3dx11.lib")	// DXD11_SDK: http://www.directxtutorial.com/lessonarticle.aspx?id=4

// https://docs.microsoft.com/en-us/windows/win32/api/d3dcompiler/nf-d3dcompiler-d3dcompile
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <sge_core/base/Error.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include <tchar.h>

// define the screen resolution (bad practics)
/*
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
*/

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace sge {

class Test_NativeUIWindow_Win32 : public UnitTestBase {
private:

	// define constant (for type tracking)
	static const int SCREEN_WIDTH = 800;
	static const int SCREEN_HEIGHT = 800;

	static LRESULT WINAPI s_MyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	IDXGISwapChain* swapchain;
	ID3D11Device* dev;
	ID3D11DeviceContext* devcon;
	ID3D11RenderTargetView* backbuffer;
	ID3D11Debug* debug;

	ID3D11VertexShader* pVS;
	ID3D11PixelShader* pPS;

	ID3D11Buffer* pVBuffer;
	ID3D11InputLayout* pLayout;

	static LRESULT WINAPI s_wndProcImGUI(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_SIZE:
			if (s_instance->dev != NULL && wParam != SIZE_MINIMIZED)
			{
				s_instance->CleanupRenderTarget();
				s_instance->swapchain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				s_instance->CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

public:

	static Test_NativeUIWindow_Win32* s_instance;
	static Test_NativeUIWindow_Win32* instance() { return s_instance; };

	Test_NativeUIWindow_Win32::Test_NativeUIWindow_Win32() {
		SGE_ASSERT(s_instance == nullptr);
		s_instance = this;
	}

	Test_NativeUIWindow_Win32::~Test_NativeUIWindow_Win32() {
		SGE_ASSERT(s_instance == this);
		s_instance = nullptr;
	}

	struct VERTEX
	{
		FLOAT X, Y, Z;      // position
		FLOAT Color[4];     // color
	};

	// test_helloWorld
	LRESULT WINAPI MyMessageBox(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	void test_helloWorld() {
		MyMessageBox(GetModuleHandle(nullptr), 0, GetCommandLineA(), 0);
	}

	// test_createWindow
	LRESULT WINAPI MyWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	LRESULT CALLBACK MyWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void test_createWindow() {
		// http://www.directxtutorial.com/Lesson.aspx?lessonid=11-1-3
		MyWindow(GetModuleHandle(nullptr), 0, GetCommandLineA(), SW_NORMAL);
	}

	// test_drawD3DTriangle
	LRESULT WINAPI Test_NativeUIWindow_Win32::MyWindowWithD3D(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	void InitD3D(HWND hWnd);     // sets up and initializes Direct3D
	void CleanD3D(void);         // closes Direct3D and releases memory
	void RenderFrame(void);      // used to render a single frame
	void InitPipeline();
	void InitGraphics();
	void test_drawD3DTriangle() {
		// http://www.directxtutorial.com/Lesson.aspx?lessonid=11-4-2
		// http://www.directxtutorial.com/Lesson.aspx?lessonid=11-4-5
		MyWindowWithD3D(GetModuleHandle(nullptr), 0, GetCommandLineA(), SW_NORMAL);
	}

	void CreateRenderTarget();
	void CleanupRenderTarget();
	LRESULT WINAPI Test_NativeUIWindow_Win32::MyWindowWithImGUI(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	void test_imgui() {
		// https://github.com/ocornut/imgui/blob/master/examples/example_win32_directx11/main.cpp
		MyWindowWithImGUI(GetModuleHandle(nullptr), 0, GetCommandLineA(), SW_NORMAL);
	}
		
}; // Test_NativeUIWindow_Win32

LRESULT CALLBACK Test_NativeUIWindow_Win32::s_MyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		// we return '0'.This is rather important,
		// because it tells Windows that we handled the message.
		// If we returned something else, Windows could get confused
		PostQuitMessage(0);
		return 0;
	}
	default:
		break;
	}

	// DefWindowProc: What this function does is handle any messages we did not handle ourselves.
	// In short, it handles messages that we didn't return a '0' for.
	// We therefore place it at the end of the WindowProc() function so that it catches anything we miss.
	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT WINAPI Test_NativeUIWindow_Win32::MyMessageBox(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MessageBox(NULL,
		L"Hello World!",
		L"Just another Hello World program!",
		MB_ICONEXCLAMATION | MB_OK);

	// return 0 to Windows
	return (LRESULT)0;
}

// the entry point for any Windows32 program
LRESULT WINAPI Test_NativeUIWindow_Win32::MyWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// this struct holds information for the window class
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX)); // clear out the window class for use

	LPTCH szClassName = L"Test_NativeUIWindow_Win32";

	wc.cbSize                = sizeof(WNDCLASSEX);
	wc.style                 = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc           = s_MyWndProc;
	wc.hInstance             = hInstance;
	wc.hCursor               = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground         = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName         = szClassName;

	RegisterClassEx(&wc); // register the window class

	RECT wr = { 0, 0, 500, 400 };								// set the size, but not the position. wr means window rect
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);			// adjust the size

	// create the window and use the result as the handle
	HWND hWnd = CreateWindowEx(0,
							szClassName,						// name of the window class
							L"Test_NativeUIWindow_Win32-Title",	// title of the window
							WS_OVERLAPPEDWINDOW,				// window style
							300, 300,							// position x y
							wr.right - wr.left,					// width of the window
							wr.bottom - wr.top,					// height of the window500, 400,
							NULL,								// we have no parent window, NULL
							NULL,								// we aren't using menus, NULL
							hInstance,							// application handle
							0);									// used with multiple windows, NULL

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg; // this struct holds Windows event messages

		// wait for the next message in the queue, store the result in 'msg'
	while (GetMessage(&msg, NULL, 0, 0))
	{
		// translate keystroke messages into the right format
		TranslateMessage(&msg);

		// send the message to the `wc.lpfnWndProc` function
		DispatchMessage(&msg);
	}

	//That way, our while () loop is only broken when the program is completely finished anyway
	return msg.wParam;
}

LRESULT WINAPI Test_NativeUIWindow_Win32::MyWindowWithD3D(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	LPTCH szClassName = L"Test_NativeUIWindow_Win32";

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = s_MyWndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// wc.hbrBackground = (HBRUSH)COLOR_WINDOW; // Modify the window to have no background, leaves the background color untouched
	wc.lpszClassName = szClassName;
	RegisterClassEx(&wc);
	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	// create the window and use the result as the handle
	HWND hWnd = CreateWindowEx(0,
		szClassName,					
		L"Test_NativeUIWindow_Win32(MyWindowWithD3D)",
		WS_OVERLAPPEDWINDOW,			
		300, 300,						
		wr.right - wr.left,				
		wr.bottom - wr.top,				
		NULL,							
		NULL,							
		hInstance,						
		0);								

	InitD3D(hWnd);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		RenderFrame();
	}
	CleanD3D();
	return msg.wParam;
}

// this function initializes and prepares Direct3D for use
void Test_NativeUIWindow_Win32::InitD3D(HWND hWnd)
{
	// Direct3D initialization -----------------------------------------
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd; // swap chain desc

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferDesc.Width = SCREEN_WIDTH;					// set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT;					// set the back buffer height
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
								D3D_DRIVER_TYPE_HARDWARE,
								NULL,
								NULL,
								NULL,
								NULL,
								D3D11_SDK_VERSION,
								&scd,
								&swapchain,
								&dev,
								NULL,
								&devcon);

	// Set the render target -----------------------------------------
	// get the address of the back buffer
	ID3D11Texture2D* pBackBuffer;

	// 0 is because BufferCount == 1, so index[0]
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);

	// Set the viewport -----------------------------------------
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	// The first parameter is the number of viewports being used,
	// and the second parameter is the address of a list of pointers to the viewport structs.
	devcon->RSSetViewports(1, &viewport);

	InitPipeline();
	InitGraphics();
}

// this is the function used to render a single frame
void Test_NativeUIWindow_Win32::RenderFrame(void)
{
	// clear the back buffer to a deep blue
	// devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f)); // https://stackoverflow.com/questions/14046179/new-equiavalent-to-d3dxcolor-structure-in-directx-11-1
	const float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devcon->ClearRenderTargetView(backbuffer, color);

	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

	// select which primtive type we are using
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	devcon->Draw(3, 0);

	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}

// this is the function that cleans up Direct3D and COM
void Test_NativeUIWindow_Win32::CleanD3D()
{
	swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

	// close and release all existing COM objects
	pVS->Release();
	pPS->Release();
	swapchain->Release();
	if (backbuffer) {
		backbuffer->Release();
	}
	
	dev->Release();
	devcon->Release();
}

void Test_NativeUIWindow_Win32::InitPipeline()
{
	// load and compile the two shaders file -> bytecode
	ID3D10Blob *VS, *PS;

	/* https://stackoverflow.com/questions/36876002/changing-over-from-d3dx-d3dcompile-not-found
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"shaders.shader",	// the name of the .shader file
							0,
							0,
							"PShader",			// the name of the shader
							"ps_4_0",			// ps = pixel shader, _4_0 stands for HLSL version
							0,
							0,
							0,
							&PS,				// a pointer to a blob object. (blob containing the compiled shader)
							0,
							0);

	*/
		
	wchar_t tmp[MAX_PATH + 1];
	::GetCurrentDirectory(MAX_PATH, tmp);

	String curDir = UtfUtil::toString(tmp); // sge/build\SimpleGameEngine-x64-windows\src\core\test
	String testDir = curDir + "/../../../../../src/core/test/src/native_ui/";
	TempStringW tmpStrW = UtfUtil::toStringW(testDir);
	::SetCurrentDirectory(tmpStrW.c_str());
	LPCWSTR shaderName = L"shaders.shader";

	SGE_LOG("testDir = {}", testDir);

	ID3D10Blob* errorMsg;
	HRESULT hr;
	hr = D3DCompileFromFile(shaderName, 0, 0, "VShader", "vs_4_0", 0, 0, &VS, &errorMsg);

	if (!SUCCEEDED(hr)) {
		auto* d = debug;
		if (d) {
			d->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}
		throw SGE_ERROR("VShader HRESULT = {}", hr);
	}

	hr = D3DCompileFromFile(shaderName, 0, 0, "PShader", "ps_4_0", 0, 0, &PS, &errorMsg);

	if (!SUCCEEDED(hr)) {
		auto* d = debug;
		if (d) {
			d->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}

		throw SGE_ERROR("PShader HRESULT = {}", hr);
	}

	// encapsulate both shaders into shader objects (create the shader objects)
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(),	// compiled data(bytecode)
							PS->GetBufferSize(),	// size of the file data, length of the bytecode
							NULL,
							&pPS);

	// set the shader objects
	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS,						// the address of the shader object 
						0,
						0);

	D3D11_INPUT_ELEMENT_DESC ied[] = // input element desc
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{
			"COLOR",                        // Semantic: POSITION, POSITIONT, COLOR, PSIZE
			0,								// SemanticIndex: If we had a vertex that had two colors, they would both use the COLOR semantic.
			DXGI_FORMAT_R32G32B32A32_FLOAT, // the format of the data.
			0,
			12,                             // This means the position starts 0 bytes into the struct and the color starts 12 bytes into the struct.
			D3D11_INPUT_PER_VERTEX_DATA,    // what the element is used as
			0								// InstanceDataStepRate, cause not used with the D3D11_INPUT_PER_VERTEX_DATA flag, so set 0
		},
	};

	dev->CreateInputLayout(ied,						// pointer to the element description array
							2,						// the number of elements in the array, POSITION and COLOR, so set 2
							VS->GetBufferPointer(), // pointer to the first shader in the pipeline, Bytecode
							VS->GetBufferSize(),	// bytecode's length
							&pLayout);				//  the pointer of 'input layout object'

	devcon->IASetInputLayout(pLayout);
}

void Test_NativeUIWindow_Win32::InitGraphics() {
	// http://www.directxtutorial.com/Lesson.aspx?lessonid=11-4-5

	VERTEX OurVertices[] =
	{
		{0.0f,   0.5f,  0.0f, {1.0f, 0.0f, 0.0f, 1.0f} },
		{0.45f,  -0.5,  0.0f, {0.0f, 1.0f, 0.0f, 1.0f} },
		{-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f} },
	};

	D3D11_BUFFER_DESC bd; // buffer desc
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage          = D3D11_USAGE_DYNAMIC;       // write access access by CPU and GPU
	bd.ByteWidth      = sizeof(VERTEX) * 3;        // size is the VERTEX struct * 3
	bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;  // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer, use D3D11_CPU_ACCESS_WRITE, because we want to copy data from system memory into the buffer.

	// create the buffer
	dev->CreateBuffer(&bd,		// address of the description struct
					NULL,		// be used to initialize the buffer with certain data upon creation
					&pVBuffer);	// the address of the buffer object

	// fill the vertex buffer
	// Direct3D may be working with a buffer in the background,
	// it never gives you direct CPU access to it. In order to access it, the buffer must be mapped.
	D3D11_MAPPED_SUBRESOURCE ms; // mapped sub resource

	// map the buffer
	devcon->Map(pVBuffer,                // the address of the buffer object
				NULL,
				D3D11_MAP_WRITE_DISCARD, // allows us to control the CPUs access to the buffer while it's mapped
				NULL,					 // an another flag. It can be NULL or D3D11_MAP_FLAG_DO_NOT_WAIT. This flag forces the program to continue, even if the GPU is still working with the buffer.
				&ms);

	memcpy(ms.pData, OurVertices, sizeof(OurVertices));	// copy the data to the buffer
	devcon->Unmap(pVBuffer, NULL);						// unmap the buffer, This reallows the GPU access to the buffer, and reblocks the CPU
}

LRESULT WINAPI Test_NativeUIWindow_Win32::MyWindowWithImGUI(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	LPTCH szClassName = L"Test_NativeUIWindow_Win32";

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = s_wndProcImGUI;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = szClassName;
	RegisterClassEx(&wc);
	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	// create the window and use the result as the handle
	HWND hWnd = CreateWindowEx(0,
		szClassName,
		L"Test_NativeUIWindow_Win32(imgui)",
		WS_OVERLAPPEDWINDOW,
		300, 300,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		0);

	InitD3D(hWnd);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	CreateRenderTarget();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(dev, devcon);

	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	bool done = false;
	
	while (!done)
    {
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}

        if (done) break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        devcon->OMSetRenderTargets(1, &backbuffer, NULL);
        devcon->ClearRenderTargetView(backbuffer, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swapchain->Present(1, 0); // Present with vsync
        //swapchain->Present(0, 0); // Present without vsync
    }

	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupRenderTarget();
	CleanD3D();
	return 0;
}

void Test_NativeUIWindow_Win32::CreateRenderTarget() {
	ID3D11Texture2D* pBackBuffer;
	swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();
}

void Test_NativeUIWindow_Win32::CleanupRenderTarget() {
	if (backbuffer) {
		backbuffer->Release(); backbuffer = NULL;
	}
}

Test_NativeUIWindow_Win32* Test_NativeUIWindow_Win32::s_instance = nullptr;

} // namespace 

void test_NativeUIWindow_Win32() {
	using namespace sge;

	//SGE_TEST_CASE(Test_NativeUIWindow_Win32, test_helloWorld());

	//SGE_TEST_CASE(Test_NativeUIWindow_Win32, test_createWindow());

	//SGE_TEST_CASE(Test_NativeUIWindow_Win32, test_drawD3DTriangle());

	SGE_TEST_CASE(Test_NativeUIWindow_Win32, test_imgui());
}