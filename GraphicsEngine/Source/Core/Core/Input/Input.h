#pragma once
#include "include\glm\fwd.hpp"
#include <map>
#include "Core/EngineTypes.h"
#include "KeyCode.h"
const int MAX_MOUSE_BUTTON_COUNT = 10;
class Input
{
public:
	static Input* instance;
	Input();
	~Input();
	void Clear();

	//input processing
	CORE_API static bool GetKeyDown(int c);
	CORE_API static bool GetKey(char c);
	CORE_API static bool GetVKey(short key);	
	CORE_API static glm::vec2 GetMouseInputAsAxis();
	CORE_API static glm::vec2 GetMousePos();

	void   ProcessInput(const float delatime);
	bool   MouseLBDown(int x, int y);
	bool   MouseLBUp(int x, int y);
	bool   MouseMove(int x, int y, double deltatime);
	bool   ProcessKeyDown(unsigned int key);
	void   LockCursor(bool state);
	static void ReciveMouseDownMessage(int Button, bool state);
	static bool GetMouseButtonDown(int button);
	static void SetCursorVisible(bool state);
	
private:
	bool LockMouse = false;
	glm::vec2 MouseAxis;
	glm::vec2 MousePosScreen;
	std::map<int, bool> KeyMap;	
	bool IsActiveWindow = false;
	IntPoint CentrePoint;
	bool MouseKeyData[MAX_MOUSE_BUTTON_COUNT] = { false };
};

