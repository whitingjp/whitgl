#include <GLFW/glfw3.h>

#include <whitgl/input.h>

bool _heldInputs[WHITGL_INPUT_MAX];
bool _pressedInputs[WHITGL_INPUT_MAX];

extern GLFWwindow* _window;

void whitgl_input_init()
{
	int i;
	for(i=0; i<WHITGL_INPUT_MAX; i++)
	{
		_heldInputs[i] = false;
		_pressedInputs[i] = false;
	}

	// Detect key presses between calls to GetKey
	// glfwSetInputMode (_window, GLFW_STICKY_KEYS, GL_TRUE);
}

void whitgl_input_shutdown() {}

bool whitgl_input_down(whitgl_input input)
{
	return _heldInputs[input];
}

bool whitgl_input_pressed(whitgl_input input)
{
	return _pressedInputs[input];
}

whitgl_ivec whitgl_input_mouse_pos(int pixel_size)
{
	whitgl_ivec out;
	double x, y;
	glfwGetCursorPos(_window, &x, &y);
	out.x = (int)x;
	out.y = (int)y;
	out.x /= pixel_size;
	out.y /= pixel_size;
	return out;
}

bool _press(int key)
{
	return glfwGetKey(_window, key) == GLFW_PRESS;
}

bool _mousepress(int button)
{
	return glfwGetMouseButton(_window, button) == GLFW_PRESS;
}

void whitgl_input_update()
{
	int i;
	bool _oldInputs[WHITGL_INPUT_MAX];
	for(i=0; i<WHITGL_INPUT_MAX; i++)
		_oldInputs[i] = _heldInputs[i];

	_heldInputs[WHITGL_INPUT_UP] = _press(GLFW_KEY_UP) || _press('K') || _press('W') || _press(GLFW_KEY_KP_8);
	_heldInputs[WHITGL_INPUT_RIGHT] = _press(GLFW_KEY_RIGHT) || _press('L') || _press('D') || _press(GLFW_KEY_KP_6);
	_heldInputs[WHITGL_INPUT_DOWN] = _press(GLFW_KEY_DOWN) || _press('J') || _press('S') || _press(GLFW_KEY_KP_2);
	_heldInputs[WHITGL_INPUT_LEFT] = _press(GLFW_KEY_LEFT) || _press('H') || _press('A') || _press(GLFW_KEY_KP_4);
	_heldInputs[WHITGL_INPUT_ESC] = _press(GLFW_KEY_ESCAPE);
	_heldInputs[WHITGL_INPUT_A] = _press('Z');
	_heldInputs[WHITGL_INPUT_B] = _press('X');
	_heldInputs[WHITGL_INPUT_X] = _press('C');
	_heldInputs[WHITGL_INPUT_Y] = _press('V');
	_heldInputs[WHITGL_INPUT_START] = _press('B');
	_heldInputs[WHITGL_INPUT_SELECT] = _press('N');
	_heldInputs[WHITGL_INPUT_MOUSE_LEFT] = _mousepress(GLFW_MOUSE_BUTTON_1);
	_heldInputs[WHITGL_INPUT_MOUSE_RIGHT] = _mousepress(GLFW_MOUSE_BUTTON_2);
	_heldInputs[WHITGL_INPUT_MOUSE_MIDDLE] = _mousepress(GLFW_MOUSE_BUTTON_3);
	if(glfwJoystickPresent(GLFW_JOYSTICK_1))
	{
		float dead = 0.4f;
		int count;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
		if(count >= 2)
		{
			_heldInputs[WHITGL_INPUT_UP] |= axes[1] < -dead;
			_heldInputs[WHITGL_INPUT_RIGHT] |= axes[0] > dead;
			_heldInputs[WHITGL_INPUT_DOWN] |= axes[1] > dead;
			_heldInputs[WHITGL_INPUT_LEFT] |= axes[0] < -dead;
		}
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1,&count);
		if(count >= 4)
		{
			_heldInputs[WHITGL_INPUT_A] = buttons[0];
			_heldInputs[WHITGL_INPUT_B] = buttons[1];
			_heldInputs[WHITGL_INPUT_X] = buttons[2];
			_heldInputs[WHITGL_INPUT_Y] = buttons[3];
		}
	}

	_heldInputs[WHITGL_INPUT_ANY] = false;
	for(i=0; i<WHITGL_INPUT_ANY; i++)
		if(_heldInputs[i])
			_heldInputs[WHITGL_INPUT_ANY] = true;

	for(i=0; i<WHITGL_INPUT_MAX; i++)
		_pressedInputs[i] = !_oldInputs[i] && _heldInputs[i];
}