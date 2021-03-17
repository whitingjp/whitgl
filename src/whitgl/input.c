#include <GLFW/glfw3.h>

#include <whitgl/input.h>
#include <whitgl/logging.h>

#include <string.h>

bool _heldInputs[WHITGL_INPUT_MAX];
bool _pressedInputs[WHITGL_INPUT_MAX];
whitgl_fvec _joystick;
whitgl_fvec _joystick2;
whitgl_fvec _joystick3;
whitgl_float _scroll;
whitgl_ivec _mouse_pos;

extern GLFWwindow* _window;

void _whitgl_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)window;
	(void)xoffset;
	_scroll += yoffset;
}

void whitgl_input_init()
{
	int i;
	for(i=0; i<WHITGL_INPUT_MAX; i++)
	{
		_heldInputs[i] = false;
		_pressedInputs[i] = false;
	}
	_joystick = whitgl_fvec_zero;
	// Detect key presses between calls to GetKey
	// glfwSetInputMode (_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(_window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	glfwSetScrollCallback(_window, _whitgl_scroll_callback);
}

void whitgl_input_shutdown()
{
	glfwSetScrollCallback(_window, NULL);
}

bool whitgl_input_held(whitgl_input input)
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
	out.x = _mouse_pos.x/pixel_size;
	out.y = _mouse_pos.y/pixel_size;
	return out;
}


whitgl_fvec whitgl_input_joystick()
{
	return _joystick;
}
whitgl_fvec whitgl_input_joystick2()
{
	return _joystick2;
}
whitgl_fvec whitgl_input_joystick3()
{
	return _joystick3;
}

bool _press(int key)
{
	return glfwGetKey(_window, key) == GLFW_PRESS;
}

bool _mousepress(int button)
{
	return glfwGetMouseButton(_window, button) == GLFW_PRESS;
}

whitgl_float _deadzone(float value)
{
	float dead = 0.4;
	float mag = 1.5;
	if(value > 0)
	{
		if(value < dead) return 0;
		value = (value-dead)/(1-dead)*mag;
		return whitgl_fmin(1, value);
	} else
	{
		if(value > -dead) return 0;
		value = (value+dead)/(1-dead)*mag;
		return whitgl_fmax(-1, value);
	}
}

void whitgl_input_update()
{
	int i;
	bool _oldInputs[WHITGL_INPUT_MAX];
	for(i=0; i<WHITGL_INPUT_MAX; i++)
		_oldInputs[i] = _heldInputs[i];

	_heldInputs[WHITGL_INPUT_KEYBOARD_UP] = _press(GLFW_KEY_UP) || _press('K') || _press('W') || _press(GLFW_KEY_KP_8);
	_heldInputs[WHITGL_INPUT_KEYBOARD_RIGHT] = _press(GLFW_KEY_RIGHT) || _press('L') || _press('D') || _press(GLFW_KEY_KP_6);
	_heldInputs[WHITGL_INPUT_KEYBOARD_DOWN] = _press(GLFW_KEY_DOWN) || _press('J') || _press('S') || _press(GLFW_KEY_KP_2);
	_heldInputs[WHITGL_INPUT_KEYBOARD_LEFT] = _press(GLFW_KEY_LEFT) || _press('H') || _press('A') || _press(GLFW_KEY_KP_4);
	_heldInputs[WHITGL_INPUT_UP] = _heldInputs[WHITGL_INPUT_KEYBOARD_UP];
	_heldInputs[WHITGL_INPUT_RIGHT] = _heldInputs[WHITGL_INPUT_KEYBOARD_RIGHT];
	_heldInputs[WHITGL_INPUT_DOWN] = _heldInputs[WHITGL_INPUT_KEYBOARD_DOWN];
	_heldInputs[WHITGL_INPUT_LEFT] = _heldInputs[WHITGL_INPUT_KEYBOARD_LEFT];
	_heldInputs[WHITGL_INPUT_ESC] = _press(GLFW_KEY_ESCAPE);
	_heldInputs[WHITGL_INPUT_A] = _press('Z') || _press(' ') || _press(GLFW_KEY_ENTER);
	_heldInputs[WHITGL_INPUT_B] = _press('X');
	_heldInputs[WHITGL_INPUT_X] = _press('C');
	_heldInputs[WHITGL_INPUT_Y] = _press('V');
	_heldInputs[WHITGL_INPUT_START] = _press('B');
	_heldInputs[WHITGL_INPUT_SELECT] = _press('N');
	_heldInputs[WHITGL_INPUT_Z_UP] = _press('Q');
	_heldInputs[WHITGL_INPUT_Z_DOWN] = _press('E');
	_heldInputs[WHITGL_INPUT_0] = _press('0');
	_heldInputs[WHITGL_INPUT_1] = _press('1');
	_heldInputs[WHITGL_INPUT_2] = _press('2');
	_heldInputs[WHITGL_INPUT_3] = _press('3');
	_heldInputs[WHITGL_INPUT_4] = _press('4');
	_heldInputs[WHITGL_INPUT_5] = _press('5');
	_heldInputs[WHITGL_INPUT_6] = _press('6');
	_heldInputs[WHITGL_INPUT_7] = _press('7');
	_heldInputs[WHITGL_INPUT_8] = _press('8');
	_heldInputs[WHITGL_INPUT_9] = _press('9');
	_heldInputs[WHITGL_INPUT_DEBUG] = _press(';');
	_heldInputs[WHITGL_INPUT_RESET] = _press('\'');
	_heldInputs[WHITGL_INPUT_MOUSE_LEFT] = _mousepress(GLFW_MOUSE_BUTTON_LEFT);
	_heldInputs[WHITGL_INPUT_MOUSE_RIGHT] = _mousepress(GLFW_MOUSE_BUTTON_RIGHT);
	_heldInputs[WHITGL_INPUT_MOUSE_MIDDLE] = _mousepress(GLFW_MOUSE_BUTTON_MIDDLE);
	_heldInputs[WHITGL_INPUT_MOUSE_SCROLL_UP] = _scroll < 0;
	_heldInputs[WHITGL_INPUT_MOUSE_SCROLL_DOWN] = _scroll > 0;
	_scroll = 0;

	GLFWgamepadstate state;
	if(glfwJoystickIsGamepad(GLFW_JOYSTICK_1) && glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
	{
		_joystick.x = _deadzone(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
		_joystick.y = _deadzone(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
		_joystick2.x = _deadzone(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]);
		_joystick2.y = _deadzone(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
		_heldInputs[WHITGL_INPUT_A] |= state.buttons[GLFW_GAMEPAD_BUTTON_A];
		_heldInputs[WHITGL_INPUT_B] |= state.buttons[GLFW_GAMEPAD_BUTTON_B];
		_heldInputs[WHITGL_INPUT_X] |= state.buttons[GLFW_GAMEPAD_BUTTON_X];
		_heldInputs[WHITGL_INPUT_Y] |= state.buttons[GLFW_GAMEPAD_BUTTON_Y];
		_heldInputs[WHITGL_INPUT_START] |= state.buttons[GLFW_GAMEPAD_BUTTON_START];
		_heldInputs[WHITGL_INPUT_SELECT] |= state.buttons[GLFW_GAMEPAD_BUTTON_BACK];
		if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]) _joystick.y = -1;
		if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT]) _joystick.x = 1;
		if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN]) _joystick.y = 1;
		if(state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT]) _joystick.x = -1;
	} else
	{
		_joystick.x = 0;
		_joystick.y = 0;
		_joystick2.x = 0;
		_joystick2.y = 0;
	}


	if(_heldInputs[WHITGL_INPUT_UP] && !_heldInputs[WHITGL_INPUT_DOWN]) _joystick.y = -1;
	if(_heldInputs[WHITGL_INPUT_RIGHT] && !_heldInputs[WHITGL_INPUT_LEFT]) _joystick.x = 1;
	if(_heldInputs[WHITGL_INPUT_DOWN] && !_heldInputs[WHITGL_INPUT_UP]) _joystick.y = 1;
	if(_heldInputs[WHITGL_INPUT_LEFT] && !_heldInputs[WHITGL_INPUT_RIGHT]) _joystick.x = -1;
	float button_dead = 0.1;
	if(_joystick.y < -button_dead) _heldInputs[WHITGL_INPUT_UP] = true;
	if(_joystick.x > button_dead) _heldInputs[WHITGL_INPUT_RIGHT] = true;
	if(_joystick.y > button_dead) _heldInputs[WHITGL_INPUT_DOWN] = true;
	if(_joystick.x < -button_dead) _heldInputs[WHITGL_INPUT_LEFT] = true;

	_heldInputs[WHITGL_INPUT_ANY] = false;
	for(i=0; i<WHITGL_INPUT_ANY; i++)
		if(_heldInputs[i])
			_heldInputs[WHITGL_INPUT_ANY] = true;

	for(i=0; i<WHITGL_INPUT_MAX; i++)
		_pressedInputs[i] = !_oldInputs[i] && _heldInputs[i];

	double x, y;
	glfwGetCursorPos(_window, &x, &y);
	_mouse_pos.x = (whitgl_int)x;
	_mouse_pos.y = (whitgl_int)y;
}
whitgl_input_joystick_style whitgl_input_get_joystick_style()
{
	if(!glfwJoystickPresent(GLFW_JOYSTICK_1))
		return WHITGL_JOYSTICK_NONE;
	const char* joyname = glfwGetJoystickName(GLFW_JOYSTICK_1);
	if(strncmp(joyname, "PLAYSTATION(R)3 Controller", 26)==0)
		return WHITGL_JOYSTICK_PLAYSTATION;
	if(strncmp(joyname, "Wireless Controller", 19)==0)
		return WHITGL_JOYSTICK_PLAYSTATION;
	return WHITGL_JOYSTICK_XBOX;
}
