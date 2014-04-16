#include <GL/glfw.h>

#include <jpw/input.h>

bool _heldInputs[JPW_INPUT_MAX];
bool _pressedInputs[JPW_INPUT_MAX];

void jpw_input_init()
{
	int i;
	for(i=0; i<JPW_INPUT_MAX; i++)
	{
		_heldInputs[i] = false;
		_pressedInputs[i] = false;
	}

	// Detect key presses between calls to GetKey
	glfwEnable( GLFW_STICKY_KEYS );
}

void jpw_input_shutdown() {}

bool jpw_input_down(jpw_input input)
{
	return _heldInputs[input];
}

bool jpw_input_pressed(jpw_input input)
{
	return _pressedInputs[input];
}

// Point jpw_input_mousePos()
// {
// 	Point out;
// 	glfwGetMousePos(&out.x, &out.y);
// 	out = pointAddPoint(out, pointInverse(_drawRect.a));  
// 	out.x /= _pixel_scale;
// 	out.y /= _pixel_scale;
// 	return out;
// }

bool _press(int key)
{
	return glfwGetKey(key) == GLFW_PRESS;
}


void jpw_input_update()
{
	int i;
	bool _oldInputs[JPW_INPUT_MAX];
	for(i=0; i<JPW_INPUT_MAX; i++)
		_oldInputs[i] = _heldInputs[i];
	  
	_heldInputs[JPW_INPUT_UP] = _press(GLFW_KEY_UP) || _press('K') || _press('W') || _press(GLFW_KEY_KP_8);
	_heldInputs[JPW_INPUT_RIGHT] = _press(GLFW_KEY_RIGHT) || _press('L') || _press('D') || _press(GLFW_KEY_KP_6);
	_heldInputs[JPW_INPUT_DOWN] = _press(GLFW_KEY_DOWN) || _press('J') || _press('S') || _press(GLFW_KEY_KP_2);
	_heldInputs[JPW_INPUT_LEFT] = _press(GLFW_KEY_LEFT) || _press('H') || _press('A') || _press(GLFW_KEY_KP_4);
	_heldInputs[JPW_INPUT_ESC] = _press(GLFW_KEY_ESC);
	_heldInputs[JPW_INPUT_A] = _press('Z');
	_heldInputs[JPW_INPUT_B] = _press('X');
	_heldInputs[JPW_INPUT_X] = _press('C');
	_heldInputs[JPW_INPUT_Y] = _press('V');
	_heldInputs[JPW_INPUT_START] = _press('B');
	_heldInputs[JPW_INPUT_SELECT] = _press('N');

	_heldInputs[JPW_INPUT_ANY] = false;
	for(i=0; i<JPW_INPUT_ANY; i++)
		if(_heldInputs[i])
			_heldInputs[JPW_INPUT_ANY] = true;
	
	for(i=0; i<JPW_INPUT_MAX; i++)
		_pressedInputs[i] = !_oldInputs[i] && _heldInputs[i];
}