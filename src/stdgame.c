#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stdgame.h"

static int mode = 0;
static int modelId = 0;

static void getGameInstance(GameInstance **this) {
	static GameInstance *storedInstance = NULL;
	if (storedInstance == NULL)
		storedInstance = *this;

	*this = storedInstance;
}

/**
 * TODO: Remove this chaos:
 */
void debugKeyPress(const char key, int x, int y) {
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
					obj->position[0], obj->position[1], obj->position[2],
					obj->rotation[0], obj->rotation[1], obj->rotation[2],
					obj->scale[0], obj->scale[1], obj->scale[2]);
			break;

			default:
				printf("Unregistered KEY: %d %c\n", key, key);
				break;
		}
	}
}

static void onError(int error, const char* description) {
	DEBUG("Error", description);
}

void onKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

#ifdef DEBUG_MOVEMENT
	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		GameInstance *gi = NULL;
		getGameInstance(&gi);
		printf("%g %g\n", gi->player->position[X], gi->player->position[Y]);
	}
	debugKeyPress(key, 0, 0);
#endif
}

void printVersionInfo() {
	printf("[Info] OpenGL version supported: (%s) \n", glGetString(GL_VERSION));
	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);
	printf("[Info] Running against GLFW %i.%i.%i\n", major, minor, revision);
}

void fixViewport(GameInstance* this) {
	int width, height;
	glfwGetFramebufferSize(this->window, &width, &height);
	glViewport(0, 0, width, height);
}

void setPerspective(GameInstance *this, float fov, float aspect, float near, float far) {
	this->options->tanFov = tanf(fov);
	this->options->aspectRatio = 1.0 / aspect;
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

void setupOpenGL(GameInstance* this, double width, double height) {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

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

void doGameLoop(GameInstance* this) {
	while (!glfwWindowShouldClose(this->window)) {
		onLogic(this);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		onRender(this);
		glfwSwapBuffers(this->window);
		glfwPollEvents();
	}
}

void initGLFW() {
	glfwSetErrorCallback(onError);

	if (!glfwInit()) {
		ERROR("[ERROR] Init failed\n");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
}

void createWindow(GameInstance* this, const GLFWvidmode* mode) {
	this->window = glfwCreateWindow(mode->width, mode->height,
			"stdgame | The Epic Platformer Game", glfwGetPrimaryMonitor(),
			NULL);
	this->options->height = mode->height;
	this->options->width = mode->width;

	DEBUG("Info", "Window size: %dx%d", mode->width, mode->height); //TODO: Load from config
	if (!this->window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// TODO: Icon
	//	GLFWimage images[2];
	//	images[0] = load_icon("my_icon.png");
	//	images[1] = load_icon("my_icon_small.png");
	//	glfwSetWindowIcon(window, 2, images);

	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetKeyCallback(this->window, onKeyAction);
	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(1);
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

int main(int argc, char *argv[]) {
	GameInstance *this = new(GameInstance);
	getGameInstance(&this);

	this->shader = new(ShaderInfo);
	this->lighting = new(LigingInfo);
	this->lighting->numLights = 0;
	this->camera = new(CameraInfo);
	setRotation(this->camera->rotation, 0.0f, 0.0f, 0.0f);
	this->options = new(Options);

	initGLFW();
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	createWindow(this, mode);
	setupOpenGL(this, mode->width, mode->height);
	printVersionInfo();
	initCursor(this);
	gameInit(this);
	fixViewport(this);

	doGameLoop(this);

	DEBUG("Info", "Closing game");
	glfwDestroyWindow(this->window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
