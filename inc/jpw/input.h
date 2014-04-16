#include <stdbool.h>

typedef enum
{
	// game
	JPW_INPUT_UP = 0,
	JPW_INPUT_RIGHT,
	JPW_INPUT_DOWN,
	JPW_INPUT_LEFT,
	JPW_INPUT_ESC,
	JPW_INPUT_A,
	JPW_INPUT_B,
	JPW_INPUT_X,
	JPW_INPUT_Y,
	JPW_INPUT_START,
	JPW_INPUT_SELECT,
	JPW_INPUT_ANY,
	JPW_INPUT_MAX,
} jpw_input;

void jpw_input_init();
void jpw_input_shutdown();
void jpw_input_update();
bool jpw_input_down(jpw_input input);
bool jpw_input_pressed(jpw_input input);
// Point jpw_input_mousePos();