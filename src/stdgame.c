#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "includes.h"

extern void gameInit(void);
extern void onRender(void);
extern void onLogic(void);

extern void setPerspective(float fov, float aspect, float near, float far);

extern GameInstance *this;

static int mode = 0;
static int modelId = 0;

void handleKeyPress(unsigned char key, int x, int y) {
	if (mode == 0) {
		switch (key) {
			case 27:
				glutDestroyWindow(this->windowInstance);
				exit(0);
				break;

			// Debug view
			case 'a': this->camera->rotation[1] += 2; break;
			case 'd': this->camera->rotation[1] -= 2; break;
			case 'w': this->camera->rotation[0] += 2; break;
			case 's': this->camera->rotation[0] -= 2; break;
			case 'q': this->camera->rotation[2] += 2; break;
			case 'e': this->camera->rotation[2] -= 2; break;
			case 'j': this->camera->position[0] -= 0.2; break;
			case 'l': this->camera->position[0] += 0.2; break;
			case 'k': this->camera->position[1] -= 0.2; break;
			case 'i': this->camera->position[1] += 0.2; break;
			case 'u': this->camera->position[2] -= 0.2; break;
			case 'o': this->camera->position[2] += 0.2; break;

			case 'n': mode = 1; break;
			case 'm': mode = 1; break;

			default:
				printf("INVALID KEY: %d %c\n", key, key);
				break;
		}
	} else if (mode == 1) {
		ObjectInstance *obj;
		ListElement *it;
		for (it = this->objects->staticInstances->first; it != NULL; it = it->next) {
			if (((ObjectInstance *)it->data)->id == modelId) {
				obj = (ObjectInstance *)it->data;
				break;
			}
		}
		switch (key) {
			case 27:
				glutDestroyWindow(this->windowInstance);
				exit(0);
				break;

			// Debug view
			case 's': obj->position[1] += 1.0f / 16; break;
			case 'w': obj->position[1] -= 1.0f / 16; break;
			case 'a': obj->position[0] += 1.0f / 16; break;
			case 'd': obj->position[0] -= 1.0f / 16; break;
			case 'q': obj->position[2] += 1.0f / 16; break;
			case 'e': obj->position[2] -= 1.0f / 16; break;

			case 'f': obj->scale[0] -= 0.1; break;
			case 'h': obj->scale[0] += 0.1; break;
			case 'g': obj->scale[1] -= 0.1; break;
			case 't': obj->scale[1] += 0.1; break;
			case 'r': obj->scale[2] -= 0.1; break;
			case 'z': obj->scale[2] += 0.1; break;

			case 'j': obj->rotation[0] -= PI / 4; break;
			case 'l': obj->rotation[0] += PI / 4; break;
			case 'k': obj->rotation[1] -= PI / 4; break;
			case 'i': obj->rotation[1] += PI / 4; break;
			case 'u': obj->rotation[2] -= PI / 4; break;
			case 'o': obj->rotation[2] += PI / 4; break;

			case 'n': mode = 0; break;
			case 'm': mode = 0; break;
			case 'b': scanf("%d", &modelId); break;
			case 'v': printf("POS: %f %f %f\nROT: %f %f %f\nSCL: %f %f %f\n",
					obj->position[0], obj->position[1], obj->position[2],
					obj->rotation[0], obj->rotation[1], obj->rotation[2],
					obj->scale[0], obj->scale[1], obj->scale[2]);
			break;

			default:
				printf("INVALID KEY: %d %c\n", key, key);
				break;
		}
	}
}

void setPerspective(float fov, float aspect, float near, float far) {
	float f = 1.0f / tanf(fov / 2.0f);

	this->camera->projMat[0] = f / aspect;
	this->camera->projMat[1] = 0.0f;
	this->camera->projMat[2] = 0.0f;
	this->camera->projMat[3] = 0.0f;

	this->camera->projMat[4] = 0.0f;
	this->camera->projMat[5] = f;
	this->camera->projMat[6] = 0.0f;
	this->camera->projMat[7] = 0.0f;

	this->camera->projMat[8] = 0.0f;
	this->camera->projMat[9] = 0.0f;
	this->camera->projMat[10] = (far + near) / (near - far);
	this->camera->projMat[11] = -1.0f;

	this->camera->projMat[12] = 0.0f;
	this->camera->projMat[13] = 0.0f;
	this->camera->projMat[14] = (2.0f * far * near) / (near - far);
	this->camera->projMat[15] = 0.0f;

	glMultMatrixf(this->camera->projMat);
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	this = malloc(sizeof(GameInstance));
	this->shader = malloc(sizeof(ShaderInfo));
	this->lighting = malloc(sizeof(LigingInfo));
	this->lighting->numLights = 0;
	this->objects = malloc(sizeof(ObjectInfo));
	this->objects->staticInstances = newLinkedListPointer(sizeof(ObjectInstance));
	this->objects->staticObjects = newLinkedListPointer(sizeof(StaticObject));
	this->camera = malloc(sizeof(CameraInfo));
	this->camera->rotation[0] = 0.0f;
	this->camera->rotation[1] = 0.0f;
	this->camera->rotation[2] = 0.0f;
	this->windowInstance = glutCreateWindow("Gerviba's Platformer Game");

	glutDisplayFunc(onRender);
	glutIdleFunc(onLogic);
	glutKeyboardFunc(handleKeyPress); //TODO: KeyUp and KeyDown eventek

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setPerspective(PI / 4.0f, (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);

	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

	gameInit();
	glutMainLoop();
	return 0;
}
