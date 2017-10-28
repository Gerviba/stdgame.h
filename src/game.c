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

GameInstance *this;

static void loadProjectionMatrix() {
	float fov = PI / 4.0f;
	float arat = (float) WINDOW_WIDTH / (float) WINDOW_HEIGHT;
	float near = 0.1f;
	float far = 200.0f;
	float f = 1.0f / tanf(fov / 2.0f);

	this->camera->projMat[0] = f / arat;
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
}

void loadTileVAO() {
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

void gameInit(void) {
	printf("Init\n");

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

		fprintf(stderr, "gameInit(): Program linking failed: %s\n", log);
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

	loadProjectionMatrix();

	loadTileVAO();

	loadTexture(&this->blankTextureId, "null.png");

	this->map = loadMap("assets/maps/test.map");
	StaticObject *so = loadStaticObject("assets/objects/tendril.sobj");
	listPush(this->objects->staticObjects, so);
	ObjectInstance oi;
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
	listPush(this->objects->staticInstances, &oi);

	onLogic();

	this->camera->position[0] = this->map->spawn[0];
	this->camera->position[1] = this->map->spawn[1];
	this->camera->position[2] = this->map->spawn[2];
	glLoadIdentity();
	glRotatef(-this->camera->rotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(-this->camera->rotation[1], 0.0f, 1.0f, 0.0f);
	glRotatef(-this->camera->rotation[2], 0.0f, 0.0f, 1.0f);
	glTranslatef(-this->camera->position[0], -this->camera->position[1], -this->camera->position[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, &this->camera->viewMat);

}

void onRender(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int i = 0;
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
		glutSolidSphere(0.04, 36, 36);
		glPopMatrix();
#endif
	}
	this->lighting->numLights = i;

	glUseProgram(this->shader->shaderId);
	glUniform3fv(this->shader->cameraPosition, 1, this->camera->position);
	glUniform3fv(this->shader->lightPosition, this->lighting->numLights, this->lighting->lightPosition);
	glUniform3fv(this->shader->lightColor, this->lighting->numLights, this->lighting->lightColor);
	glUniform1i(this->shader->numLights, this->lighting->numLights);
	glUniform4fv(this->shader->baseColor, 1, (GLfloat[]) {1.0f, 1.0f, 1.0f, 1.0f});
	glUniformMatrix4fv(this->shader->projMat, 1, GL_FALSE, this->camera->projMat);
	glGetFloatv(GL_MODELVIEW_MATRIX, &this->camera->viewMat);
	glUniformMatrix4fv(this->shader->viewMat, 1, GL_FALSE, this->camera->viewMat);
	//Experimental
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, (GLfloat[]) {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f});

	glBindVertexArray(this->tileVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(this->shader->texturePosition, 0);

	for (it = this->map->tiles->first; it != NULL; it = it->next) {
		Tile tile = *(Tile *)it->data;
		glBindTexture(GL_TEXTURE_2D, tile.texture->base->textureId);
		glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				tile.x, tile.y, (tile.type & TTMASK_RENDER_FRONT) == TTMASK_RENDER_FRONT ? 1.0f : 0.0f, 1.0f});
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		if (tile.type != TT_BACKGROUND) {
			if ((tile.type & TTMASK_RENDER_TOP) > 1) {
				glBindTexture(GL_TEXTURE_2D, tile.texture->top->textureId);
				glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
						1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, cosf(PI / 2), -sinf(PI / 2), 0.0f,
						0.0f, sinf(PI / 2), cosf(PI / 2), 0.0f,
						tile.x, tile.y + 1, 1.0f, 1.0f});
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}

			if ((tile.type & TTMASK_RENDER_LEFT) > 1) {
				glBindTexture(GL_TEXTURE_2D, tile.texture->left->textureId);
				glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
						cosf(PI + PI / 2), 0.0f, sinf(PI + PI / 2), 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						-sinf(PI + PI / 2), 0.0f, cosf(PI + PI / 2), 0.0f,
						tile.x + 1, tile.y, 1.0f, 1.0f});
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}

			if ((tile.type & TTMASK_RENDER_BOTTOM) > 1) {
				glBindTexture(GL_TEXTURE_2D, tile.texture->bottom->textureId);
				glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
						1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, cosf(PI + PI / 2), -sinf(PI + PI / 2), 0.0f,
						0.0f, sinf(PI + PI / 2), cosf(PI + PI / 2), 0.0f,
						tile.x, tile.y, 0.0f, 1.0f});
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}

			if ((tile.type & TTMASK_RENDER_RIGHT) > 1) {
				glBindTexture(GL_TEXTURE_2D, tile.texture->right->textureId);
				glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
						cosf(PI / 2), 0.0f, sinf(PI / 2), 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						-sinf(PI / 2), 0.0f, cosf(PI / 2), 0.0f,
						tile.x, tile.y, 0.0f, 1.0f});
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	for (it = this->objects->staticInstances->first; it != NULL; it = it->next) {
		ObjectInstance *sobj = (ObjectInstance *)it->data;
		renderStaticObject(this, sobj);
	}

    // Cleanup

    glUseProgram(0);

//	for (i = 0; i < MAX_NUM_LIGHTS; ++i) {
//		glPushMatrix();
//		glTranslatef(this->lighting->lightPosition[i * 3 + 0], this->lighting->lightPosition[i * 3 + 1], this->lighting->lightPosition[i * 3 + 2]);
//		glColor3fv(this->lighting->lightColor + (i * 3));
//		glutSolidSphere(0.04, 36, 36);
//		glPopMatrix();
//	}

	glLoadIdentity();
	glRotatef(-this->camera->rotation[0], 1.0f, 0.0f, 0.0f);
	glRotatef(-this->camera->rotation[1], 0.0f, 1.0f, 0.0f);
	glRotatef(-this->camera->rotation[2], 0.0f, 0.0f, 1.0f);
	glTranslatef(-this->camera->position[0], -this->camera->position[1], -this->camera->position[2]);

	glutSwapBuffers();
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

void onLogic(void) {
//	printf("logic\n");
	static unsigned int prevTicks = 0;
	unsigned int ticks;
	float secondsElapsed;
	int i;

	if (prevTicks == 0)
		prevTicks = getTicks();
	ticks = getTicks();
	secondsElapsed = (float) (ticks - prevTicks) / 1000.0f;
	prevTicks = ticks;

//	lightRotation += (PI / 4.0f) * secondsElapsed;
//	for (i = 0; i < MAX_NUM_LIGHTS; ++i) {
//		const float radius = 1.75f;
//		float r = (((PI * 2.0f) / (float) MAX_NUM_LIGHTS) * (float) i) + lightRotation;
//
//		lightPosition[i * 3 + 0] = cosf(r) * radius;
//		lightPosition[i * 3 + 1] = cosf(r) * sinf(r);
//		lightPosition[i * 3 + 2] = sinf(r) * radius;
//	}

	glutPostRedisplay();
}
