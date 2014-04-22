#include <stdbool.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw.h>
#include <SOIL/SOIL.h>

#include <whitgl/logging.h>
#include <whitgl/sys.h>


bool _shouldClose;
int _pixel_scale;
whitgl_ivec _window_size;

typedef struct
{
	int id;
	GLuint gluint;
	whitgl_ivec size;
} whitgl_image;
#define WHITGL_IMAGE_MAX (64)
whitgl_image images[WHITGL_IMAGE_MAX];
int num_images;

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
uniform mat4 sLocalToProjMatrix;\
void main()\
{\
    gl_Position = sLocalToProjMatrix * vec4( position, 0.0, 1.0 );\
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


GLuint vbo;
GLuint shaderProgram;
GLuint flatShaderProgram;

int GLFWCALL _whitgl_sys_close_callback();

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

	// Enable vertical sync (on cards that support it)
	glfwSwapInterval( 1 );

	glfwSetWindowCloseCallback(_whitgl_sys_close_callback);

	if(setup.disable_mouse_cursor)
		glfwDisable(GLFW_MOUSE_CURSOR);

	int i;
	for(i=0; i<WHITGL_IMAGE_MAX; i++)
		images[i].id = -1;
	num_images = 0;

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

void _whitgl_populate_vertices(float* vertices, whitgl_iaabb r)
{
	vertices[ 0] = r.a.x; vertices[ 1] = r.b.y; vertices[ 2] = 0; vertices[ 3] = 1;
	vertices[ 4] = r.b.x; vertices[ 5] = r.a.y; vertices[ 6] = 1; vertices[ 7] = 0;
	vertices[ 8] = r.a.x; vertices[ 9] = r.a.y; vertices[10] = 0; vertices[11] = 0;

	vertices[12] = r.a.x; vertices[13] = r.b.y; vertices[14] = 0; vertices[15] = 1;
	vertices[16] = r.b.x; vertices[17] = r.b.y; vertices[18] = 1; vertices[19] = 1;
	vertices[20] = r.b.x; vertices[21] = r.a.y; vertices[22] = 1; vertices[23] = 0;
}


void _whitgl_sys_orthographic(GLuint program, float left, float right, float top, float bottom)
{
	float sumX = right + left;
	float invX = 1.0f / (right - left);
	float sumY = top + bottom;
	float invY = 1.0f / (top - bottom);
	float nearDepth = 0;
	float farDepth = 100;
	float sumZ = farDepth + nearDepth;
	float invZ = 1.0f / (farDepth - nearDepth);

	GLfloat matrix[4*4];
	matrix[0] = 2*invX; matrix[1] = 0; matrix[2] = 0; matrix[3] = 0;
	matrix[4] = 0; matrix[5] = 2 * invY; matrix[6] = 0; matrix[7] = 0;
	matrix[8] = 0; matrix[9] = 0; matrix[10] = 2 * invZ; matrix[11] = 0;
	matrix[12] = -sumX * invX; matrix[13] = -sumY * invY; matrix[14] = -sumZ * invZ; matrix[15] = 1;

	glUniformMatrix4fv( glGetUniformLocation( program, "sLocalToProjMatrix"), 1, GL_FALSE, matrix);
}

void whitgl_sys_draw_finish()
{
	glfwSwapBuffers();
	glDisable(GL_BLEND);
}

void whitgl_sys_draw_iaabb(whitgl_iaabb rect, whitgl_sys_color col)
{
	float vertices[6*4];
	_whitgl_populate_vertices(vertices, rect);
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW );

	glUseProgram( flatShaderProgram );
	glUniform4f( glGetUniformLocation( flatShaderProgram, "sColor" ), (float)col.r/255.0, (float)col.g/255.0, (float)col.b/255.0, (float)col.a/255.0 );
	_whitgl_sys_orthographic(flatShaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( flatShaderProgram, "position" );
	glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0 );
	glEnableVertexAttribArray( posAttrib );

	glDrawArrays( GL_TRIANGLES, 0, 6 );
}

void whitgl_sys_draw_tex_iaabb(int id, whitgl_iaabb rect)
{
	int index = -1;
	int i;
	for(i=0; i<num_images; i++)
	{
		if(images[i].id == id)
		{
			index = i;
			continue;
		}
	}
	if(index == -1)
	{
		WHITGL_LOG("ERR Cannot find image %d", id);
		return;
	}

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, images[index].gluint );

	float vertices[6*4];
	_whitgl_populate_vertices(vertices, rect);
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW );

	glUseProgram( shaderProgram );
	glUniform1i( glGetUniformLocation( shaderProgram, "tex" ), 0 );
	_whitgl_sys_orthographic(shaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0 );
	glEnableVertexAttribArray( posAttrib );

	GLint texturePosAttrib = glGetAttribLocation( shaderProgram, "texturepos" );
	glVertexAttribPointer( texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), BUFFER_OFFSET(sizeof(float)*2) );
	glEnableVertexAttribArray( texturePosAttrib );

	glDrawArrays( GL_TRIANGLES, 0, 6 );
}

void whitgl_sys_add_image(int id, const char* filename)
{
	if(num_images >= WHITGL_IMAGE_MAX)
	{
		WHITGL_LOG("ERR Too many images");
		return;
	}
	images[num_images].gluint = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,0);
	if( images[num_images].gluint == 0 )
	{
		WHITGL_LOG( "SOIL loading error: '%s'\n", SOIL_last_result() );
		return;
	}
	images[num_images].id = id;
	glBindTexture(GL_TEXTURE_2D, images[num_images].gluint);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int w, h;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
	images[num_images].size.x = w;
	images[num_images].size.y = h;

	num_images++;
}
