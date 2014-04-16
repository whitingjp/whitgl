#include <stdbool.h>
#include <stdlib.h>

#include <GL/glfw.h>

#include <jpw/logging.h>
#include <jpw/sys.h>

bool _shouldClose;
int _pixel_scale;
int _window_width;
int _window_height;

jpw_sys_setup jpw_default_setup =
{
	"default window name",
	120,
	80,
	4,
	false,
	false,
};

int GLFWCALL _jpw_sys_close_callback();

void jpw_sys_init(jpw_sys_setup setup)
{
	bool result;
	_shouldClose = false;
	
	JPW_LOG("Initialize GLFW");

	result = glfwInit();
	if(!result)
	{
		JPW_LOG("Failed to initialize GLFW");
		exit( EXIT_FAILURE );
	}

	GLFWvidmode desktop;
	glfwGetDesktopMode( &desktop );

	_pixel_scale = setup.pixel_size;
	if(setup.fullscreen)
	{
		result = glfwOpenWindow( desktop.Width, desktop.Height, desktop.RedBits,
		                         desktop.GreenBits, desktop.BlueBits, 8, 32, 0, 
		                         GLFW_FULLSCREEN );
		_pixel_scale = desktop.Width/setup.width;
	} else
	{
		result = glfwOpenWindow( setup.width*_pixel_scale, setup.height*_pixel_scale,
		                         0,0,0,0, 0,0, GLFW_WINDOW );
	}  
	glfwSetWindowTitle(setup.name);
	if(!result)
	{
		JPW_LOG("Failed to open GLFW window");
		exit( EXIT_FAILURE );
	}
	
	// Enable vertical sync (on cards that support it)
	glfwSwapInterval( 1 );
	
	glfwSetWindowCloseCallback(_jpw_sys_close_callback);

	if(setup.disable_mouse_cursor)
		glfwDisable(GLFW_MOUSE_CURSOR);
}

int GLFWCALL _jpw_sys_close_callback()
{
  _shouldClose = true;
  return true;
}

bool jpw_sys_should_close()
{
	return _shouldClose;
}

void jpw_sys_close()
{
	glfwTerminate();
}

double jpw_sys_getTime()
{
	return glfwGetTime();
}

void jpw_sys_sleep(double time)
{
	glfwSleep(time);
}

void jpw_sys_draw_init()
{
	glfwGetWindowSize( &_window_width, &_window_height);  
	glViewport( 0, 0, _window_width, _window_height ); 
	
	glClearColor(0, 0, 0, 0);
	glClear( GL_COLOR_BUFFER_BIT );
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, _window_width, _window_height,0);
}

void jpw_sys_draw_finish()
{
	glfwSwapBuffers();
}
