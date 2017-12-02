#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "stdgame.h"

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

	DEBUG("Info", "Closing game");

	exit(EXIT_SUCCESS);
}

void initGLFW(GameInstance *this) {
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

void createWindow(GameInstance* this, const GLFWvidmode* mode) {
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

void setupOpenGL(GameInstance *this, double width, double height) {
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

void setupWindowSize(const GLFWvidmode* mode, GameInstance* this) {
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

void setPerspective(GameInstance *this, float fov, float aspect, float near, float far) {
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

void printVersionInfo() {
	printf("[Info] OpenGL version supported: (%s) \n", glGetString(GL_VERSION));
	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);
	printf("[Info] Running against GLFW %i.%i.%i\n", major, minor, revision);
}

void initCursor(GameInstance* this) {
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

void fixViewport(GameInstance* this) {
	int width, height;
	glfwGetFramebufferSize(this->window, &width, &height);
	glViewport(0, 0, width, height);
}

void doGameLoop(GameInstance* this) {
	while (!glfwWindowShouldClose(this->window)) {
		glfwPollEvents();
		onLogic(this);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		onRender(this);
		glfwSwapBuffers(this->window);
	}
}

void getGameInstance(GameInstance **this) {
	static GameInstance *storedInstance = NULL;
	if (storedInstance == NULL)
		storedInstance = *this;

	*this = storedInstance;
}

void onErrorEvent(int error, const char* description) {
	ERROR("[Error] [%d] %s", error, description);
}

void onClickEvent(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_RELEASE) {
		GameInstance *this = NULL;
		getGameInstance(&this);

		if (updateControlsMouse(this, button))
			return;
	}

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    	GameInstance *this = NULL;
		getGameInstance(&this);
		if (this->map->menu->onClick != NULL)
			this->map->menu->onClick(this);
    }

}

void onScrollEvent(GLFWwindow* window, double xOffset, double yOffset) {
	GameInstance *this = NULL;
	getGameInstance(&this);

	if (this->map->menu->onScroll != NULL)
		this->map->menu->onScroll(this, yOffset / 10);
}

void onCharModEvent(GLFWwindow* window, unsigned int key, int mods) {
	GameInstance *this = NULL;
	getGameInstance(&this);
	updateControlsKey(key, this);
}

void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	GameInstance *this = NULL;
	getGameInstance(&this);
	updateControlsMods(this, mods, key);

#ifdef DEBUG_MOVEMENT
	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		printf("%g %g\n", this->player->position[X], this->player->position[Y]);
	}
	onDebugKeyPress(key, 0, 0);
#endif
}

#ifdef DEBUG_MOVEMENT
void onDebugKeyPress(const char key, int x, int y) {
	static int mode = 0;
	static int modelId = 0;
	GameInstance *gi = NULL;
	getGameInstance(&gi);

	if (mode == 0) {
		switch (key) {
		case 'A': gi->camera->rotation[1] += 2; break;
		case 'D': gi->camera->rotation[1] -= 2; break;
		case 'W': gi->camera->rotation[0] += 2; break;
		case 'S': gi->camera->rotation[0] -= 2; break;
		case 'Q': gi->camera->rotation[2] += 2; break;
		case 'E': gi->camera->rotation[2] -= 2; break;
		case 'J': gi->camera->position[0] -= 0.2; break;
		case 'L': gi->camera->position[0] += 0.2; break;
		case 'K': gi->camera->position[1] -= 0.2; break;
		case 'I': gi->camera->position[1] += 0.2; break;
		case 'U': gi->camera->position[2] -= 0.2; break;
		case 'O': gi->camera->position[2] += 0.2; break;

		case 'N': mode = 2; break;
		case 'M': mode = 1; break;

		default:
			printf("Unregistered KEY: %d %c\n", key, key);
			break;
		}
	} else if (mode == 1) {
		StaticObjectInstance *obj;
		Iterator it;
		for (it = gi->map->objects->staticInstances->first; it != NULL; it = it->next) {
			if (((StaticObjectInstance *)it->data)->id == modelId) {
				obj = (StaticObjectInstance *)it->data;
				break;
			}
		}
		switch (key) {
		case 'S': obj->position[1] += 1.0f / 16; break;
		case 'W': obj->position[1] -= 1.0f / 16; break;
		case 'A': obj->position[0] += 1.0f / 16; break;
		case 'D': obj->position[0] -= 1.0f / 16; break;
		case 'Q': obj->position[2] += 1.0f / 16; break;
		case 'E': obj->position[2] -= 1.0f / 16; break;

		case 'F': obj->scale[0] -= 0.1; break;
		case 'H': obj->scale[0] += 0.1; break;
		case 'G': obj->scale[1] -= 0.1; break;
		case 'T': obj->scale[1] += 0.1; break;
		case 'R': obj->scale[2] -= 0.1; break;
		case 'Z': obj->scale[2] += 0.1; break;

		case 'J': obj->rotation[0] -= PI / 4; break;
		case 'L': obj->rotation[0] += PI / 4; break;
		case 'K': obj->rotation[1] -= PI / 4; break;
		case 'I': obj->rotation[1] += PI / 4; break;
		case 'U': obj->rotation[2] -= PI / 4; break;
		case 'O': obj->rotation[2] += PI / 4; break;

		case 'N': mode = 0; break;
		case 'M': mode = 2; break;
		case 'B': scanf("%d", &modelId); break;
		case 'V': printf("POS: %f %f %f\nROT: %f %f %f\nSCL: %f %f %f\n",
				obj->position[X], obj->position[Y], obj->position[Z],
				obj->rotation[X], obj->rotation[Y], obj->rotation[Z],
				obj->scale[X], obj->scale[Y], obj->scale[Z]);
		break;

		default:
			printf("Unregistered KEY: %d %c\n", key, key);
			break;
		}
	}
}
#endif
