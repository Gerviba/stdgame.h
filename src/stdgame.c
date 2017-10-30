#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "includes.h"

extern void gameInit(GameInstance *this);
extern void onRender(GameInstance *this);
extern void onLogic(GameInstance *this);
extern void setPerspective(GameInstance *this, float fov, float aspect, float near, float far);

//extern GameInstance *this;
//
//static int mode = 0;
//static int modelId = 0;
//
//void debugKeyPress(unsigned char key, int x, int y) {
//	if (mode == 0) {
//		switch (key) {
//			case 27:
//				exit(0);
//				break;
//
//			// Debug view
//			case 'a': this->camera->rotation[1] += 2; break;
//			case 'd': this->camera->rotation[1] -= 2; break;
//			case 'w': this->camera->rotation[0] += 2; break;
//			case 's': this->camera->rotation[0] -= 2; break;
//			case 'q': this->camera->rotation[2] += 2; break;
//			case 'e': this->camera->rotation[2] -= 2; break;
//			case 'j': this->camera->position[0] -= 0.2; break;
//			case 'l': this->camera->position[0] += 0.2; break;
//			case 'k': this->camera->position[1] -= 0.2; break;
//			case 'i': this->camera->position[1] += 0.2; break;
//			case 'u': this->camera->position[2] -= 0.2; break;
//			case 'o': this->camera->position[2] += 0.2; break;
//
//			case 'n': mode = 1; break;
//			case 'm': mode = 1; break;
//
//			default:
//				printf("INVALID KEY: %d %c\n", key, key);
//				break;
//		}
//	} else if (mode == 1) {
//		ObjectInstance *obj;
//		ListElement *it;
//		for (it = this->map->objects->staticInstances->first; it != NULL; it = it->next) {
//			if (((ObjectInstance *)it->data)->id == modelId) {
//				obj = (ObjectInstance *)it->data;
//				break;
//			}
//		}
//		switch (key) {
//			case 27:
////				glutDestroyWindow(this->windowInstance);
//				exit(0);
//				break;
//
//			// Debug view
//			case 's': obj->position[1] += 1.0f / 16; break;
//			case 'w': obj->position[1] -= 1.0f / 16; break;
//			case 'a': obj->position[0] += 1.0f / 16; break;
//			case 'd': obj->position[0] -= 1.0f / 16; break;
//			case 'q': obj->position[2] += 1.0f / 16; break;
//			case 'e': obj->position[2] -= 1.0f / 16; break;
//
//			case 'f': obj->scale[0] -= 0.1; break;
//			case 'h': obj->scale[0] += 0.1; break;
//			case 'g': obj->scale[1] -= 0.1; break;
//			case 't': obj->scale[1] += 0.1; break;
//			case 'r': obj->scale[2] -= 0.1; break;
//			case 'z': obj->scale[2] += 0.1; break;
//
//			case 'j': obj->rotation[0] -= PI / 4; break;
//			case 'l': obj->rotation[0] += PI / 4; break;
//			case 'k': obj->rotation[1] -= PI / 4; break;
//			case 'i': obj->rotation[1] += PI / 4; break;
//			case 'u': obj->rotation[2] -= PI / 4; break;
//			case 'o': obj->rotation[2] += PI / 4; break;
//
//			case 'n': mode = 0; break;
//			case 'm': mode = 0; break;
//			case 'b': scanf("%d", &modelId); break;
//			case 'v': printf("POS: %f %f %f\nROT: %f %f %f\nSCL: %f %f %f\n",
//					obj->position[0], obj->position[1], obj->position[2],
//					obj->rotation[0], obj->rotation[1], obj->rotation[2],
//					obj->scale[0], obj->scale[1], obj->scale[2]);
//			break;
//
//			default:
//				printf("INVALID KEY: %d %c\n", key, key);
//				break;
//		}
//	}
//}

void onKeyAction(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
//	if (action == GLFW_PRESS)
//		debugKeyPress(key, 0, 0);
}

static void onError(int error, const char* description) {
	fprintf(stderr, "[Error] %s\n", description);
}

void printVersionInfo() {
	printf("[Info] OpenGL version supported: (%s): \n", glGetString(GL_VERSION));
	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);
	printf("[Info] Running against GLFW %i.%i.%i\n", major, minor, revision);
}

int main(int argc, char *argv[]) {
	GameInstance *this = malloc(sizeof(GameInstance));
	this->shader = malloc(sizeof(ShaderInfo));
	this->lighting = malloc(sizeof(LigingInfo));
	this->lighting->numLights = 0;
	this->camera = malloc(sizeof(CameraInfo));
	this->camera->rotation[0] = 0.0f;
	this->camera->rotation[1] = 0.0f;
	this->camera->rotation[2] = 0.0f;

	glfwSetErrorCallback(onError);

	if (!glfwInit()) {
		printf("[ERROR] Init failed");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	this->window = glfwCreateWindow(mode->width, mode->height,
			"stdgame | The Epic Platformer Game",
			glfwGetPrimaryMonitor(), NULL);
	printf("[Info] Window size: %dx%d\n", mode->width, mode->height); //TODO: Load from config

	if (!this->window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

//	glfwSetWindowCloseCallback(window, window_close_callback);
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	glfwSetKeyCallback(this->window, onKeyAction);

//	GLFWimage images[2];
//	images[0] = load_icon("my_icon.png");
//	images[1] = load_icon("my_icon_small.png");
//	glfwSetWindowIcon(window, 2, images);

	glfwMakeContextCurrent(this->window);
	glfwSwapInterval(1);


//	glutKeyboardFunc(handleKeyPress); //TODO: KeyUp and KeyDown eventek

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_MULTISAMPLE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setPerspective(this, PI / 4.0f, (double) mode->width / (double) mode->height, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);

	printVersionInfo();
	gameInit(this);

	while (!glfwWindowShouldClose(this->window)) {
		onLogic(this);

//		int width, height;
//		glfwGetFramebufferSize(window, &width, &height);
//		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		onRender(this);

		glfwSwapBuffers(this->window);
		glfwPollEvents();
	}

	glfwDestroyWindow(this->window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
