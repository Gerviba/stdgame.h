#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
	#include <windows.h>
	#include <wingdi.h>
#else
	#include <sys/time.h>
#endif /* _WIN32 */

#include "includes.h"

extern GLuint shaderAttachFromFile(GLuint, GLenum, const char *); //TODO: add to shader.h
extern void onLogicIngame(GameInstance*, float);
extern void onLogicMenu(GameInstance*, float);

void setPerspective(GameInstance *this, float fov, float aspect, float near, float far) {
	this->options.tanFov = tanf(fov);
	this->options.aspectRatio = 1.0 / aspect;
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

static void loadDefaultOptions(GameInstance *this) {
	this->options.moveLeft[0] = GLFW_KEY_A;
	this->options.moveLeft[1] = GLFW_KEY_LEFT;
	this->options.moveRight[0] = GLFW_KEY_D;
	this->options.moveRight[1] = GLFW_KEY_RIGHT;
	this->options.jump[0] = GLFW_KEY_W;
	this->options.jump[1] = GLFW_KEY_SPACE;
	this->options.sneek[0] = GLFW_KEY_S;
	this->options.sneek[1] = GLFW_KEY_DOWN;
	this->options.attack[0] = GLFW_KEY_RIGHT_CONTROL;
}

void loadTileVAO(const GameInstance *this) {
	glGenVertexArrays(1, &this->tileVAO);
	glBindVertexArray(this->tileVAO);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLfloat v[] = {
			0.0f, 0.0f, 0.0f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,	1.0f, 1.0f, 	0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 	0.0f, 0.0f, 	0.0f, 0.0f, 1.0f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 32, v, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, sizeof(GLfloat) * 3);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, sizeof(GLfloat) * 5);
	glEnableVertexAttribArray(2);

	// TODO: cleanup
}

void gameInit(GameInstance *this) {
	loadDefaultOptions(this);

	this->shader->shaderId = glCreateProgram();
	shaderAttachFromFile(this->shader->shaderId, GL_VERTEX_SHADER, "assets/shaders/shader.vertex");
	shaderAttachFromFile(this->shader->shaderId, GL_FRAGMENT_SHADER, "assets/shaders/shader.fragment");

	glBindAttribLocation(this->shader->shaderId, 0, "position");
	glBindAttribLocation(this->shader->shaderId, 1, "texCoord");
	glBindAttribLocation(this->shader->shaderId, 2, "normal");

	GLint result;
	glLinkProgram(this->shader->shaderId);
	glGetProgramiv(this->shader->shaderId, GL_LINK_STATUS, &result);
	if (result == GL_FALSE) {
		GLint length;
		char *log;

		glGetProgramiv(this->shader->shaderId, GL_INFO_LOG_LENGTH, &length);
		log = malloc(length);
		glGetProgramInfoLog(this->shader->shaderId, length, &result, log);

		fprintf(stderr, "[Error] Shader program linking failed: %s\n", log);
		free(log);

		glDeleteProgram(this->shader->shaderId);
		this->shader->shaderId = 0;
	}

	this->shader->cameraPosition = glGetUniformLocation(this->shader->shaderId, "cameraPosition");
	this->shader->lightPosition = glGetUniformLocation(this->shader->shaderId, "lightPosition");
	this->shader->lightColor = glGetUniformLocation(this->shader->shaderId, "lightColor");
	this->shader->texturePosition = glGetUniformLocation(this->shader->shaderId, "tex");
	this->shader->numLights = glGetUniformLocation(this->shader->shaderId, "numLights");
	this->shader->baseColor = glGetUniformLocation(this->shader->shaderId, "baseColor");
	this->shader->projMat = glGetUniformLocation(this->shader->shaderId, "projMat");
	this->shader->viewMat = glGetUniformLocation(this->shader->shaderId, "viewMat");
	this->shader->moveMat = glGetUniformLocation(this->shader->shaderId, "moveMat");
	this->shader->modelMat = glGetUniformLocation(this->shader->shaderId, "modelMat");

	loadTileVAO(this);
	loadTexture(&this->blankTextureId, "null.png");

	this->state = MENU;
	this->map = loadMap(this, "assets/maps/menu.map");

	initFont(this);
	onLogic(this);
	printf("[Render] First logic done\n");

	updateCamera(this);
	glGetFloatv(GL_MODELVIEW_MATRIX, &this->camera->viewMat);
}

void updateCamera(GameInstance* this) {
	glLoadIdentity();
	glRotatef(-this->camera->rotation[X], 1.0f, 0.0f, 0.0f);
	glRotatef(-this->camera->rotation[Y], 0.0f, 1.0f, 0.0f);
	glRotatef(-this->camera->rotation[Z], 0.0f, 0.0f, 1.0f);
	glTranslatef(-this->camera->position[X],
			-this->camera->position[Y],
			-this->camera->position[Z]);
}

static void renderGUI(GameInstance* this) {
	GLfloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	renderFontTo(this, "Test message, $ *", (GLfloat[]) {6.0f, 7.0625f, 1.0f}, color, FS_NORMAL_DPI);

	char str[16];
	if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_PRESS)
		++this->score;
	sprintf(str, "%d *", this->score);

	renderFontTo(this, str, (GLfloat[]) {
			this->camera->position[X] + 2.0f - (strlen(str) * 0.0625f * 4),
			this->camera->position[Y] + 1.0625f, 1.0f}, color, FS_NORMAL_DPI);
	renderFontTo(this, "$$$", (GLfloat[]) {
			this->camera->position[X] - 2.0f,
			this->camera->position[Y] + 1.0625f,
			1.0f}, color, FS_NORMAL_DPI);


//	renderFontTo(this, "#", (GLfloat[]) {
//			getCursorProjectedX(this, cursorX),
//			getCursorProjectedY(this, cursorY), 1.0f}, color, FS_NORMAL_DPI);
}

void onRender(GameInstance *this) {
	static const GLfloat TILE_MOVE_MAT[16] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f};
	static const GLfloat BASE_COLOR[4] = {1.0f, 1.0f, 1.0f, 1.0f};

	glUseProgram(this->shader->shaderId);
	glUniform3fv(this->shader->cameraPosition, 1, this->camera->position);
	glUniform3fv(this->shader->lightPosition, this->lighting->numLights, this->lighting->lightPosition);
	glUniform3fv(this->shader->lightColor, this->lighting->numLights, this->lighting->lightColor);
	glUniform1i(this->shader->numLights, this->lighting->numLights);
	glUniform4fv(this->shader->baseColor, 1, BASE_COLOR);
	glUniformMatrix4fv(this->shader->projMat, 1, GL_FALSE, this->camera->projMat);
	glGetFloatv(GL_MODELVIEW_MATRIX, &this->camera->viewMat);
	glUniformMatrix4fv(this->shader->viewMat, 1, GL_FALSE, this->camera->viewMat);
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, TILE_MOVE_MAT);

	glBindVertexArray(this->tileVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(this->shader->texturePosition, 0);

	ListElement *it;
	for (it = this->map->tiles->first; it != NULL; it = it->next)
		renderTile(this, (Tile *) it->data);

	glBindTexture(GL_TEXTURE_2D, 0);

	for (it = this->map->objects->staticInstances->first; it != NULL; it = it->next)
		renderStaticObject(this, (StaticObjectInstance *) it->data);
	for (it = this->map->objects->dynamicInstances->first; it != NULL; it = it->next)
		renderDynamicObject(this, (DynamicObjectInstance *) it->data);
	for (it = this->map->objects->activeInstances->first; it != NULL; it = it->next)
		renderActiveObject(this, (ActiveObjectInstance *) it->data);

//	if (this->state == INGAME) {
//		renderGUI(this);
//	} else if (this->state == MENU) {
//
//	}

	if (this->map->menu != NULL) {
		for (it = this->map->menu->components->first; it != NULL; it = it->next) {
			Component* comp = (Component *) it->data;
			if (comp->onRender != NULL)
				comp->onRender(comp, this);
		}
	}

    // TODO: Cleanup
    glUseProgram(0);

	updateCamera(this);
}

static unsigned int getTicks(void) {
	#ifdef _WIN32
		return GetTickCount();
	#else
		struct timeval t;
		gettimeofday(&t, NULL);
		return (t.tv_sec * 1000) + (t.tv_usec / 1000);
	#endif /* _WIN32 */
}

void onLogic(GameInstance *this) {
	static unsigned int prevTicks = 0;
	unsigned int ticks;
	float secondsElapsed;
	int i;

	if (prevTicks == 0)
		prevTicks = getTicks();
	ticks = getTicks();
	secondsElapsed = (float) (ticks - prevTicks) / 1000.0f;
	prevTicks = ticks;

	int width, height;
	glfwGetFramebufferSize(this->window, &width, &height);
	double cursorX, cursorY;
	glfwGetCursorPos(this->window, &cursorX, &cursorY);

#ifndef DEBUG_MOVEMENT
	if (this->camera->destinationRotation[Y] > 0) {
		this->camera->rotation[Y] = min(this->camera->rotation[Y] + 0.2,
				this->camera->destinationRotation[Y]);
	} else if (this->camera->destinationRotation[Y] < 0) {
		this->camera->rotation[Y] = max(this->camera->rotation[Y] - 0.2,
				this->camera->destinationRotation[Y]);
	} else if (this->camera->rotation[Y] > 0) {
		this->camera->rotation[Y] -= 0.3;
		if (this->camera->rotation[Y] < 0)
			this->camera->rotation[Y] = 0;
	} else if (this->camera->rotation[Y] < 0) {
		this->camera->rotation[Y] += 0.3;
		if (this->camera->rotation[Y] > 0)
			this->camera->rotation[Y] = 0;
	}
#endif

	ListElement *it;
	i = 0;
	for (it = this->map->lights->first; it != NULL; it = it->next, ++i) {
		Light light = *(Light *)it->data;
		this->lighting->lightColor[i * 3 + 0] = light.color[R];
		this->lighting->lightColor[i * 3 + 1] = light.color[G];
		this->lighting->lightColor[i * 3 + 2] = light.color[B];
		this->lighting->lightPosition[i * 3 + 0] = light.position[X];
		this->lighting->lightPosition[i * 3 + 1] = light.position[Y];
		this->lighting->lightPosition[i * 3 + 2] = light.position[Z];

#ifdef DEBUG
		glPushMatrix();
		glTranslatef(this->lighting->lightPosition[i * 3 + 0],
				this->lighting->lightPosition[i * 3 + 1],
				this->lighting->lightPosition[i * 3 + 2]);
		glColor3fv(this->lighting->lightColor + (i * 3));
//		glutSolidSphere(0.04, 36, 36);
		glPopMatrix();
#endif
	}
	this->lighting->numLights = i;


	if (this->state == INGAME)
		onLogicIngame(this, secondsElapsed);
	else
		onLogicMenu(this, secondsElapsed);

}
