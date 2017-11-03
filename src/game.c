#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef _WIN32
	#include <windows.h>
	#include <wingdi.h>
	#define GLUT_DISABLE_ATEXIT_HACK
#else
	#include <sys/time.h>
#endif /* _WIN32 */

#include "includes.h"

extern GLuint shaderAttachFromFile(GLuint, GLenum, const char *);

void setPerspective(const GameInstance *this, float fov, float aspect, float near, float far) {
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

	this->map = loadMap(this, "assets/maps/test2.map");
	
	{
		StaticObject *so = loadObject("assets/objects/tendril.sobj");
		listPush(this->map->objects->staticObjects, so);
	}

	{
		StaticObject *so = loadObject("assets/objects/gnome.sobj");
		listPush(this->map->objects->staticObjects, so);
		StaticObjectInstance oi;
		oi.id = 0;
		oi.position[0] = 0.0f;
		oi.position[1] = 0.0f;
		oi.position[2] = 0.0f;
		oi.rotation[0] = 0.0f;
		oi.rotation[1] = 0.0f;
		oi.rotation[2] = 0.0f;
		oi.scale[0] = 1;
		oi.scale[1] = 1;
		oi.scale[2] = 1;
		oi.object = so;
		listPush(this->map->objects->staticInstances, &oi);
	}

	{
		StaticObject *so = loadObject("assets/objects/lock_little.sobj");
		listPush(this->map->objects->staticObjects, so);
		StaticObjectInstance oi;
		oi.id = 1;
		oi.position[0] = 10.75f;
		oi.position[1] = 7.5f;
		oi.position[2] = 0.8f;
		oi.rotation[0] = 0.0f;
		oi.rotation[1] = 90.0f;
		oi.rotation[2] = 0.0f;
		oi.scale[0] = 0.5;
		oi.scale[1] = 0.5;
		oi.scale[2] = 0.5;
		oi.object = so;
		listPush(this->map->objects->staticInstances, &oi);
	}

	{
		StaticObject *so = loadObject("assets/objects/key_little.sobj");
		listPush(this->map->objects->staticObjects, so);
		StaticObjectInstance oi;
		oi.id = 2;
		oi.position[0] = 0.0f;
		oi.position[1] = 0.0f;
		oi.position[2] = 0.0f;
		oi.rotation[0] = 0.0f;
		oi.rotation[1] = 0.0f;
		oi.rotation[2] = 0.0f;
		oi.scale[0] = 1;
		oi.scale[1] = 1;
		oi.scale[2] = 1;
		oi.object = so;
		listPush(this->map->objects->staticInstances, &oi);
	}

	{
		StaticObject *so = loadObject("assets/objects/door.dobj");
		listPush(this->map->objects->staticObjects, so);
		StaticObjectInstance oi;
		oi.id = 3;
		oi.position[0] = 11.0f;
		oi.position[1] = 7.0f;
		oi.position[2] = 0.0f;
		oi.rotation[0] = 0.0f;
		oi.rotation[1] = 180.0f;
		oi.rotation[2] = 0.0f;
		oi.scale[0] = 1;
		oi.scale[1] = 1;
		oi.scale[2] = 1;
		oi.object = so;
		listPush(this->map->objects->staticInstances, &oi);
	}

	{
		StaticObject *so = loadObject("assets/objects/ghost.aobj");
		listPush(this->map->objects->staticObjects, so);
		StaticObjectInstance oi;
		oi.id = 3;
		oi.position[0] = 14.0f;
		oi.position[1] = 7.3f;
		oi.position[2] = 0.0f;
		oi.rotation[0] = 0.0f;
		oi.rotation[1] = 180.0f;
		oi.rotation[2] = 0.0f;
		oi.scale[0] = 1;
		oi.scale[1] = 1;
		oi.scale[2] = 1;
		oi.object = so;
		listPush(this->map->objects->staticInstances, &oi);
	}

	initPlayer(this);
	initFont(this);
	onLogic(this);


	updateCamera(this);
	glGetFloatv(GL_MODELVIEW_MATRIX, &this->camera->viewMat);
}

void updateCamera(GameInstance* this) {
	glLoadIdentity();
	glRotatef(-this->camera->rotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(-this->camera->rotation[1], 0.0f, 1.0f, 0.0f);
	glRotatef(-this->camera->rotation[2], 0.0f, 0.0f, 1.0f);
	glTranslatef(-this->camera->position[0],
			-this->camera->position[1],
			-this->camera->position[2]);
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
		renderTile(this, (Tile *)it->data);

	glBindTexture(GL_TEXTURE_2D, 0);
	for (it = this->map->objects->staticInstances->first; it != NULL; it = it->next)
		renderStaticObject(this, (StaticObjectInstance *)it->data);

	GLfloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	renderFontTo(this, "You have no $", (GLfloat[]) {6.0f, 7.0625f, 1.0f, 1.0f}, color);

	char str[16];
	if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_PRESS)
		++this->score;
	sprintf(str, "%d *", this->score);

	renderFontTo(this, str, (GLfloat[]) {this->camera->position[0] + 2.0f - (strlen(str) * 0.0625f * 4),
			9.0625f, 1.0f, 1.0f}, color);
	renderFontTo(this, "$$$$$", (GLfloat[]) {this->camera->position[0] - 2.0f, 9.0625f, 1.0f, 1.0f}, color);

    // Cleanup
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

	StaticObjectInstance *obj = ((StaticObjectInstance *) this->map->objects->staticInstances->first->data);
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS
			|| glfwGetKey(this->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		this->player->position[0] -= secondsElapsed * 2;
		obj->rotation[1] = 0;
	}

	if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS
			|| glfwGetKey(this->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		this->player->position[0] += secondsElapsed * 2;
		obj->rotation[1] = 180;
	}

	if (this->player->position[1] <= 7) {
		this->player->velocity[1] = 0;
		this->player->position[1] = 7;
		this->player->jumping = GL_FALSE;
	}

	if (!this->player->jumping && (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS
			|| glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)) {
		this->player->jumping = GL_TRUE;
		this->player->velocity[1] = 10;
	}

	if (this->player->jumping) {
		this->player->position[1] += (this->player->velocity[1] * (9.81 / 1000));
		this->player->velocity[1] -= 0.5;
	}

#ifndef DEBUG_MOVEMENT
	this->camera->position[0] = this->player->position[0] + 2;
	this->camera->position[1] = this->player->position[1] + 1;
	this->camera->position[2] = 4.8;
#endif

	// DEBUG
	obj->position[0] = this->player->position[0];
	obj->position[1] = this->player->position[1];
//	obj->rotation[2]d = 270;
	// END DEBUG

	int width, height;
	glfwGetFramebufferSize(this->window, &width, &height);
	double cursorX, cursorY;
	glfwGetCursorPos(this->window, &cursorX, &cursorY);
#ifndef DEBUG_MOVEMENTd
//	this->camdera->rotation[1] = (cursorX - (width / 2)) * -0.01;
#endif

	i = 0;
	ListElement *it;
	for (it = this->map->lights->first; it != NULL; it = it->next, ++i) {
		Light light = *(Light *)it->data;
		this->lighting->lightColor[i * 3 + 0] = light.color[0];
		this->lighting->lightColor[i * 3 + 1] = light.color[1];
		this->lighting->lightColor[i * 3 + 2] = light.color[2];
		this->lighting->lightPosition[i * 3 + 0] = light.x;
		this->lighting->lightPosition[i * 3 + 1] = light.y;
		this->lighting->lightPosition[i * 3 + 2] = 1.4f;

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

}
