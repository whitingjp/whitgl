#include <stdbool.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw.h>

#include <whitgl/logging.h>
#include <whitgl/sys.h>

bool _shouldClose;
int _pixel_scale;
whitgl_ivec _window_size;

whitgl_sys_setup whitgl_default_setup =
{
	"default window name",
	{120, 80},
	4,
	false,
	false,
};

const char* vertex_src = "\
#version 150\
\n\
\
in vec2 position;\
in vec2 texturepos;\
out vec2 Texturepos;\
void main()\
{\
    gl_Position = vec4( position, 0.0, 1.0 );\
    Texturepos = texturepos;\
}\
";

const char* fragment_src = "\
#version 150\
\n\
in vec2 Texturepos;\
out vec4 outColor;\
uniform sampler2D tex;\
void main()\
{\
	outColor = texture( tex, Texturepos );\
}\
";

const char* flat_src = "\
#version 150\
\n\
uniform vec4 sColor;\
out vec4 outColor;\
void main()\
{\
	outColor = sColor;\
}\
";

float fullVertices[] = {
	-1.0f,  1.0f, 0, 0, // Vertex 1 (X, Y)
	1.0f, -1.0f, 1, 1,// Vertex 2 (X, Y)
	-1.0f, -1.0f, 0, 1,  // Vertex 3 (X, Y)
	-1.0f,  1.0f, 0, 0, // Vertex 1 (X, Y)
	1.0f, 1.0f, 1, 0,// Vertex 2 (X, Y)
	1.0f, -1.0f, 1, 1,  // Vertex 3 (X, Y)
};

GLuint vbo;
GLuint shaderProgram;
GLuint flatShaderProgram;
GLuint tex1;

int GLFWCALL _whitgl_sys_close_callback();

unsigned char* screen;

whitgl_sys_setup _setup;

bool whitgl_sys_init(whitgl_sys_setup setup)
{
	_setup = setup;

	bool result;
	_shouldClose = false;

	WHITGL_LOG("Initialize GLFW");

	result = glfwInit();
	if(!result)
	{
		WHITGL_LOG("Failed to initialize GLFW");
		exit( EXIT_FAILURE );
	}

	glfwOpenWindowHint( GLFW_OPENGL_VERSION_MAJOR, 3 );
	glfwOpenWindowHint( GLFW_OPENGL_VERSION_MINOR, 2 );
	glfwOpenWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_TRUE );

	GLFWvidmode desktop;
	glfwGetDesktopMode( &desktop );

	_pixel_scale = setup.pixel_size;
	if(setup.fullscreen)
	{
		result = glfwOpenWindow( desktop.Width, desktop.Height, desktop.RedBits,
		                         desktop.GreenBits, desktop.BlueBits, 8, 32, 0,
		                         GLFW_FULLSCREEN );
		_pixel_scale = desktop.Width/setup.size.x;
	} else
	{
		result = glfwOpenWindow( setup.size.x*_pixel_scale, setup.size.y*_pixel_scale,
		                         0,0,0,0, 0,0, GLFW_WINDOW );
	}
	glfwSetWindowTitle(setup.name);
	if(!result)
	{
		WHITGL_LOG("Failed to open GLFW window");
		exit( EXIT_FAILURE );
	}

	glewExperimental = GL_TRUE;
	glewInit();

	glGenBuffers( 1, &vbo ); // Generate 1 buffer

	GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertexShader, 1, &vertex_src, NULL );
	glCompileShader( vertexShader );
	GLint status;
	glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &status );
	if(status != GL_TRUE)
	{
		char buffer[512];
		glGetShaderInfoLog( vertexShader, 512, NULL, buffer );
		WHITGL_LOG(buffer);
		return false;
	}

	GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragmentShader, 1, &fragment_src, NULL );
	glCompileShader( fragmentShader );

	glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &status );
	if(status != GL_TRUE)
	{
		char buffer[512];
		glGetShaderInfoLog( fragmentShader, 512, NULL, buffer );
		WHITGL_LOG(buffer);
		return false;
	}

	GLuint flatShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( flatShader, 1, &flat_src, NULL );
	glCompileShader( flatShader );

	glGetShaderiv( flatShader, GL_COMPILE_STATUS, &status );
	if(status != GL_TRUE)
	{
		char buffer[512];
		glGetShaderInfoLog( flatShader, 512, NULL, buffer );
		WHITGL_LOG(buffer);
		return false;
	}

	shaderProgram = glCreateProgram();
	glAttachShader( shaderProgram, vertexShader );
	glAttachShader( shaderProgram, fragmentShader );
	glBindFragDataLocation( shaderProgram, 0, "outColor" );
	glLinkProgram( shaderProgram );

	flatShaderProgram = glCreateProgram();
	glAttachShader( flatShaderProgram, vertexShader );
	glAttachShader( flatShaderProgram, flatShader );
	glBindFragDataLocation( flatShaderProgram, 0, "outColor" );
	glLinkProgram( flatShaderProgram );

	glGenTextures(1, &tex1);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Enable vertical sync (on cards that support it)
	glfwSwapInterval( 1 );

	glfwSetWindowCloseCallback(_whitgl_sys_close_callback);

	if(setup.disable_mouse_cursor)
		glfwDisable(GLFW_MOUSE_CURSOR);

	screen = malloc(sizeof(unsigned char)*setup.size.x*setup.size.y*3);

	return true;
}

int GLFWCALL _whitgl_sys_close_callback()
{
  _shouldClose = true;
  return true;
}

bool whitgl_sys_should_close()
{
	return _shouldClose;
}

void whitgl_sys_close()
{
	glfwTerminate();
}

double whitgl_sys_getTime()
{
	return glfwGetTime();
}

void whitgl_sys_sleep(double time)
{
	glfwSleep(time);
}

void whitgl_sys_draw_init()
{
	int w, h;
	glfwGetWindowSize( &w, &h);
	_window_size.x = w;
	_window_size.y = h;
	glViewport( 0, 0, _window_size.x, _window_size.y );

	glClearColor(0, 0.0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, _window_size.x, _window_size.y,0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static const int kNumColors = 4;
int colors[12] =
{
	0x2b, 0x12, 0x33,
	0x28, 0x31, 0x74,
	0xc1, 0x24, 0x8f,
	0xf8, 0xd5, 0x5a,
};

void whitgl_sys_draw_finish()
{
	int i;
	for(i=0; i<_setup.size.x*_setup.size.y; i++)
	{
		int x = i%_setup.size.x;

		int ai = x/(_setup.size.x/kNumColors);
		screen[i*3] = colors[ai*3+0];
		screen[i*3+1] = colors[ai*3+1];
		screen[i*3+2] = colors[ai*3+2];
	}
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( fullVertices ), fullVertices, GL_DYNAMIC_DRAW );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, tex1 );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _setup.size.x, _setup.size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, screen);

	glUseProgram( shaderProgram );
	glUniform1i( glGetUniformLocation( shaderProgram, "tex" ), 0 );

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0 );
	glEnableVertexAttribArray( posAttrib );

	GLint texturePosAttrib = glGetAttribLocation( shaderProgram, "texturepos" );
	glVertexAttribPointer( texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), BUFFER_OFFSET(sizeof(float)*2) );
	glEnableVertexAttribArray( texturePosAttrib );

	glDrawArrays( GL_TRIANGLES, 0, 6 );

	whitgl_iaabb rect = whitgl_iaabb_zero;
	rect.a.x = 10;
	rect.a.y = 10;
	rect.b.x = _setup.size.x-2;
	rect.b.y = _setup.size.y-2;
	whitgl_sys_color col = whitgl_sys_color_zero;
	col.r = 255;
	col.g = 10;
	col.b = 120;
	col.a = 255;
	whitgl_sys_draw_iaabb(rect, col);
	col.g = 255;
	rect.a.x = 2;
	rect.a.y = 2;
	rect.b.x = 20;
	rect.b.y = 20;
	whitgl_sys_draw_iaabb(rect, col);

	glfwSwapBuffers();
	glDisable(GL_BLEND);
}


whitgl_ivec _sys_world_ivec_to_screen_ivec(whitgl_ivec p)
{
	p.x *= _pixel_scale;
	p.y *= _pixel_scale;
	return p;
}

whitgl_iaabb _sys_world_iaabb_to_screen_iaabb(whitgl_iaabb r)
{
	r.a = _sys_world_ivec_to_screen_ivec(r.a);
	r.b = _sys_world_ivec_to_screen_ivec(r.b);
	return r;
}

void _whitgl_sys_draw_screen_iaabb(whitgl_iaabb rect, whitgl_sys_color col)
{
	whitgl_faabb r;
	r.a.x = (whitgl_float)rect.a.x / (_setup.size.x*2) - 1.0f;
	r.a.y = (whitgl_float)rect.a.y / (_setup.size.y*2) - 1.0f;
	r.b.x = (whitgl_float)rect.b.x / (_setup.size.x*2) - 1.0f;
	r.b.y = (whitgl_float)rect.b.y / (_setup.size.y*2) - 1.0f;
	r.a.y = -r.a.y;
	r.b.y = -r.b.y;

	float vertices[6*2];
	vertices[ 0] = r.a.x; vertices[ 1] = r.b.y;
	vertices[ 2] = r.b.x; vertices[ 3] = r.a.y;
	vertices[ 4] = r.a.x; vertices[ 5] = r.a.y;

	vertices[ 6] = r.a.x; vertices[ 7] = r.b.y;
	vertices[ 8] = r.b.x; vertices[ 9] = r.b.y;
	vertices[10] = r.b.x; vertices[11] = r.a.y;
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW );

	glUseProgram( flatShaderProgram );
	glUniform4f( glGetUniformLocation( flatShaderProgram, "sColor" ), (float)col.r/255.0, (float)col.g/255.0, (float)col.b/255.0, (float)col.a/255.0 );

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( flatShaderProgram, "position" );
	glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0 );
	glEnableVertexAttribArray( posAttrib );

	glDrawArrays( GL_TRIANGLES, 0, 6 );
}

void whitgl_sys_draw_iaabb(whitgl_iaabb rectangle, whitgl_sys_color col)
{
  rectangle = _sys_world_iaabb_to_screen_iaabb(rectangle);
  _whitgl_sys_draw_screen_iaabb(rectangle, col);
}
