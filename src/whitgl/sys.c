#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <png.h>

#include <whitgl/logging.h>
#include <whitgl/profile.h>
#include <whitgl/sys.h>

void _whitgl_sys_flush_tex_iaabb();

whitgl_bool _shouldClose;
whitgl_ivec _window_size;
whitgl_ivec _buffer_size;
GLFWwindow* _window;
whitgl_bool _windowFocused;

typedef struct
{
	whitgl_int id;
	GLuint gluint;
	whitgl_ivec size;
} whitgl_image;
#define WHITGL_IMAGE_MAX (64)
whitgl_image images[WHITGL_IMAGE_MAX];
whitgl_int num_images;

typedef struct
{
	whitgl_int id;
	GLuint vbo;
	whitgl_int num_vertices;
	whitgl_int max_vertices;
} whitgl_model;
static const whitgl_model whitgl_model_zero = {-1, 0, -1, -1};
#define WHITGL_MODEL_MAX (32)
whitgl_model models[WHITGL_MODEL_MAX];
whitgl_int num_models;


typedef struct
{
	GLuint buffer;
	GLuint texture;
	GLuint depth;
	whitgl_ivec size;
} whitgl_framebuffer;
#define WHITGL_FRAMEBUFFER_MAX (8)
whitgl_framebuffer framebuffers[WHITGL_MODEL_MAX];
whitgl_int num_framebuffers;

const char* _vertex_src = "\
#version 150\
\n\
\
in vec3 position;\
in vec2 texturepos;\
in vec3 vertexColor;\
in vec3 vertexNormal;\
out vec2 Texturepos;\
out vec3 fragmentColor;\
out vec3 fragmentNormal;\
out vec3 fragmentPosition;\
out mat4 normalMatrix;\
uniform mat4 m_model;\
uniform mat4 m_view;\
uniform mat4 m_perspective;\
void main()\
{\
	gl_Position = m_perspective * m_view * m_model * vec4( position, 1.0 );\
	Texturepos = texturepos;\
	fragmentColor = vertexColor;\
	fragmentNormal = vertexNormal;\
	fragmentPosition = vec3( m_model * vec4( position, 1.0));\
	normalMatrix = transpose(inverse(m_model));\
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
uniform vec4 color;\
out vec4 outColor;\
void main()\
{\
	outColor = color;\
}\
";

const char* _model_src = "\
#version 150\
\n\
in vec3 fragmentColor;\
in vec3 fragmentNormal;\
out vec4 outColor;\
void main()\
{\
	vec3 col = vec3(fragmentNormal)/2+0.5;\
	outColor = vec4((col+fragmentColor)*0.5,1);\
}\
";

typedef union
{
	whitgl_float number;
	whitgl_fvec fvec;
	whitgl_fvec3 fvec3;
	whitgl_sys_color color;
	whitgl_int image;
	whitgl_int framebuffer;
	whitgl_fmat matrix;
} whitgl_uniform_data;

typedef struct
{
	GLuint program;
	whitgl_uniform_data uniforms[WHITGL_MAX_SHADER_UNIFORMS];
	whitgl_shader shader;
} whitgl_shader_data;

typedef struct
{
	bool to_file;
	bool do_next;
	bool pre_postprocess;
	char file[512];
	whitgl_sys_color* data;
	whitgl_int frame_buffer;
} whitgl_frame_capture;
static const whitgl_frame_capture whitgl_frame_capture_zero = {true, false, false, {'\0'}, NULL, 0};

GLuint vbo;
whitgl_shader_data shaders[WHITGL_SHADER_MAX];
whitgl_frame_capture capture;
whitgl_bool started_drawing = false;

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


void _whitgl_sys_handle_signal(int signal);
void _whitgl_sys_close_callback(GLFWwindow*);
void _whitgl_sys_glfw_error_callback(int code, const char* error);
void _whitgl_sys_window_focus_callback(GLFWwindow *window, int focused);
void _whitgl_sys_drop_callback(GLFWwindow* window, int count, const char** paths);
void _whitgl_sys_window_size_callback(GLFWwindow* window, int width, int height);

whitgl_sys_setup _setup;
whitgl_sys_setup* _setup_pointer;
whitgl_ivec _initial_setup_size;

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
		WHITGL_PANIC("Invalid shader type %d", type);
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
		WHITGL_PANIC(buffer);
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
		WHITGL_PANIC(buffer);
		return false;
	}

	shaders[type].program = glCreateProgram();
	glAttachShader( shaders[type].program, vertexShader );
	glAttachShader( shaders[type].program, fragmentShader );
	glBindFragDataLocation( shaders[type].program, 0, "outColor" );
	glLinkProgram( shaders[type].program );

	return true;
}

void _whitgl_check_uniform_validity(whitgl_shader_slot slot, whitgl_int uniform, whitgl_uniform_type type)
{
	if(slot >= WHITGL_SHADER_MAX)
		WHITGL_PANIC("Invalid shader slot %d", slot);
	if(uniform < 0 || uniform >= WHITGL_MAX_SHADER_UNIFORMS || uniform >= shaders[slot].shader.num_uniforms)
		WHITGL_PANIC("Invalid shader uniform %d", uniform);
	if(shaders[slot].shader.uniforms[uniform].type != type)
		WHITGL_PANIC("Invalid uniform type %d expecting %d", shaders[slot].shader.uniforms[uniform].type, type);
}

void whitgl_set_shader_float(whitgl_shader_slot type, whitgl_int uniform, float value)
{
	_whitgl_check_uniform_validity(type, uniform, WHITGL_UNIFORM_FLOAT);
	if(shaders[type].uniforms[uniform].number != value)
		_whitgl_sys_flush_tex_iaabb();
	shaders[type].uniforms[uniform].number = value;
}
void whitgl_set_shader_fvec(whitgl_shader_slot type, whitgl_int uniform, whitgl_fvec value)
{
	_whitgl_check_uniform_validity(type, uniform, WHITGL_UNIFORM_FVEC);
	whitgl_fvec existing = shaders[type].uniforms[uniform].fvec;
	if(existing.x != value.x || existing.y != value.y)
		_whitgl_sys_flush_tex_iaabb();
	shaders[type].uniforms[uniform].fvec = value;
}
void whitgl_set_shader_fvec3(whitgl_shader_slot type, whitgl_int uniform, whitgl_fvec3 value)
{
	_whitgl_check_uniform_validity(type, uniform, WHITGL_UNIFORM_FVEC3);
	whitgl_fvec3 existing = shaders[type].uniforms[uniform].fvec3;
	if(existing.x != value.x || existing.y != value.y || existing.z != value.z)
		_whitgl_sys_flush_tex_iaabb();
	shaders[type].uniforms[uniform].fvec3 = value;
}
void whitgl_set_shader_color(whitgl_shader_slot type, whitgl_int uniform, whitgl_sys_color value)
{
	_whitgl_check_uniform_validity(type, uniform, WHITGL_UNIFORM_COLOR);
	whitgl_sys_color existing = shaders[type].uniforms[uniform].color;
	if(existing.r != value.r ||
	   existing.g != value.g ||
	   existing.b != value.b ||
	   existing.a != value.a)
		_whitgl_sys_flush_tex_iaabb();
	shaders[type].uniforms[uniform].color = value;
}
void whitgl_set_shader_image(whitgl_shader_slot type, whitgl_int uniform, whitgl_int index)
{
	_whitgl_check_uniform_validity(type, uniform, WHITGL_UNIFORM_IMAGE);
	if(shaders[type].uniforms[uniform].image != index)
		_whitgl_sys_flush_tex_iaabb();
	shaders[type].uniforms[uniform].image = index;
}
void whitgl_set_shader_framebuffer(whitgl_shader_slot type, whitgl_int uniform, whitgl_int index)
{
	_whitgl_check_uniform_validity(type, uniform, WHITGL_UNIFORM_FRAMEBUFFER);
	if(shaders[type].uniforms[uniform].framebuffer != index)
		_whitgl_sys_flush_tex_iaabb();
	shaders[type].uniforms[uniform].framebuffer = index;
}
void whitgl_set_shader_matrix(whitgl_shader_slot type, whitgl_int uniform, whitgl_fmat fmat)
{
	_whitgl_check_uniform_validity(type, uniform, WHITGL_UNIFORM_MATRIX);
	if(!whitgl_fmat_eq(shaders[type].uniforms[uniform].matrix, fmat))
		_whitgl_sys_flush_tex_iaabb();
	shaders[type].uniforms[uniform].matrix = fmat;
};

void whitgl_resize_framebuffer(whitgl_int i, whitgl_ivec size, whitgl_bool one_color)
{
	if(i >= _setup.num_framebuffers)
		WHITGL_LOG("Invalid framebuffer number");

	WHITGL_LOG("Deleting framebuffer %d");
	GL_CHECK( glDeleteRenderbuffers(1, &framebuffers[i].depth ) );
	GL_CHECK( glDeleteTextures(1, &framebuffers[i].texture ) );
	GL_CHECK( glDeleteFramebuffers(1, &framebuffers[i].buffer ) );


	WHITGL_LOG("Creating framebuffer %d", i);
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GL_CHECK( glGenFramebuffers(1, &framebuffers[i].buffer) );
	GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i].buffer) );
	GL_CHECK( glGenTextures(1, &framebuffers[i].texture) );
	GL_CHECK( glBindTexture(GL_TEXTURE_2D, framebuffers[i].texture) );
	if(one_color)
		GL_CHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, 0) );
	else
		GL_CHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0) );
	GL_CHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
	GL_CHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
	GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );
	GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) );
	// The depth buffer
	if(!one_color)
	{
		GL_CHECK( glGenRenderbuffers(1, &framebuffers[i].depth) );
		GL_CHECK( glBindRenderbuffer(GL_RENDERBUFFER, framebuffers[i].depth) );
		GL_CHECK( glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y) );
		GL_CHECK( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffers[i].depth) );
	}
	GL_CHECK( glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, framebuffers[i].texture, 0) );
	GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	GL_CHECK( glDrawBuffers(1, drawBuffers) ); // "1" is the size of drawBuffers
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		WHITGL_LOG("Problem setting up intermediate render target");
	GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
	framebuffers[i].size = size;
}

void _whitgl_calculate_setup_size(whitgl_sys_setup* setup, whitgl_ivec screen_size)
{
	if(setup->resolution_mode == RESOLUTION_USE_WINDOW)
		setup->size = screen_size;

	// RESOLUTION_AT_LEAST is essentially, find the lowest allowable pixel size that fits setup->size within the screen
	if(setup->resolution_mode == RESOLUTION_AT_LEAST)
	{
		setup->pixel_size = screen_size.x/_initial_setup_size.x;
		whitgl_ivec new_size;
		bool searching = true;
		while(searching)
		{
			new_size = whitgl_ivec_divide(screen_size, whitgl_ivec_val(setup->pixel_size));
			searching = false;
			if(new_size.x < _initial_setup_size.x) searching = true;
			if(new_size.y < _initial_setup_size.y) searching = true;
			if(setup->pixel_size == 1) searching = false;
			if(searching)
				setup->pixel_size--;
		}
		setup->size = new_size;
	}
	// RESOLUTION_AT_MOST is essentially, find the highest allowable pixel size that doesn't let the screen exceed setup->size
	if(setup->resolution_mode == RESOLUTION_AT_MOST)
	{
		setup->pixel_size = 1;
		whitgl_ivec new_size = _initial_setup_size;
		bool searching = true;
		while(searching)
		{
			new_size = whitgl_ivec_divide(screen_size, whitgl_ivec_val(setup->pixel_size));
			searching = false;
			if(new_size.x > _initial_setup_size.x) searching = true;
			if(new_size.y > _initial_setup_size.y) searching = true;
			if(searching)
				setup->pixel_size++;
		}
		setup->size = new_size;
	}
}

bool whitgl_sys_init(whitgl_sys_setup* setup)
{
	bool result;
	_shouldClose = false;

	WHITGL_LOG("Initialize GLFW");

	glfwSetErrorCallback(&_whitgl_sys_glfw_error_callback);

#if defined(__APPLE__)
	if(setup->start_hidden)
		glfwInitHint(GLFW_COCOA_MENUBAR, false);
#endif

	WHITGL_LOG("glfwInit");
	result = glfwInit();
	if(!result)
	{
		WHITGL_PANIC("Failed to initialize GLFW");
		exit( EXIT_FAILURE );
	}

	WHITGL_LOG("Setting GLFW window hints");
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	_windowFocused = setup->start_focused || setup->fullscreen;
	if(!_windowFocused)
		glfwWindowHint(GLFW_FOCUSED, GL_FALSE);
	if(!setup->resizable)
		glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

	if(setup->start_hidden)
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	// attempt to ensure that no mode changing takes place
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	if(setup->fullscreen)
	{
		WHITGL_LOG("Opening fullscreen w%d h%d", mode->width, mode->height);
		_window = glfwCreateWindow(mode->width, mode->height, setup->name, glfwGetPrimaryMonitor(), NULL);
	} else
	{
		whitgl_ivec window_size;
		window_size.x = setup->size.x*setup->pixel_size;
		window_size.y = setup->size.y*setup->pixel_size;
		WHITGL_LOG("Opening windowed w%d h%d", window_size.x, window_size.y);
		_window = glfwCreateWindow(window_size.x, window_size.y, setup->name, NULL, NULL);
	}

	if(!_window)
		WHITGL_LOG("Failed to create window.");

	 glfwSetWindowFocusCallback (_window, _whitgl_sys_window_focus_callback);

	glfwSetWindowSizeCallback(_window, _whitgl_sys_window_size_callback);

	// Retrieve actual size of window in pixels, don't make an assumption
	// (because of retina screens etc.
	int w, h;
	glfwGetFramebufferSize(_window, &w, &h);
	whitgl_ivec screen_size = {w, h};

	_initial_setup_size = setup->size;
	_whitgl_calculate_setup_size(setup, screen_size);

	WHITGL_LOG("Pixel scale %d render area %d %d", setup->pixel_size, setup->size.x, setup->size.y);
	if(!_window)
	{
		WHITGL_PANIC("Failed to open GLFW window");
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
	flat_shader.num_uniforms = 1;
	flat_shader.uniforms[0].type = WHITGL_UNIFORM_COLOR;
	flat_shader.uniforms[0].name = "color";
	if(!whitgl_change_shader( WHITGL_SHADER_FLAT, flat_shader))
		return false;
	whitgl_shader texture_shader = whitgl_shader_zero;
	if(!whitgl_change_shader( WHITGL_SHADER_TEXTURE, texture_shader))
		return false;
	if(!whitgl_change_shader( WHITGL_SHADER_POST, texture_shader))
		return false;
	whitgl_shader model_shader = whitgl_shader_zero;
	model_shader.fragment_src = _model_src;
	if(!whitgl_change_shader( WHITGL_SHADER_MODEL, model_shader))
		return false;

	WHITGL_LOG("Creating framebuffers");
	whitgl_int i;
	if(setup->num_framebuffers > WHITGL_FRAMEBUFFER_MAX)
		WHITGL_PANIC("More than maximum framebuffers");
	num_framebuffers = setup->num_framebuffers;
	for(i=0; i<num_framebuffers; i++)
	{
		WHITGL_LOG("Creating framebuffer %d", i);
		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
		GL_CHECK( glGenFramebuffers(1, &framebuffers[i].buffer) );
		GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i].buffer) );
		GL_CHECK( glGenTextures(1, &framebuffers[i].texture) );
		GL_CHECK( glBindTexture(GL_TEXTURE_2D, framebuffers[i].texture) );
		GL_CHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, setup->size.x, setup->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0) );
		GL_CHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
		GL_CHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
		GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );
		GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) );
		// The depth buffer
		GL_CHECK( glGenRenderbuffers(1, &framebuffers[i].depth) );
		GL_CHECK( glBindRenderbuffer(GL_RENDERBUFFER, framebuffers[i].depth) );
		GL_CHECK( glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, setup->size.x, setup->size.y) );
		GL_CHECK( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffers[i].depth) );
		GL_CHECK( glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, framebuffers[i].texture, 0) );
		GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		GL_CHECK( glDrawBuffers(1, drawBuffers) ); // "1" is the size of drawBuffers
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			WHITGL_LOG("Problem setting up intermediate render target");
		GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
		framebuffers[i].size = setup->size;
	}

	if(setup->vsync)
	{
		WHITGL_LOG("Enabling vsync");
		GL_CHECK( glfwSwapInterval(1) ); // some cards still wont vsync!
	} else
	{
		WHITGL_LOG("Disabling vsync");
		GL_CHECK( glfwSwapInterval(0) );
	}

	WHITGL_LOG("Setting close callback");
	glfwSetWindowCloseCallback(_window, _whitgl_sys_close_callback);

	WHITGL_LOG("Setting drag-and-drop callback");
	glfwSetDropCallback(_window, _whitgl_sys_drop_callback);

	// Set mouse cursor mode
	WHITGL_LOG("Disable mouse cursor");
	whitgl_int glfw_mode;
	switch(setup->cursor)
	{
		case CURSOR_HIDE: glfw_mode = GLFW_CURSOR_HIDDEN; break;
		case CURSOR_DISABLE: glfw_mode = GLFW_CURSOR_DISABLED; break;
		case CURSOR_SHOW:
		default:
			glfw_mode = GLFW_CURSOR_NORMAL; break;
	}
	glfwSetInputMode (_window, GLFW_CURSOR, glfw_mode);

	for(i=0; i<WHITGL_IMAGE_MAX; i++)
		images[i].id = -1;
	num_images = 0;
	for(i=0; i<WHITGL_MODEL_MAX; i++)
		models[i] = whitgl_model_zero;
	num_models = 0;

	_setup = *setup;
	_setup_pointer = setup;

	capture = whitgl_frame_capture_zero;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);

	signal(SIGTERM, _whitgl_sys_handle_signal);

	whitgl_profile_init();

	WHITGL_LOG("Sys initiated");

	return true;
}

void _whitgl_sys_handle_signal(int signal)
{
	WHITGL_LOG("recieved signal %d", signal);
	_shouldClose = true;
}
void _whitgl_sys_close_callback(GLFWwindow* window)
{
	(void)window;
	_shouldClose = true;
}

void _whitgl_sys_glfw_error_callback(int code, const char* error)
{
	WHITGL_PANIC("glfw error %d '%s'", code, error);
}

void _whitgl_sys_window_focus_callback(GLFWwindow *window, int focused)
{
	(void)window;
	_windowFocused = focused;
}

void _whitgl_sys_window_size_callback(GLFWwindow* window, int width, int height)
{
	(void)window;
	whitgl_ivec new_size = {width, height};

	_whitgl_calculate_setup_size(&_setup, new_size);
	_setup_pointer->size = _setup.size;
	_setup_pointer->pixel_size = _setup.pixel_size;
	whitgl_resize_framebuffer(0, _setup.size, false);
}

bool whitgl_sys_should_close()
{
	return _shouldClose;
}

void whitgl_sys_close()
{
	whitgl_profile_shutdown();
	glfwTerminate();
}

double whitgl_sys_getTime()
{
	return glfwGetTime();
}

void whitgl_sys_draw_init(whitgl_int framebuffer_id)
{
	if(framebuffer_id >= num_framebuffers)
		WHITGL_PANIC("invalid framebuffer, have you assigned enough");
	if(started_drawing)
	{
		_whitgl_sys_flush_tex_iaabb();
	}
	int w, h;
	glfwGetFramebufferSize(_window, &w, &h);
	_window_size.x = w;
	_window_size.y = h;
	GL_CHECK( glEnable(GL_BLEND) );
	GL_CHECK( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
	GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[framebuffer_id].buffer) );
	_buffer_size = framebuffers[framebuffer_id].size;

	GL_CHECK( glViewport( 0, 0, _buffer_size.x, _buffer_size.y ) );
	if(_setup.clear_buffer)
		GL_CHECK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );

	if(!started_drawing)
	{
		whitgl_profile_start_drawing();
		started_drawing = true;
	}
}

void _whitgl_populate_vertices(float* vertices, whitgl_iaabb s, whitgl_iaabb d, whitgl_ivec image_size)
{
	// cpu optimisation for "whitgl_faabb sf = whitgl_faabb_divide(whitgl_iaabb_to_faabb(s), whitgl_ivec_to_fvec(image_size));"
	whitgl_faabb sf = {{((float)s.a.x)/((float)image_size.x),((float)s.a.y)/((float)image_size.y)},
                       {((float)s.b.x)/((float)image_size.x),((float)s.b.y)/((float)image_size.y)}};
	whitgl_int i = 0;
	vertices[i++] = d.a.x; vertices[i++] = d.b.y; vertices[i++] = 1; vertices[i++] = sf.a.x; vertices[i++] = sf.b.y;
	vertices[i++] = d.b.x; vertices[i++] = d.a.y; vertices[i++] = 1; vertices[i++] = sf.b.x; vertices[i++] = sf.a.y;
	vertices[i++] = d.a.x; vertices[i++] = d.a.y; vertices[i++] = 1; vertices[i++] = sf.a.x; vertices[i++] = sf.a.y;

	vertices[i++] = d.a.x; vertices[i++] = d.b.y; vertices[i++] = 1; vertices[i++] = sf.a.x; vertices[i++] = sf.b.y;
	vertices[i++] = d.b.x; vertices[i++] = d.b.y; vertices[i++] = 1; vertices[i++] = sf.b.x; vertices[i++] = sf.b.y;
	vertices[i++] = d.b.x; vertices[i++] = d.a.y; vertices[i++] = 1; vertices[i++] = sf.b.x; vertices[i++] = sf.a.y;
}

void _whitgl_sys_orthographic(GLuint program, float left, float right, float top, float bottom)
{
	whitgl_fmat m = whitgl_fmat_orthographic(left, right, top, bottom, 0, 100);
	glUniformMatrix4fv( glGetUniformLocation( program, "m_model"), 1, GL_FALSE, whitgl_fmat_identity.mat);
	glUniformMatrix4fv( glGetUniformLocation( program, "m_view"), 1, GL_FALSE, whitgl_fmat_identity.mat);
	glUniformMatrix4fv( glGetUniformLocation( program, "m_perspective"), 1, GL_FALSE, m.mat);
	GL_CHECK( return );
}

void _whitgl_load_uniforms(whitgl_shader_slot slot)
{
	int i;
	for(i=0; i<shaders[slot].shader.num_uniforms; i++)
	{
		GLint location = glGetUniformLocation( shaders[slot].program, shaders[slot].shader.uniforms[i].name);
		switch(shaders[slot].shader.uniforms[i].type)
		{
			case WHITGL_UNIFORM_FLOAT:
			{
				glUniform1f(location, shaders[slot].uniforms[i].number);
				break;
			}
			case WHITGL_UNIFORM_FVEC:
			{
				whitgl_fvec v = shaders[slot].uniforms[i].fvec;
				glUniform2f(location, v.x, v.y);
				break;
			}
			case WHITGL_UNIFORM_FVEC3:
			{
				whitgl_fvec3 v = shaders[slot].uniforms[i].fvec3;
				glUniform3f(location, v.x, v.y, v.z);
				break;
			}
			case WHITGL_UNIFORM_COLOR:
			{
				whitgl_sys_color c = shaders[slot].uniforms[i].color;
				float r = ((float)c.r)/255.0;
				float g = ((float)c.g)/255.0;
				float b = ((float)c.b)/255.0;
				float a = ((float)c.a)/255.0;
				glUniform4f(location, r, g, b, a);
				break;
			}
			case WHITGL_UNIFORM_IMAGE:
			{
				glUniform1i(location, i+1); // i+1 here is imperfect, it'd be better to know how many images we are actually using
				whitgl_int id = shaders[slot].uniforms[i].image;
				int index = -1;
				int j;
				for(j=0; j<num_images; j++)
				{
					if(images[j].id == id)
					{
						index = j;
						break;
					}
				}
				if(index == -1)
				{
					WHITGL_PANIC("ERR Cannot find image %d", id);
					return;
				}
				glActiveTexture(GL_TEXTURE0 + 1 + i);
				glBindTexture(GL_TEXTURE_2D, images[index].gluint);
				break;
			}
			case WHITGL_UNIFORM_FRAMEBUFFER:
			{
				glUniform1i(location, i+1); // i+1 here is imperfect, it'd be better to know how many images we are actually using
				whitgl_int framebuffer = shaders[slot].uniforms[i].framebuffer;
				glActiveTexture(GL_TEXTURE0 + 1 + i);
				GL_CHECK( glBindTexture( GL_TEXTURE_2D, framebuffers[framebuffer].texture ) );
				break;
			}
			case WHITGL_UNIFORM_MATRIX:
			{
				whitgl_int j;
				GLfloat converted[16];
				for(j=0; j<16; j++)
					converted[j] = shaders[slot].uniforms[i].matrix.mat[j];
				glUniformMatrix4fv(location, 1, GL_FALSE, converted);
				break;
			}
		}
	}
	// for(i=0; i<shaders[slot].shader.num_matrices; i++)
	// {
	// 	GLint location = glGetUniformLocation(shaders[slot].program, shaders[slot].shader.matrices[i]);
	// 	whitgl_int j;
	// 	GLfloat converted[16];
	// 	for(j=0; j<16; j++)
	// 		converted[j] = shaders[slot].matrices[i].mat[j];
	// 	glUniformMatrix4fv( location, 1, GL_FALSE, converted);
	// }
	GL_CHECK( return );
}

void whitgl_sys_draw_finish()
{
	_whitgl_sys_flush_tex_iaabb();

	if(capture.do_next && (capture.pre_postprocess || capture.frame_buffer != 0))
	{
		whitgl_ivec capture_size = _buffer_size;
		if(capture.frame_buffer != 0)
		{
			GL_CHECK( glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[capture.frame_buffer].buffer) );
			capture_size = framebuffers[capture.frame_buffer].size;
		}
		unsigned char* flipped_buffer = malloc(capture_size.x*capture_size.y*4);
		glReadPixels(0, 0, capture_size.x, capture_size.y, GL_RGBA, GL_UNSIGNED_BYTE, flipped_buffer);

		unsigned char* buffer = malloc(capture_size.x*capture_size.y*4);
		whitgl_int i;
		for(i=0; i<capture_size.y; i++)
			memcpy(buffer+i*capture_size.x*4, flipped_buffer+(capture_size.y-1-i)*capture_size.x*4, capture_size.x*4);

		if(capture.to_file)
			whitgl_sys_save_png(capture.file, capture_size.x, capture_size.y, buffer);
		else
			memcpy(capture.data, buffer, capture_size.x*capture_size.y*4);
		free(buffer);
		free(flipped_buffer);

		capture.do_next = false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GL_CHECK( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
	GL_CHECK( glViewport( 0, 0, _window_size.x, _window_size.y ) );
	GL_CHECK( glActiveTexture( GL_TEXTURE0 ) );
	GL_CHECK( glBindTexture( GL_TEXTURE_2D, framebuffers[0].texture ) );

	float vertices[6*5];
	whitgl_iaabb src = whitgl_iaabb_zero;
	whitgl_iaabb dest = whitgl_iaabb_zero;
	src.b.x = _buffer_size.x;
	src.a.y = _buffer_size.y;

	if(_setup.resolution_mode == RESOLUTION_EXACT || _setup.resolution_mode == RESOLUTION_USE_WINDOW)
	{
		dest.b = _window_size;
	}
	else
	{
		dest.a.x = (_window_size.x-_buffer_size.x*_setup.pixel_size)/2;
		dest.a.y = (_window_size.y-_buffer_size.y*_setup.pixel_size)/2;
		dest.b.x = dest.a.x+_buffer_size.x*_setup.pixel_size;
		dest.b.y = dest.a.y+_buffer_size.y*_setup.pixel_size;
	}

	_whitgl_populate_vertices(vertices, src, dest, _buffer_size);
	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW ) );

	GLuint shaderProgram = shaders[WHITGL_SHADER_POST].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	GL_CHECK( glUniform1i( glGetUniformLocation( shaderProgram, "tex" ), 0 ) );
	_whitgl_load_uniforms(WHITGL_SHADER_POST);
	_whitgl_sys_orthographic(shaderProgram, 0, _window_size.x, 0, _window_size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );

	GLint texturePosAttrib = glGetAttribLocation( shaderProgram, "texturepos" );
	GL_CHECK( glVertexAttribPointer( texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), BUFFER_OFFSET(sizeof(float)*3) ) );
	GL_CHECK( glEnableVertexAttribArray( texturePosAttrib ) );

	GL_CHECK( glDrawArrays( GL_TRIANGLES, 0, 6 ) );

	if(capture.do_next && !capture.pre_postprocess)
	{
		whitgl_ivec capture_size = _window_size;

		unsigned char* flipped_buffer = malloc(capture_size.x*capture_size.y*4);
		glReadPixels(0, 0, capture_size.x, capture_size.y, GL_RGBA, GL_UNSIGNED_BYTE, flipped_buffer);

		unsigned char* buffer = malloc(capture_size.x*capture_size.y*4);
		whitgl_int i;
		for(i=0; i<capture_size.y; i++)
			memcpy(buffer+i*capture_size.x, flipped_buffer+(capture_size.y-1-i)*capture_size.x, capture_size.x);

		if(capture.to_file)
			whitgl_sys_save_png(capture.file, capture_size.x, capture_size.y, buffer);
		else
			memcpy(capture.data, buffer, capture_size.x*capture_size.y*4);
		free(buffer);
		free(flipped_buffer);

		capture.do_next = false;
	}

	whitgl_profile_end_frame();
	started_drawing = false;
	glfwSwapBuffers(_window);

	glfwPollEvents();
	GL_CHECK( glDisable(GL_BLEND) );
	whitgl_profile_start_frame();
}

void whitgl_sys_draw_buffer_pane(whitgl_int id, whitgl_fvec3 v[4], whitgl_shader_slot shader, whitgl_fmat m_model, whitgl_fmat m_view, whitgl_fmat m_perspective)
{
	_whitgl_sys_flush_tex_iaabb();


	if(shader >= WHITGL_SHADER_MAX)
	{
		WHITGL_PANIC("Invalid shader type %d", shader);
		return;
	}

	GL_CHECK( glActiveTexture( GL_TEXTURE0 ) );
	GL_CHECK( glBindTexture( GL_TEXTURE_2D, framebuffers[id].texture ) );


	float vertices[6*5];
	whitgl_int i=0;
	vertices[i++] = v[3].x; vertices[i++] = v[3].y; vertices[i++] = v[3].z; vertices[i++] = 1; vertices[i++] = 1;
	vertices[i++] = v[0].x; vertices[i++] = v[0].y; vertices[i++] = v[0].z; vertices[i++] = 0; vertices[i++] = 0;
	vertices[i++] = v[1].x; vertices[i++] = v[1].y; vertices[i++] = v[1].z; vertices[i++] = 1; vertices[i++] = 0;

	vertices[i++] = v[3].x; vertices[i++] = v[3].y; vertices[i++] = v[3].z; vertices[i++] = 1; vertices[i++] = 1;
	vertices[i++] = v[2].x; vertices[i++] = v[2].y; vertices[i++] = v[2].z; vertices[i++] = 0; vertices[i++] = 1;
	vertices[i++] = v[0].x; vertices[i++] = v[0].y; vertices[i++] = v[0].z; vertices[i++] = 0; vertices[i++] = 0;

	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW ) );

	GLuint shaderProgram = shaders[shader].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	GL_CHECK( glUniform1i( glGetUniformLocation( shaderProgram, "tex" ), 0 ) );

	_whitgl_load_uniforms(shader);
	glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "m_model"), 1, GL_FALSE, m_model.mat);
	glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "m_view"), 1, GL_FALSE, m_view.mat);
	glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "m_perspective"), 1, GL_FALSE, m_perspective.mat);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );


	GLint texturePosAttrib = glGetAttribLocation( shaderProgram, "texturepos" );
	GL_CHECK( glVertexAttribPointer( texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), BUFFER_OFFSET(sizeof(float)*3) ) );
	GL_CHECK( glEnableVertexAttribArray( texturePosAttrib ) );

	GL_CHECK( glDrawArrays( GL_TRIANGLES, 0, 6 ) );
}

void whitgl_sys_draw_iaabb(whitgl_iaabb rect, whitgl_sys_color col)
{
	_whitgl_sys_flush_tex_iaabb();
	float vertices[6*5];
	_whitgl_populate_vertices(vertices, whitgl_iaabb_zero, rect, whitgl_ivec_zero);

	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW ) );

	GLuint shaderProgram = shaders[WHITGL_SHADER_FLAT].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	whitgl_set_shader_color(WHITGL_SHADER_FLAT, 0, col);
	_whitgl_load_uniforms(WHITGL_SHADER_FLAT);
	_whitgl_sys_orthographic(shaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );

	GL_CHECK( glDrawArrays( GL_TRIANGLES, 0, 6 ) );
}

void whitgl_sys_draw_hollow_iaabb(whitgl_iaabb rect, whitgl_int width, whitgl_sys_color col)
{
	whitgl_iaabb n = {rect.a, {rect.b.x, rect.a.y+width}};
	whitgl_iaabb e = {{rect.b.x-width, rect.a.y}, rect.b};
	whitgl_iaabb s = {{rect.a.x, rect.b.y-width}, rect.b};
	whitgl_iaabb w = {rect.a, {rect.a.x+width, rect.b.y}};
	whitgl_sys_draw_iaabb(n, col);
	whitgl_sys_draw_iaabb(e, col);
	whitgl_sys_draw_iaabb(s, col);
	whitgl_sys_draw_iaabb(w, col);
}
void whitgl_sys_draw_line(whitgl_iaabb l, whitgl_sys_color col)
{
	_whitgl_sys_flush_tex_iaabb();
	float vertices[2*3];
	whitgl_int i = 0;
	vertices[i++] = l.a.x; vertices[i++] = l.a.y; vertices[i++] = 0;
	vertices[i++] = l.b.x; vertices[i++] = l.b.y; vertices[i++] = 0;

	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW ) );

	GLuint shaderProgram = shaders[WHITGL_SHADER_FLAT].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	whitgl_set_shader_color(WHITGL_SHADER_FLAT, 0, col);
	_whitgl_load_uniforms(WHITGL_SHADER_FLAT);
	_whitgl_sys_orthographic(shaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );

	GL_CHECK( glDrawArrays( GL_LINES, 0, 2 ) );
}
void whitgl_sys_draw_fcircle(whitgl_fcircle c, whitgl_sys_color col, int tris)
{
	_whitgl_sys_flush_tex_iaabb();
	int num_vertices = tris*3*3;
	whitgl_fvec scale = {c.size, c.size};
	float *vertices = malloc(sizeof(float)*num_vertices);
	int i;
	for(i=0; i<tris; i++)
	{
		whitgl_float dir;
		whitgl_fvec off;
		int vertex_offset = 3*3*i;
		vertices[vertex_offset++] = c.pos.x; vertices[vertex_offset++] = c.pos.y; vertices[vertex_offset++] = 0;

		dir = ((whitgl_float)(i+1))/tris * whitgl_pi * 2;
		off = whitgl_fvec_scale(whitgl_angle_to_fvec(dir), scale);
		vertices[vertex_offset++] = c.pos.x+off.x; vertices[vertex_offset++] = c.pos.y+off.y; vertices[vertex_offset++] = 0;

		dir = ((whitgl_float)i)/tris * whitgl_pi * 2 ;
		off = whitgl_fvec_scale(whitgl_angle_to_fvec(dir), scale);
		vertices[vertex_offset++] = c.pos.x+off.x; vertices[vertex_offset++] = c.pos.y+off.y; vertices[vertex_offset++] = 0;
	}

	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, sizeof(float)*num_vertices , vertices, GL_DYNAMIC_DRAW ) );

	GLuint shaderProgram = shaders[WHITGL_SHADER_FLAT].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	whitgl_set_shader_color(WHITGL_SHADER_FLAT, 0, col);
	_whitgl_load_uniforms(WHITGL_SHADER_FLAT);
	_whitgl_sys_orthographic(shaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );

	GL_CHECK( glDrawArrays( GL_TRIANGLES, 0, tris*3 ) );
	free(vertices);
}

void whitgl_sys_draw_model(whitgl_int id, whitgl_shader_slot shader, whitgl_fmat m_model, whitgl_fmat m_view, whitgl_fmat m_perspective)
{
	_whitgl_sys_flush_tex_iaabb();

	int index = -1;
	int i;
	for(i=0; i<num_models; i++)
	{
		if(models[i].id == id)
		{
			index = i;
			break;
		}
	}
	if(index == -1)
	{
		WHITGL_PANIC("ERR Cannot find model %d", id);
		return;
	}
	if(shader >= WHITGL_SHADER_MAX)
	{
		WHITGL_PANIC("Invalid shader type %d", shader);
		return;
	}

	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, models[index].vbo ) );

	GLuint shaderProgram = shaders[shader].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	_whitgl_load_uniforms(shader);
	glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "m_model"), 1, GL_FALSE, m_model.mat);
	glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "m_view"), 1, GL_FALSE, m_view.mat);
	glUniformMatrix4fv( glGetUniformLocation( shaderProgram, "m_perspective"), 1, GL_FALSE, m_perspective.mat);


	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );

	GLint vertexColor = glGetAttribLocation( shaderProgram, "vertexColor" );
	if(vertexColor > -1)
	{
		GL_CHECK( glVertexAttribPointer( vertexColor, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), BUFFER_OFFSET(sizeof(float)*3) ) );
		GL_CHECK( glEnableVertexAttribArray( vertexColor ) );
	}

	GLint vertexNormal = glGetAttribLocation( shaderProgram, "vertexNormal" );
	if(vertexNormal > -1)
	{
		GL_CHECK( glVertexAttribPointer( vertexNormal, 3, GL_FLOAT, GL_FALSE, 9*sizeof(float), BUFFER_OFFSET(sizeof(float)*6) ) );
		GL_CHECK( glEnableVertexAttribArray( vertexNormal ) );
	}

	GL_CHECK( glDrawArrays( GL_TRIANGLES, 0, models[index].num_vertices ) );

	if(vertexColor > -1)
		GL_CHECK( glDisableVertexAttribArray(vertexColor) );
	if(vertexNormal > -1)
		GL_CHECK( glDisableVertexAttribArray(vertexNormal) );
}

whitgl_int buffer_curindex = -1;
static const whitgl_int buffer_num_quads = 128;
float buffer_vertices[128*6*5];
whitgl_int buffer_index = 0;
void _whitgl_sys_flush_tex_iaabb()
{
	if(buffer_index == 0 || buffer_curindex == -1)
		return;
	GL_CHECK( glActiveTexture( GL_TEXTURE0 ) );
	GL_CHECK( glBindTexture( GL_TEXTURE_2D, images[buffer_curindex].gluint ) );

	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, sizeof(float)*buffer_index*6*5, buffer_vertices, GL_DYNAMIC_DRAW ) );

	GLuint shaderProgram = shaders[WHITGL_SHADER_TEXTURE].program;
	GL_CHECK( glUseProgram( shaderProgram ) );
	GL_CHECK( glUniform1i( glGetUniformLocation( shaderProgram, "tex" ), 0 ) );
	_whitgl_load_uniforms(WHITGL_SHADER_TEXTURE);
	_whitgl_sys_orthographic(shaderProgram, 0, _setup.size.x, 0, _setup.size.y);

	#define BUFFER_OFFSET(i) ((void*)(i))
	GLint posAttrib = glGetAttribLocation( shaderProgram, "position" );
	GL_CHECK( glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0 ) );
	GL_CHECK( glEnableVertexAttribArray( posAttrib ) );

	GLint texturePosAttrib = glGetAttribLocation( shaderProgram, "texturepos" );
	GL_CHECK( glVertexAttribPointer( texturePosAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), BUFFER_OFFSET(sizeof(float)*3) ) );
	GL_CHECK( glEnableVertexAttribArray( texturePosAttrib ) );

	GL_CHECK( glDrawArrays( GL_TRIANGLES, 0, buffer_index*6 ) );
	buffer_curindex = -1;
	buffer_index = 0;
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
			break;
		}
	}
	if(index == -1)
	{
		WHITGL_PANIC("ERR Cannot find image %d", id);
		return;
	}
	if(buffer_curindex != index || buffer_index+1 >= buffer_num_quads)
	{
		if(buffer_curindex != -1)
			_whitgl_sys_flush_tex_iaabb();
		buffer_curindex = index;
	}
	_whitgl_populate_vertices(&buffer_vertices[buffer_index*6*5], src, dest, images[index].size);
	buffer_index++;
}

void whitgl_sys_draw_sprite(whitgl_sprite sprite, whitgl_ivec frame, whitgl_ivec pos)
{
	whitgl_sys_draw_sprite_sized(sprite, frame, pos, sprite.size);
}

void whitgl_sys_draw_sprite_sized(whitgl_sprite sprite, whitgl_ivec frame, whitgl_ivec pos, whitgl_ivec dest_size)
{
	whitgl_iaabb src = whitgl_iaabb_zero;
	whitgl_ivec offset = whitgl_ivec_scale(sprite.size, frame);
	src.a = whitgl_ivec_add(sprite.top_left, offset);
	src.b = whitgl_ivec_add(src.a, sprite.size);
	whitgl_iaabb dest = whitgl_iaabb_zero;
	dest.a = pos;
	dest.b = whitgl_ivec_add(dest.a, dest_size);
	whitgl_sys_draw_tex_iaabb(sprite.image, src, dest);
}

void whitgl_sys_draw_text(whitgl_sprite sprite, const char* string, whitgl_ivec pos)
{
	whitgl_ivec draw_pos = pos;
	while(*string)
	{
		int index = -1;
		if(*string >= 'a' && *string <= 'z')
			index = *string-'a';
		if(*string >= 'A' && *string <= 'Z')
			index = *string-'A';
		if(*string >= '0' && *string <= '9')
			index = *string-'0'+26;
		if(*string == ',')
			index = 36;
		if(*string == '.')
			index = 37;
		if(*string == ':')
			index = 38;
		if(*string == '$')
			index = 39;
		if(*string == '!')
			index = 40;
		if(*string == '\'')
			index = 41;
		if(*string == '?')
			index = 42;
		if(*string == '-')
			index = 43;
		if(*string == '<')
			index = 44;
		if(*string == '>')
			index = 45;
		if(*string == ' ')
			draw_pos.x += sprite.size.x;
		if(index != -1)
		{
			whitgl_ivec frame = {index%6, index/6};
			whitgl_sys_draw_sprite(sprite, frame, draw_pos);
			draw_pos.x += sprite.size.x;
		}
		string++;
	}
}

bool whitgl_sys_load_png(const char *name, whitgl_int *width, whitgl_int *height, unsigned char **data)
{
	png_image image;
	memset(&image, 0, (sizeof image));
	image.version = PNG_IMAGE_VERSION;
	if (png_image_begin_read_from_file(&image, name) == 0)
		return false;

	image.format = PNG_FORMAT_RGBA;
	*data = (unsigned char*) malloc(PNG_IMAGE_SIZE(image));

	if(*data == NULL)
		return false;

	if (png_image_finish_read(&image, NULL, *data, 0, NULL) == 0)
	{
		if (*data == NULL)
			png_image_free(&image);
		else
			free(*data);
		return false;
	}
	*width = image.width;
	*height = image.height;
	return true;
}
bool whitgl_sys_save_png(const char *name, whitgl_int width, whitgl_int height, unsigned char *data)
{
	png_image save;

	memset(&save, 0, sizeof save);
	save.version = PNG_IMAGE_VERSION;
	save.width = width;
	save.height = height;
	save.format = PNG_FORMAT_RGBA;

	if (png_image_write_to_file(&save, name, 0, data, 0, NULL) == 0)
		return false;
	return true;
}

void whitgl_sys_add_image_from_data(int id, whitgl_ivec size, unsigned char* data)
{
	if(num_images >= WHITGL_IMAGE_MAX)
	{
		WHITGL_PANIC("ERR Too many images");
		return;
	}

	images[num_images].size = size;
	GL_CHECK( glPixelStorei(GL_UNPACK_ALIGNMENT, 1) );
	GL_CHECK( glPixelStorei(GL_PACK_LSB_FIRST, 1) );
	GL_CHECK( glGenTextures(1, &images[num_images].gluint ) );
	GL_CHECK( glActiveTexture( GL_TEXTURE0 ) );
	GL_CHECK( glBindTexture(GL_TEXTURE_2D, images[num_images].gluint) );
	GL_CHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
	GL_CHECK( glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );
	GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );
	GL_CHECK( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) );
	GL_CHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x,
				 size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				 data) );

	images[num_images].id = id;
	num_images++;
}
void whitgl_sys_capture_frame(const char *name, bool pre_postprocess)
{
	capture = whitgl_frame_capture_zero;
	capture.to_file = true;
	capture.do_next = true;
	capture.pre_postprocess = pre_postprocess;
	strncpy(capture.file, name, sizeof(capture.file));
}
void whitgl_sys_capture_frame_to_data(whitgl_sys_color* data, bool pre_postprocess, int frame_buffer)
{
	capture = whitgl_frame_capture_zero;
	capture.to_file = false;
	capture.do_next = true;
	capture.pre_postprocess = pre_postprocess;
	capture.data = data;
	capture.frame_buffer = frame_buffer;
}

void whitgl_sys_update_image_from_data(int id, whitgl_ivec size, unsigned char* data)
{
	int index = -1;
	int i;
	for(i=0; i<num_images; i++)
	{
		if(images[i].id == id)
		{
			index = i;
			break;
		}
	}
	if(index == -1)
	{
		whitgl_sys_add_image_from_data(id, size, data);
		return;
	}
	if(images[index].size.x != size.x || images[index].size.y != size.y)
	{
		WHITGL_PANIC("ERR Image sizes don't match");
		return;
	}
	GL_CHECK( glActiveTexture( GL_TEXTURE0 ) );
	GL_CHECK( glBindTexture(GL_TEXTURE_2D, images[index].gluint) );
	GL_CHECK( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x,
				 size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				 data) );
}

void whitgl_sys_add_image(int id, const char* filename)
{
	GLubyte *textureImage;
	whitgl_ivec size;
	WHITGL_LOG("Adding image: %s", filename);
	bool success = whitgl_sys_load_png(filename, &size.x, &size.y, &textureImage);
	if(!success)
	{
		WHITGL_PANIC("loadPngImage error");
	}
	whitgl_sys_add_image_from_data(id, size, textureImage);
	free(textureImage);
}

whitgl_bool whitgl_load_model(whitgl_int id, const char* filename)
{
	FILE *src;
	int read;
	int readSize;
	src = fopen(filename, "rb");
	if (src == NULL)
	{
		WHITGL_LOG("Failed to open %s for load.", filename);
		return false;
	}
	read = fread( &readSize, 1, sizeof(readSize), src );
	if(read != sizeof(readSize))
	{
		WHITGL_LOG("Failed to read size from %s", filename);
		fclose(src);
		return false;
	}
	GLfloat* data = (GLfloat*)malloc(readSize);
	read = fread( data, 1, readSize, src );
	if(read != readSize)
	{
		WHITGL_LOG("Failed to read object from %s", filename);
		fclose(src);
		return false;
	}
	WHITGL_LOG("Loaded data from %s", filename);
	fclose(src);

	whitgl_int num_vertices = ((readSize/sizeof(GLfloat))/3)/3;

	whitgl_sys_update_model_from_data(id, num_vertices, (char*)data);
	free(data);
	return true;
}

void whitgl_sys_update_model_from_data(int id, whitgl_int num_vertices, const char* data)
{
	if(num_vertices < 0)
		WHITGL_PANIC("invalid num_vertices");
	int index = -1;
	int i;
	for(i=0; i<num_models; i++)
	{
		if(models[i].id == id)
		{
			index = i;
			break;
		}
	}
	if(index == -1)
	{
		// not added yet, add now
		if(num_models >= WHITGL_MODEL_MAX)
			WHITGL_PANIC("ERR Too many models");
		models[num_models] = whitgl_model_zero;
		models[num_models].id = id;
		index = num_models;
		GL_CHECK( glGenBuffers( 1, &models[num_models].vbo ) ); // Generate 1 buffer
		num_models++;
	}

	if(num_vertices > models[index].max_vertices)
	{
		GL_CHECK( glDeleteBuffers(1, &models[index].vbo) );
		GL_CHECK( glGenBuffers( 1, &models[index].vbo ) ); // Generate 1 buffer
		models[index].max_vertices = num_vertices;
	}
	models[index].num_vertices = num_vertices;


	GL_CHECK( glBindBuffer( GL_ARRAY_BUFFER, models[index].vbo ) );
	GL_CHECK( glBufferData( GL_ARRAY_BUFFER, 4*9*num_vertices, data, GL_DYNAMIC_DRAW ) );
}

whitgl_ivec whitgl_sys_get_image_size(whitgl_int id)
{
	int index = -1;
	int i;
	for(i=0; i<num_images; i++)
	{
		if(images[i].id == id)
		{
			index = i;
			break;
		}
	}
	if(index == -1)
	{
		WHITGL_PANIC("ERR Cannot find image %d", id);
		return whitgl_ivec_zero;
	}
	return images[index].size;
}

whitgl_float whitgl_sys_get_time()
{
	return glfwGetTime();
}

whitgl_sys_color whitgl_sys_color_blend(whitgl_sys_color a, whitgl_sys_color b, whitgl_float factor)
{
	whitgl_sys_color out;
	whitgl_int fac = 256*factor;
	whitgl_int inv = 256*(1-factor);
	out.r = (a.r*inv + b.r*fac)>>8;
	out.g = (a.g*inv + b.g*fac)>>8;
	out.b = (a.b*inv + b.b*fac)>>8;
	out.a = (a.a*inv + b.a*fac)>>8;
	return out;
}
whitgl_sys_color whitgl_sys_color_multiply(whitgl_sys_color a, whitgl_sys_color b)
{
	whitgl_sys_color out;
	out.r = (((whitgl_int)a.r)*((whitgl_int)b.r))>>8;
	out.g = (((whitgl_int)a.g)*((whitgl_int)b.g))>>8;
	out.b = (((whitgl_int)a.b)*((whitgl_int)b.b))>>8;
	out.a = (((whitgl_int)a.a)*((whitgl_int)b.a))>>8;

	return out;
}
whitgl_bool whitgl_sys_window_focused()
{
	return _windowFocused;
}
void whitgl_sys_enable_depth(whitgl_bool enable)
{
	if(enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}
void whitgl_sys_cull_side(whitgl_bool cull_front)
{
	glCullFace(cull_front ? GL_FRONT : GL_BACK);
}
void whitgl_set_clipboard(const char* string)
{
	glfwSetClipboardString(_window, string);
}
const char* whitgl_get_clipboard()
{
	return glfwGetClipboardString(_window);
}

#define MAX_DRAG_PATHS (16)
char _drag_and_drop_paths[PATH_MAX*MAX_DRAG_PATHS];
whitgl_int _num_drops = 0;

void _whitgl_sys_drop_callback(GLFWwindow* window, int count, const char** paths)
{
	WHITGL_LOG("_whitgl_sys_drop_callback %d", count);
	(void)window;
	if(count < 1)
		return;

	_num_drops = 0;

	whitgl_int i;
	for(i=0; i<count && _num_drops < MAX_DRAG_PATHS; i++)
	{
		WHITGL_LOG("Recieved drag-and-drop file: %s", paths[i]);
		strncpy(&_drag_and_drop_paths[i*PATH_MAX], paths[i], PATH_MAX);
		_num_drops++;
	}
}

bool whitgl_get_drag_and_drop(char filename[PATH_MAX])
{
	if(_num_drops <= 0)
		return false;
	_num_drops--;
	strncpy(filename, &_drag_and_drop_paths[_num_drops*PATH_MAX], PATH_MAX);
	return true;
}

void whitgl_change_icon(const char* filename)
{
	GLFWimage icon;
	whitgl_int width, height;
	if(whitgl_sys_load_png(filename, &width, &height, &icon.pixels))
	{
		icon.width = width;
		icon.height = height;
		glfwSetWindowIcon(_window, 1, &icon );
		free(icon.pixels);
	}
}


void whitgl_grab_focus()
{
	glfwFocusWindow(_window);
}
