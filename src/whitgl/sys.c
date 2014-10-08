#include <stdbool.h>
#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <png.h>

#include <whitgl/logging.h>
#include <whitgl/sys.h>

bool _shouldClose;
int _pixel_scale;
whitgl_ivec _window_size;
GLFWwindow* _window;

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
	whitgl_sys_color colors[WHITGL_MAX_SHADER_COLORS];
	whitgl_shader shader;
} whitgl_shader_data;

GLuint vbo;
whitgl_shader_data shaders[WHITGL_SHADER_MAX];
GLuint frameBuffer;
GLuint intermediateTexture;

void _whitgl_check_gl_error(const char* stmt, const char *file, int line)
{
	GLenum err = glGetError();
	if(!err) return;
	const char* error = "UNKNOWN GL ERROR!";
	switch(err)
	{
		case GL_NO_ERROR: error = "GL_NO_ERROR"; break;
		case GL_INVALID_ENUM: error = "GL_INVALID_ENUM"; break;
		case GL_INVALID_VALUE: error = "GL_INVALID_VALUE"; break;
		case GL_INVALID_OPERATION: error = "GL_INVALID_OPERATION"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
		case GL_OUT_OF_MEMORY: error = "GL_OUT_OF_MEMORY"; break;
		case GL_STACK_UNDERFLOW: error = "GL_STACK_UNDERFLOW"; break;
		case GL_STACK_OVERFLOW: error = "GL_STACK_OVERFLOW"; break;
	}
	whitgl_logit(file, line, "%s in %s", error, stmt);
}

#define GL_CHECK(stmt) do { \
		stmt; \
		_whitgl_check_gl_error(#stmt, __FILE__, __LINE__); \
	} while (0)


void _whitgl_sys_close_callback(GLFWwindow*);

whitgl_sys_setup _setup;

void _whitgl_sys_glfw_error_callback(int code, const char* error)
{
	WHITGL_LOG("glfw error %d '%s'", code, error);
}

void whitgl_sys_set_clear_color(whitgl_sys_color col)
{
	float r = (float)col.r/255.0;
	float g = (float)col.g/255.0;
	float b = (float)col.b/255.0;
	float a = (float)col.a/255.0;
	glClearColor(r, g, b, a);
}

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
void whitgl_set_shader_color(whitgl_shader_slot type, int color, whitgl_sys_color value)
{
	if(type >= WHITGL_SHADER_MAX)
	{
		WHITGL_LOG("Invalid shader type %d", type);
		return;
	}
	if(color < 0 || color >= WHITGL_MAX_SHADER_COLORS || color >= shaders[type].shader.num_colors)
	{
		WHITGL_LOG("Invalid shader uniform %d", color);
		return;
	}
	shaders[type].colors[color] = value;
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

	if(setup->fullscreen)
	{
		const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		WHITGL_LOG("Opening fullscreen w%d h%d", mode->width, mode->height);
		_window = glfwCreateWindow(mode->width, mode->height, setup->name, glfwGetPrimaryMonitor(), NULL);
		bool searching = true;
		setup->pixel_size = mode->width/setup->size.x;
		whitgl_ivec new_size = setup->size;
		while(searching)
		{
			new_size.x = mode->width/setup->pixel_size;
			new_size.y = mode->height/setup->pixel_size;
			searching = false;
			if(new_size.x < setup->size.x) searching = true;
			if(new_size.y < setup->size.y) searching = true;
			if(setup->pixel_size == 1) searching = false;
			if(searching) setup->pixel_size--;
		}
	} else
	{
		WHITGL_LOG("Opening windowed w%d h%d", setup->size.x*setup->pixel_size, setup->size.y*setup->pixel_size);
		_window = glfwCreateWindow(setup->size.x*setup->pixel_size, setup->size.y*setup->pixel_size, setup->name, NULL, NULL);
	}
	_pixel_scale = setup->pixel_size;
	if(!_window)
	{
		WHITGL_LOG("Failed to open GLFW window");
		exit( EXIT_FAILURE );
	}
	glfwMakeContextCurrent(_window);

	WHITGL_LOG("Initiating glew");
	glewExperimental = GL_TRUE;
	glewInit();
	glGetError(); // Ignore any glGetError in glewInit, nothing to panic about, see https://www.opengl.org/wiki/OpenGL_Loading_Library

	WHITGL_LOG("Generating vbo");
	GL_CHECK( glGenBuffers( 1, &vbo ) ); // Generate 1 buffer

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

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
	GL_CHECK( glGenFramebuffers(1, &frameBuffer) );
	GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer) );
	GL_CHECK( glGenTextures(1, &intermediateTexture) );
	GL_CHECK( glBindTexture(GL_TEXTURE_2D, intermediateTexture) );
	WHITGL_LOG("Creating framebuffer glTexImage2D");
	GL_CHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, setup->size.x, setup->size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0) );
	GL_CHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
	GL_CHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
	GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );
	GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) );
	GL_CHECK( glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, intermediateTexture, 0) );
	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	GL_CHECK( glDrawBuffers(1, drawBuffers) ); // "1" is the size of drawBuffers
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		WHITGL_LOG("Problem setting up intermediate render target");
	GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, 0) );

	WHITGL_LOG("Enabling vsync");
	// Enable vertical sync (on cards that support it)
	GL_CHECK( glfwSwapInterval( 1 ) );

	WHITGL_LOG("Setting close callback");
	glfwSetWindowCloseCallback(_window, _whitgl_sys_close_callback);

	if(setup->disable_mouse_cursor)
	{
		WHITGL_LOG("Disable mouse cursor");
		glfwSetInputMode (_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

	int i;
	for(i=0; i<WHITGL_IMAGE_MAX; i++)
		images[i].id = -1;
	num_images = 0;

	_setup = *setup;

	WHITGL_LOG("Sys initiated");

	return true;
}

void _whitgl_sys_close_callback(GLFWwindow* window)
{
	(void)window;
	_shouldClose = true;
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

void whitgl_sys_draw_init()
{
	int w, h;
	glfwGetWindowSize( _window, &w, &h);
	_window_size.x = w;
	_window_size.y = h;

	GL_CHECK( glEnable(GL_BLEND) );
	GL_CHECK( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
	GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer) );
	GL_CHECK( glViewport( 0, 0, _setup.size.x, _setup.size.y ) );
	GL_CHECK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
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
	GL_CHECK( return );
}

void _whitgl_load_uniforms(whitgl_shader_slot slot)
{
	int i;
	for(i=0; i<shaders[slot].shader.num_uniforms; i++)
		glUniform1f( glGetUniformLocation( shaders[slot].program, shaders[slot].shader.uniforms[i]), shaders[slot].uniforms[i]);
	for(i=0; i<shaders[slot].shader.num_colors; i++)
	{
		whitgl_sys_color c = shaders[slot].colors[i];
		float r = ((float)c.r)/255.0;
		float g = ((float)c.g)/255.0;
		float b = ((float)c.b)/255.0;
		float a = ((float)c.a)/255.0;
		glUniform4f( glGetUniformLocation( shaders[slot].program, shaders[slot].shader.colors[i]), r, g, b, a);
	}
	GL_CHECK( return );
}

void whitgl_sys_draw_finish()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GL_CHECK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
	GL_CHECK( glViewport( 0, 0, _window_size.x, _window_size.y ) );
	GL_CHECK( glActiveTexture( GL_TEXTURE0 ) );
	GL_CHECK( glBindTexture( GL_TEXTURE_2D, intermediateTexture ) );

	float vertices[6*4];
	whitgl_iaabb src = whitgl_iaabb_zero;
	whitgl_iaabb dest = whitgl_iaabb_zero;
	src.b.x = _setup.size.x;
	src.a.y = _setup.size.y;
	dest.a.x = (_window_size.x-_setup.size.x*_setup.pixel_size)/2;
	dest.a.y = (_window_size.y-_setup.size.y*_setup.pixel_size)/2;
	dest.b.x = dest.a.x+_setup.size.x*_setup.pixel_size;
	dest.b.y = dest.a.y+_setup.size.y*_setup.pixel_size;

	_whitgl_populate_vertices(vertices, src, dest, _setup.size);
	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW ) );

	GLuint shaderProgram = shaders[WHITGL_SHADER_POST].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	GL_CHECK( glUniform1i( glGetUniformLocation( shaderProgram, "tex" ), 0 ) );
	_whitgl_load_uniforms(WHITGL_SHADER_POST);
	_whitgl_sys_orthographic(shaderProgram, 0, _window_size.x, 0, _window_size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );

	GLint texturePosAttrib = glGetAttribLocation( shaderProgram, "texturepos" );
	GL_CHECK( glVertexAttribPointer( texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), BUFFER_OFFSET(sizeof(float)*2) ) );
	GL_CHECK( glEnableVertexAttribArray( texturePosAttrib ) );

	GL_CHECK( glDrawArrays( GL_TRIANGLES, 0, 6 ) );

	glfwSwapBuffers(_window);
	glfwPollEvents();
	GL_CHECK( glDisable(GL_BLEND) );
}

void whitgl_sys_draw_iaabb(whitgl_iaabb rect, whitgl_sys_color col)
{
	float vertices[6*4];
	_whitgl_populate_vertices(vertices, whitgl_iaabb_zero, rect, whitgl_ivec_zero);

	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW ) );

	GLuint shaderProgram = shaders[WHITGL_SHADER_FLAT].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	GL_CHECK( glUniform4f( glGetUniformLocation( shaderProgram, "sColor" ), (float)col.r/255.0, (float)col.g/255.0, (float)col.b/255.0, (float)col.a/255.0 ) );
	_whitgl_load_uniforms(WHITGL_SHADER_FLAT);
	_whitgl_sys_orthographic(shaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );

	GL_CHECK( glDrawArrays( GL_TRIANGLES, 0, 6 ) );

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

	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, sizeof(float)*num_vertices , vertices, GL_DYNAMIC_DRAW ) );

	GLuint shaderProgram = shaders[WHITGL_SHADER_FLAT].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	GL_CHECK( glUniform4f( glGetUniformLocation( shaderProgram, "sColor" ), (float)col.r/255.0, (float)col.g/255.0, (float)col.b/255.0, (float)col.a/255.0 ) );
	_whitgl_load_uniforms(WHITGL_SHADER_FLAT);
	_whitgl_sys_orthographic(shaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );

	GL_CHECK( glDrawArrays( GL_TRIANGLES, 0, tris*3 ) );
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

	GL_CHECK( glActiveTexture( GL_TEXTURE0 ) );
	GL_CHECK( glBindTexture( GL_TEXTURE_2D, images[index].gluint ) );

	float vertices[6*4];
	_whitgl_populate_vertices(vertices, src, dest, images[index].size);
	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW ) );

	GLuint shaderProgram = shaders[WHITGL_SHADER_TEXTURE].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	GL_CHECK( glUniform1i( glGetUniformLocation( shaderProgram, "tex" ), 0 ) );
	_whitgl_load_uniforms(WHITGL_SHADER_TEXTURE);
	_whitgl_sys_orthographic(shaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );

	GLint texturePosAttrib = glGetAttribLocation( shaderProgram, "texturepos" );
	GL_CHECK( glVertexAttribPointer( texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), BUFFER_OFFSET(sizeof(float)*2) ) );
	GL_CHECK( glEnableVertexAttribArray( texturePosAttrib ) );

	GL_CHECK( glDrawArrays( GL_TRIANGLES, 0, 6 ) );
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

bool loadPngImage(const char *name, whitgl_int *outWidth, whitgl_int *outHeight, GLubyte **outData)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	int color_type, interlace_type;
	FILE *fp;

	if ((fp = fopen(name, "rb")) == NULL)
		return false;

	/* Create and initialize the png_struct
	 * with the desired error handler
	 * functions.  If you want to use the
	 * default stderr and longjump method,
	 * you can supply NULL for the last
	 * three parameters.  We also supply the
	 * the compiler header file version, so
	 * that we know if the application
	 * was compiled with a compatible version
	 * of the library.  REQUIRED
	 */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
									 NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return false;
	}

	/* Allocate/initialize the memory
	 * for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return false;
	}

	/* Set error handling if you are
	 * using the setjmp/longjmp method
	 * (this is the normal method of
	 * doing things with libpng).
	 * REQUIRED unless you  set up
	 * your own error handlers in
	 * the png_create_read_struct()
	 * earlier.
	 */
	if (setjmp(png_jmpbuf(png_ptr))) {
		/* Free all of the memory associated
		 * with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		/* If we get here, we had a
		 * problem reading the file */
		return false;
	}

	/* Set up the output control if
	 * you are using standard C streams */
	png_init_io(png_ptr, fp);

	/* If we have already
	 * read some of the signature */
	png_set_sig_bytes(png_ptr, sig_read);

	/*
	 * If you have enough memory to read
	 * in the entire image at once, and
	 * you need to specify only
	 * transforms that can be controlled
	 * with one of the PNG_TRANSFORM_*
	 * bits (this presently excludes
	 * dithering, filling, setting
	 * background, and doing gamma
	 * adjustment), then you can read the
	 * entire image (including pixels)
	 * into the info structure with this
	 * call
	 *
	 * PNG_TRANSFORM_STRIP_16 |
	 * PNG_TRANSFORM_PACKING  forces 8 bit
	 * PNG_TRANSFORM_EXPAND forces to
	 *  expand a palette into RGB
	 */
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	png_uint_32 width, height;
	int bit_depth;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
				 &interlace_type, NULL, NULL);
	*outWidth = width;
	*outHeight = height;

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	*outData = (unsigned char*) malloc(row_bytes *  (*outHeight));

	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	int i;
	for (i = 0; i < *outHeight; i++)
	{
		memcpy(*outData+(row_bytes * (i)), row_pointers[i], row_bytes);
	}

	/* Clean up after the read,
	 * and free any memory allocated */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);


	/* Close the file */
	fclose(fp);

	/* That's it */
	return true;
}

void whitgl_sys_add_image(int id, const char* filename)
{
	WHITGL_LOG("Adding image: %s", filename);
	if(num_images >= WHITGL_IMAGE_MAX)
	{
		WHITGL_LOG("ERR Too many images");
		return;
	}

	GLubyte *textureImage;
	bool success = loadPngImage(filename, &images[num_images].size.x, &images[num_images].size.y, &textureImage);
	if(!success)
	{
		WHITGL_LOG("loadPngImage error");
	}

	GL_CHECK( glPixelStorei(GL_UNPACK_ALIGNMENT, 1) );
	GL_CHECK( glPixelStorei(GL_PACK_LSB_FIRST, 1) );
	GL_CHECK( glGenTextures(1, &images[num_images].gluint ) );
	GL_CHECK( glActiveTexture( GL_TEXTURE0 ) );
	GL_CHECK( glBindTexture(GL_TEXTURE_2D, images[num_images].gluint) );
	GL_CHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
	GL_CHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
	GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );
	GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) );
	GL_CHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, images[num_images].size.x,
				 images[num_images].size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				 textureImage) );
	free(textureImage);

	images[num_images].id = id;
	num_images++;
}

double whitgl_sys_get_time()
{
	return glfwGetTime();
}
