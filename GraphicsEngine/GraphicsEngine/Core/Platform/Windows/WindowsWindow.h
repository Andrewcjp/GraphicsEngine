#pragma once
#include "../Core/MinWindows.h"
#include "../Core/EngineTypes.h"
#include "../Generic/GenericWindow.h"
class WindowsWindow
{
private:

	static WindowsWindow*		app;
	class Engine*				m_engine;
	BOOL						m_terminate;
	BOOL						MyRegisterClass(HINSTANCE hinst);
	int							mwidth, mheight;
	HWND						HWindow;
	HINSTANCE					m_hInst;
	HKL							Layout;
	void						AddMenus(HWND hwnd);
	HCURSOR						Cursor;
	HCURSOR						Cursors[GenericWindow::CursorType::Limit];

public:	
	WindowsWindow();
	virtual						~WindowsWindow();
	void						GetDesktopResolution(int& horizontal, int& vertical);
	IntPoint					GetMousePos();
	void						SetMousePos(IntPoint& point);
	char						GetVirtualKeyAsChar(unsigned int key);	
	short						GetCharAsVirtualKey(char c);
	bool						IsKeyDown(short key);
	static void					SetCursorType(GenericWindow::CursorType Type);
	static WindowsWindow*		CreateApplication(Engine * EnginePtr, HINSTANCE hInst, LPSTR args, int nshow);
	void						SetVisible(bool visible);
	bool						CreateOSWindow(int width, int height);
	static HWND					GetHWND();
	static void					DestroyApplication();
	static WindowsWindow*		GetApplication();
	int							Run();
	void						Kill();
	static bool					IsActiveWindow();

	static	LRESULT CALLBACK	WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};
typedef WindowsWindow PlatformWindow;