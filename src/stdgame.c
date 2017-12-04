/**
 * @file stdgame.c
 * @author Gerviba (Szabo Gergely)
 * @brief The main header of the project
 *
 * @par Header:
 * 		stdgame.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "stdgame.h"

static void initGLFW();
static void createWindow(GameInstance* this, const GLFWvidmode* mode);
static void setupOpenGL(GameInstance* this, double width, double height);
static void setupWindowSize(const GLFWvidmode* mode, GameInstance* this);
static void setPerspective(GameInstance *this, float fov, float aspect, float near, float far);
static void printVersionInfo(void);
static void initCursor(GameInstance* this);
static void doGameLoop(GameInstance* this);
static void freeGameInstance(GameInstance* this);

/**
 * The main method
 *
 * This is where the story begins.
 */
int main(int argc, char *argv[]) {
	GameInstance *this = new(GameInstance);
	getGameInstance(&this);

	this->shader = new(ShaderInfo);
	this->lighting = new(LigingInfo);
	this->lighting->numLights = 0;
	this->camera = new(CameraInfo);
	setRotation(this->camera->rotation, 0.0f, 0.0f, 0.0f);
	this->options = new(Options);
	this->options->selectedToSet = NULL;
	this->player = NULL;

	loadDefaultOptions(this);
	loadOptions(this);

	do {
		this->options->reloadProgram = GL_FALSE;
		initGLFW(this);
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		createWindow(this, mode);
		setupWindowSize(mode, this);
		printVersionInfo();
		initCursor(this);
		gameInit(this);
		fixViewport(this);

		doGameLoop(this);

		saveOptions(this);
		glfwDestroyWindow(this->window);
		glfwTerminate();
	} while(this->options->reloadProgram);

	freeGameInstance(this);
	DEBUG("Info", "Closing game");

	exit(EXIT_SUCCESS);
}

/**
 * Free all the memory allocated values from GameInstance
 * and then the GameInstance itself.
 *
 * @param this Actual GameInstance instance
 */
static void freeGameInstance(GameInstance* this) {
	free(this->shader);
	free(this->lighting);
	free(this->camera);
	free(this->options);

	if (this->player != NULL)
		freePlayer(this);

	int i;
	for (i = 0; i < this->cursor->cursorObject->size; ++i) {
		listFree(this->cursor->cursorObject->parts[i].parts);
		free(this->cursor->cursorObject->parts[i].parts);
	}
	listFree(this->cursor->cursorObject->parts[0].colors);
	free(this->cursor->cursorObject->parts[0].colors);

	free(this->cursor->cursorObject->parts);
	free(this->cursor->cursorObject);
	free(this->cursor->pointer);
	free(this->cursor);

	Iterator it;
	foreach (it, this->font->chars->first) {
		Char *c = it->data;
		listFree(c->parts);
		free(c->parts);
	}
	listFree(this->font->chars);
	free(this->font->chars);

	free(this->font->colors);
	listFree(this->font->unknown->parts);
	free(this->font->unknown->parts);
	free(this->font->unknown);
	free(this->font);

	listFree(this->referencePoints);
	free(this->referencePoints);
	free(this);
}

/**
 * Initialize GLFW
 *
 * @param this Actual GameInstance instance
 */
static void initGLFW(GameInstance *this) {
	glfwSetErrorCallback(onErrorEvent);

	if (!glfwInit()) {
		ERROR("GLFW init failed");
		exit(EXIT_FAILURE);
	}

	if (this->options->msaa != 0)
		glfwWindowHint(GLFW_SAMPLES, this->options->msaa);
	DEBUG("Info", "MSAA: x%d", this->options->msaa);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
}

/**
 * Create GLFW window
 *
 * @param this Actual GameInstance instance
 * @param mode GLFW video mode
 */
static void createWindow(GameInstance* this, const GLFWvidmode* mode) {
	if (this->options->fullscreen) {
		this->window = glfwCreateWindow(mode->width, mode->height,
				"stdgame | The Epic Platformer Game", glfwGetPrimaryMonitor(), NULL);
	} else {
		if (this->options->windowedWidth == 0)
			this->options->windowedWidth = this->options->width;
		if (this->options->windowedHeight == 0)
			this->options->windowedHeight = this->options->height;

		this->window = glfwCreateWindow(this->options->windowedWidth,
				this->options->windowedHeight, "stdgame | The Epic Platformer Game",
				NULL, NULL);
	}

	this->options->height = mode->height;
	this->options->width = mode->width;

	DEBUG("Info", "Window size: %dx%d", mode->width, mode->height); //TODO: Load from config
	if (!this->window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetMouseButtonCallback(this->window, onClickEvent);
	glfwSetKeyCallback(this->window, onKeyEvent);
	glfwSetScrollCallback(this->window, onScrollEvent);
	glfwSetCharModsCallback(this->window, onCharModEvent);
	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(1);
}

/**
 * Setup OpenGL
 *
 * @param this Actual GameInstance instance
 * @param width Width of the window
 * @param height Height of the window
 */
static void setupOpenGL(GameInstance *this, double width, double height) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	if (this->options->msaa != 0)
		glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setPerspective(this, PI / 4.0f, (float) width / (float) height, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
}

/**
 * Setup GLFW window size
 *
 * @param mode GLFW video mode
 * @param this Actual GameInstance instance
 */
static void setupWindowSize(const GLFWvidmode* mode, GameInstance* this) {
	if (this->options->fullscreen) {
		setupOpenGL(this, mode->width, mode->height);
	} else {
		if (this->options->windowedWidth == 0)
			this->options->windowedWidth = this->options->width;

		if (this->options->windowedHeight == 0)
			this->options->windowedHeight = this->options->height;

		setupOpenGL(this, this->options->windowedWidth,
				this->options->windowedHeight);
	}
}

/**
 * Setup the perspective, projection matrix
 *
 * @param this Actual GameInstance instance
 * @param fov Field of view
 * @param aspect Aspect ratio
 * @param near Near value
 * @param far Far value
 */
static void setPerspective(GameInstance *this, float fov, float aspect, float near, float far) {
	this->options->tanFov = tanf(fov);
	this->options->aspectRatio = aspect;
	float temp = 1.0f / tanf(fov / 2.0f);

	this->camera->projMat[0x0] = temp / aspect;
	this->camera->projMat[0x1] = 0.0f;
	this->camera->projMat[0x2] = 0.0f;
	this->camera->projMat[0x3] = 0.0f;

	this->camera->projMat[0x4] = 0.0f;
	this->camera->projMat[0x5] = temp;
	this->camera->projMat[0x6] = 0.0f;
	this->camera->projMat[0x7] = 0.0f;

	this->camera->projMat[0x8] = 0.0f;
	this->camera->projMat[0x9] = 0.0f;
	this->camera->projMat[0xA] = (far + near) / (near - far);
	this->camera->projMat[0xB] = -1.0f;

	this->camera->projMat[0xC] = 0.0f;
	this->camera->projMat[0xD] = 0.0f;
	this->camera->projMat[0xE] = (2.0f * far * near) / (near - far);
	this->camera->projMat[0xF] = 0.0f;

	glMultMatrixf(this->camera->projMat);
}

/**
 * Prints OpenGL and GLFW version numbers
 */
static void printVersionInfo(void) {
	printf("[Info] OpenGL version supported: (%s) \n", glGetString(GL_VERSION));
	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);
	printf("[Info] Running against GLFW %i.%i.%i\n", major, minor, revision);
}

/**
 * Initialize the cursor
 *
 * @param this Actual GameInstance instance
 */
static void initCursor(GameInstance* this) {
	this->cursor = new(Cursor);
	this->cursor->cursorObject = loadActiveObject("assets/objects/cursor.aobj");
	this->cursor->pointer = new(ActiveObjectInstance);
	this->cursor->pointer->id = -2147483647;
	setPosition(this->cursor->pointer->position, 0.0f, 0.0f, 0.0f);
	setRotation(this->cursor->pointer->rotation, 0.0f, 0.0f, 0.0f);
	setScale(this->cursor->pointer->scale, 1.0, 1.0, 1.0);
	this->cursor->pointer->visible = GL_TRUE;
	this->cursor->pointer->activePart = 0;
	this->cursor->pointer->object = this->cursor->cursorObject;
}

/**
 * Fix the viewport size
 *
 * Sometimes there is a black rectangle at the top the window. This function fix it.
 *
 * @param this Actual GameInstance instance
 */
void fixViewport(GameInstance* this) {
	int width, height;
	glfwGetFramebufferSize(this->window, &width, &height);
	glViewport(0, 0, width, height);
}

/**
 * GameInstance getter for GLFW event functions
 *
 * @warning  Don't use it is you can.
 *
 * @param this Actual GameInstance instance
 */
void getGameInstance(GameInstance **this) {
	static GameInstance *storedInstance = NULL;
	if (storedInstance == NULL)
		storedInstance = *this;

	*this = storedInstance;
}

/**
 * Game loop method
 *
 * @param this Actual GameInstance instance
 */
static void doGameLoop(GameInstance* this) {
	while (!glfwWindowShouldClose(this->window)) {
		glfwPollEvents();
		onLogic(this);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		onRender(this);
		glfwSwapBuffers(this->window);
	}
	freeMap(this->map);
}

