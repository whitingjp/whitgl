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

const char* _vertex_src = "\
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

const char* _fragment_src = "\
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

const char* _flat_src = "\
#version 150\
\n\
uniform vec4 sColor;\
out vec4 outColor;\
void main()\
{\
	outColor = sColor;\
}\
";

typedef struct
{
	GLuint program;
	float uniforms[WHITGL_MAX_SHADER_UNIFORMS];
	whitgl_shader shader;
} whitgl_shader_data;


GLuint vbo;
whitgl_shader_data shaders[WHITGL_SHADER_MAX];
GLuint frameBuffer;
GLuint intermediateTexture;

int GLFWCALL _whitgl_sys_close_callback();

whitgl_sys_setup _setup;

// void _whitgl_sys_glfw_error_callback(int code, const char* error)
// {
// 	WHITGL_LOG("glfw error %d '%s'", code, error);
// }

bool whitgl_change_shader(whitgl_shader_slot type, whitgl_shader shader)
{
	if(type >= WHITGL_SHADER_MAX)
	{
		WHITGL_LOG("Invalid shader type %d", type);
		return false;
	}
	shaders[type].shader = shader;

	if(shader.vertex_src == NULL)
		shader.vertex_src = _vertex_src;
	if(shader.fragment_src == NULL)
		shader.fragment_src = _fragment_src;

	if(glIsProgram(shaders[type].program))
		glDeleteProgram(shaders[type].program);

	GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertexShader, 1, &shader.vertex_src, NULL );
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
	glShaderSource( fragmentShader, 1, &shader.fragment_src, NULL );
	glCompileShader( fragmentShader );

	glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &status );
	if(status != GL_TRUE)
	{
		char buffer[512];
		glGetShaderInfoLog( fragmentShader, 512, NULL, buffer );
		WHITGL_LOG(buffer);
		return false;
	}

	shaders[type].program = glCreateProgram();
	glAttachShader( shaders[type].program, vertexShader );
	glAttachShader( shaders[type].program, fragmentShader );
	glBindFragDataLocation( shaders[type].program, 0, "outColor" );
	glLinkProgram( shaders[type].program );

	return true;
}

void whitgl_set_shader_uniform(whitgl_shader_slot type, int uniform, float value)
{
	if(type >= WHITGL_SHADER_MAX)
	{
		WHITGL_LOG("Invalid shader type %d", type);
		return;
	}
	if(uniform < 0 || uniform >= WHITGL_MAX_SHADER_UNIFORMS || uniform >= shaders[type].shader.num_uniforms)
	{
		WHITGL_LOG("Invalid shader uniform %d", uniform);
		return;
	}
	shaders[type].uniforms[uniform] = value;
}

bool whitgl_sys_init(whitgl_sys_setup* setup)
{
	bool result;
	_shouldClose = false;

	WHITGL_LOG("Initialize GLFW");

	// glfwSetErrorCallback(&_whitgl_sys_glfw_error_callback);
	result = glfwInit();
	if(!result)
	{
		WHITGL_LOG("Failed to initialize GLFW");
		exit( EXIT_FAILURE );
	}

	WHITGL_LOG("Setting GLFW window hints");
	glfwOpenWindowHint( GLFW_OPENGL_VERSION_MAJOR, 3 );
	glfwOpenWindowHint( GLFW_OPENGL_VERSION_MINOR, 2 );
	glfwOpenWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE );
	glfwOpenWindowHint( GLFW_WINDOW_NO_RESIZE, GL_TRUE );

	GLFWvidmode desktop;
	glfwGetDesktopMode( &desktop );

	if(setup->fullscreen)
	{
		WHITGL_LOG("Opening fullscreen w%d h%d", desktop.Width, desktop.Height);
		result = glfwOpenWindow( desktop.Width, desktop.Height, desktop.RedBits,
		                         desktop.GreenBits, desktop.BlueBits, 8, 32, 0,
		                         GLFW_FULLSCREEN );
		bool searching = true;
		setup->pixel_size = desktop.Width/setup->size.x;
		while(searching)
		{
			setup->size.x = desktop.Width/setup->pixel_size;
			setup->size.y = desktop.Height/setup->pixel_size;
			searching = false;
			if(setup->size.x < 320) searching = true;
			if(setup->size.y < 240) searching = true;
			if(setup->pixel_size == 1) searching = false;
			if(searching) setup->pixel_size--;
		}
	} else
	{
		WHITGL_LOG("Opening windowed w%d h%d", setup->size.x*setup->pixel_size, setup->size.y*setup->pixel_size);
		result = glfwOpenWindow( setup->size.x*setup->pixel_size, setup->size.y*setup->pixel_size,
		                         0,0,0,0, 0,0, GLFW_WINDOW );
	}
	_pixel_scale = setup->pixel_size;
	WHITGL_LOG("Setting title");
	glfwSetWindowTitle(setup->name);
	if(!result)
	{
		WHITGL_LOG("Failed to open GLFW window");
		exit( EXIT_FAILURE );
	}

	WHITGL_LOG("Initiating glew");
	glewExperimental = GL_TRUE;
	glewInit();

	WHITGL_LOG("Generating vbo");
	glGenBuffers( 1, &vbo ); // Generate 1 buffer

	WHITGL_LOG("Loading shaders");
	whitgl_shader flat_shader = whitgl_shader_zero;
	flat_shader.fragment_src = _flat_src;
	if(!whitgl_change_shader( WHITGL_SHADER_FLAT, flat_shader))
		return false;
	whitgl_shader texture_shader = whitgl_shader_zero;
	if(!whitgl_change_shader( WHITGL_SHADER_TEXTURE, texture_shader))
		return false;
	if(!whitgl_change_shader( WHITGL_SHADER_POST, texture_shader))
		return false;

	WHITGL_LOG("Creating framebuffer");
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glGenTextures(1, &intermediateTexture);
	glBindTexture(GL_TEXTURE_2D, intermediateTexture);
	WHITGL_LOG("Creating framebuffer glTexImage2D");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, setup->size.x, setup->size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, intermediateTexture, 0);
	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, drawBuffers); // "1" is the size of drawBuffers
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		WHITGL_LOG("Problem setting up intermediate render target");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	WHITGL_LOG("Enabling vsync");
	// Enable vertical sync (on cards that support it)
	glfwSwapInterval( 1 );

	WHITGL_LOG("Setting close callback");
	glfwSetWindowCloseCallback(_whitgl_sys_close_callback);

	if(setup->disable_mouse_cursor)
	{
		WHITGL_LOG("Disable mouse cursor");
		glfwDisable(GLFW_MOUSE_CURSOR);
	}

	int i;
	for(i=0; i<WHITGL_IMAGE_MAX; i++)
		images[i].id = -1;
	num_images = 0;

	_setup = *setup;

	WHITGL_LOG("Sys initiated");

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, _setup.size.x, _setup.size.y, 0);
	glViewport( 0, 0, _setup.size.x, _setup.size.y );

	glClearColor(0, 0.0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void _whitgl_populate_vertices(float* vertices, whitgl_iaabb s, whitgl_iaabb d, whitgl_ivec image_size)
{
	whitgl_faabb sf = whitgl_faabb_divide(whitgl_iaabb_to_faabb(s), whitgl_ivec_to_fvec(image_size));
	vertices[ 0] = d.a.x; vertices[ 1] = d.b.y; vertices[ 2] = sf.a.x; vertices[ 3] = sf.b.y;
	vertices[ 4] = d.b.x; vertices[ 5] = d.a.y; vertices[ 6] = sf.b.x; vertices[ 7] = sf.a.y;
	vertices[ 8] = d.a.x; vertices[ 9] = d.a.y; vertices[10] = sf.a.x; vertices[11] = sf.a.y;

	vertices[12] = d.a.x; vertices[13] = d.b.y; vertices[14] = sf.a.x; vertices[15] = sf.b.y;
	vertices[16] = d.b.x; vertices[17] = d.b.y; vertices[18] = sf.b.x; vertices[19] = sf.b.y;
	vertices[20] = d.b.x; vertices[21] = d.a.y; vertices[22] = sf.b.x; vertices[23] = sf.a.y;
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

void _whitgl_load_uniforms(whitgl_shader_slot slot)
{
	int i;
	for(i=0; i<shaders[slot].shader.num_uniforms; i++)
		glUniform1f( glGetUniformLocation( shaders[slot].program, shaders[slot].shader.uniforms[i]), shaders[slot].uniforms[i]);
}

void whitgl_sys_draw_finish()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, _window_size.x, _window_size.y, 0);
	glViewport( 0, 0, _window_size.x, _window_size.y );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, intermediateTexture );

	float vertices[6*4];
	whitgl_iaabb src = whitgl_iaabb_zero;
	whitgl_iaabb dest = whitgl_iaabb_zero;
	src.b.x = _setup.size.x;
	src.a.y = _setup.size.y;
	dest.b = _window_size;

	_whitgl_populate_vertices(vertices, src, dest, _setup.size);
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW );

	GLuint shaderProgram = shaders[WHITGL_SHADER_POST].program;
	glUseProgram( shaderProgram );
	glUniform1i( glGetUniformLocation( shaderProgram, "tex" ), 0 );
	_whitgl_load_uniforms(WHITGL_SHADER_POST);
	_whitgl_sys_orthographic(shaderProgram, 0, _window_size.x, 0, _window_size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0 );
	glEnableVertexAttribArray( posAttrib );

	GLint texturePosAttrib = glGetAttribLocation( shaderProgram, "texturepos" );
	glVertexAttribPointer( texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), BUFFER_OFFSET(sizeof(float)*2) );
	glEnableVertexAttribArray( texturePosAttrib );

	glDrawArrays( GL_TRIANGLES, 0, 6 );

	glfwSwapBuffers();
	glDisable(GL_BLEND);
}

void whitgl_sys_draw_iaabb(whitgl_iaabb rect, whitgl_sys_color col)
{
	float vertices[6*4];
	_whitgl_populate_vertices(vertices, whitgl_iaabb_zero, rect, whitgl_ivec_zero);
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW );

	GLuint shaderProgram = shaders[WHITGL_SHADER_FLAT].program;
	glUseProgram( shaderProgram );
	glUniform4f( glGetUniformLocation( shaderProgram, "sColor" ), (float)col.r/255.0, (float)col.g/255.0, (float)col.b/255.0, (float)col.a/255.0 );
	_whitgl_load_uniforms(WHITGL_SHADER_FLAT);
	_whitgl_sys_orthographic(shaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0 );
	glEnableVertexAttribArray( posAttrib );

	glDrawArrays( GL_TRIANGLES, 0, 6 );
}

void whitgl_sys_draw_fcircle(whitgl_fcircle c, whitgl_sys_color col, int tris)
{
	int num_vertices = tris*3*2;
	whitgl_fvec scale = {c.size, c.size};
	float *vertices = malloc(sizeof(float)*num_vertices);
	int i;
	for(i=0; i<tris; i++)
	{
		whitgl_float dir;
		whitgl_fvec off;
		int vertex_offset = 6*i;
		vertices[vertex_offset+0] = c.pos.x; vertices[vertex_offset+1] = c.pos.y;

		dir = ((whitgl_float)i)/tris * whitgl_pi * 2 ;
		off = whitgl_fvec_scale(whitgl_angle_to_fvec(dir), scale);
		vertices[vertex_offset+2] = c.pos.x+off.x; vertices[vertex_offset+3] = c.pos.y+off.y;

		dir = ((whitgl_float)(i+1))/tris * whitgl_pi * 2;
		off = whitgl_fvec_scale(whitgl_angle_to_fvec(dir), scale);
		vertices[vertex_offset+4] = c.pos.x+off.x; vertices[vertex_offset+5] = c.pos.y+off.y;
	}

	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(float)*num_vertices , vertices, GL_DYNAMIC_DRAW );

	GLuint shaderProgram = shaders[WHITGL_SHADER_FLAT].program;
	glUseProgram( shaderProgram );
	glUniform4f( glGetUniformLocation( shaderProgram, "sColor" ), (float)col.r/255.0, (float)col.g/255.0, (float)col.b/255.0, (float)col.a/255.0 );
	_whitgl_load_uniforms(WHITGL_SHADER_FLAT);
	_whitgl_sys_orthographic(shaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0 );
	glEnableVertexAttribArray( posAttrib );

	glDrawArrays( GL_TRIANGLES, 0, tris*3 );
	free(vertices);
}

void whitgl_sys_draw_tex_iaabb(int id, whitgl_iaabb src, whitgl_iaabb dest)
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
	_whitgl_populate_vertices(vertices, src, dest, images[index].size);
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW );

	GLuint shaderProgram = shaders[WHITGL_SHADER_TEXTURE].program;
	glUseProgram( shaderProgram );
	glUniform1i( glGetUniformLocation( shaderProgram, "tex" ), 0 );
	_whitgl_load_uniforms(WHITGL_SHADER_TEXTURE);
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
void whitgl_sys_draw_sprite(whitgl_sprite sprite, whitgl_ivec frame, whitgl_ivec pos)
{
	whitgl_iaabb src = whitgl_iaabb_zero;
	whitgl_ivec offset = whitgl_ivec_scale(sprite.size, frame);
	src.a = whitgl_ivec_add(sprite.top_left, offset);
	src.b = whitgl_ivec_add(src.a, sprite.size);
	whitgl_iaabb dest = whitgl_iaabb_zero;
	dest.a = pos;
	dest.b = whitgl_ivec_add(dest.a, sprite.size);
	whitgl_sys_draw_tex_iaabb(sprite.image, src, dest);
}

void whitgl_sys_add_image(int id, const char* filename)
{
	WHITGL_LOG("Adding image: %s", filename);
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
void whitgl_sys_image_from_data(int id, whitgl_ivec size, const unsigned char* data)
{
	if(num_images >= WHITGL_IMAGE_MAX)
	{
		WHITGL_LOG("ERR Too many images");
		return;
	}
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
		index = num_images;
		images[index].gluint = SOIL_create_OGL_texture(data, size.x, size.y, 3, SOIL_CREATE_NEW_ID, SOIL_LOAD_L);
	}
	else
	{
		SOIL_create_OGL_texture(data, size.x, size.y, 3, images[index].gluint, SOIL_LOAD_L);
	}
	if( images[index].gluint == 0 )
	{
		WHITGL_LOG( "SOIL loading error: '%s'\n", SOIL_last_result() );
		return;
	}
	images[index].id = id;
	glBindTexture(GL_TEXTURE_2D, images[index].gluint);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	int w, h;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
	images[index].size.x = w;
	images[index].size.y = h;


	if(index == num_images)
		num_images++;
}

double whitgl_sys_get_time()
{
	return glfwGetTime();
}