#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stdgame.h"

void loadTileVAO(const GameInstance *this) {
	glGenVertexArrays(1, &this->tileVAO);
	glBindVertexArray(this->tileVAO);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	/** (3 x Position, 2 x Texture coord, 3 x Normal vectors) x 4 */
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

		ERROR("Shader program linking failed: %s", log);
		free(log);

		glDeleteProgram(this->shader->shaderId);
		this->shader->shaderId = 0;
	}

	this->shader->cameraPosition = glGetUniformLocation(this->shader->shaderId, "cameraPosition");
	this->shader->lightPosition = glGetUniformLocation(this->shader->shaderId, "lightPosition");
	this->shader->lightColor = glGetUniformLocation(this->shader->shaderId, "lightColor");
	this->shader->lightInfo = glGetUniformLocation(this->shader->shaderId, "lightInfo");
	this->shader->texturePosition = glGetUniformLocation(this->shader->shaderId, "tex");
	this->shader->numLights = glGetUniformLocation(this->shader->shaderId, "numLights");
	this->shader->baseColor = glGetUniformLocation(this->shader->shaderId, "baseColor");
	this->shader->projMat = glGetUniformLocation(this->shader->shaderId, "projMat");
	this->shader->viewMat = glGetUniformLocation(this->shader->shaderId, "viewMat");
	this->shader->moveMat = glGetUniformLocation(this->shader->shaderId, "moveMat");
	this->shader->modelMat = glGetUniformLocation(this->shader->shaderId, "modelMat");

	loadTileVAO(this);
	loadTexture(&this->blankTextureId, "null.png");
	initReferencePoints(this);

	this->state = MENU;
	this->map = loadMap(this, "assets/maps/main.menu");

	initFont(this);
	DEBUG("Logic", "Ready to start")
	onLogic(this);
	DEBUG("Logic", "First logic done");

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
	glUniform3fv(this->shader->lightInfo, this->lighting->numLights, this->lighting->lightInfo);
	glUniform1i(this->shader->numLights, this->lighting->numLights);
	glUniform4fv(this->shader->baseColor, 1, BASE_COLOR);
	glUniformMatrix4fv(this->shader->projMat, 1, GL_FALSE, this->camera->projMat);
	glGetFloatv(GL_MODELVIEW_MATRIX, &this->camera->viewMat);
	glUniformMatrix4fv(this->shader->viewMat, 1, GL_FALSE, this->camera->viewMat);
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, TILE_MOVE_MAT);

	glBindVertexArray(this->tileVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(this->shader->texturePosition, 0);

	Iterator it;
	foreach (it, this->map->tiles->first)
		renderTile(this, (Tile *) it->data);

	glBindTexture(GL_TEXTURE_2D, 0);

#ifdef DEBUG_LIGHT
	{
		Iterator it;
		int i = 0;
		for (it = this->map->lights->first; it != NULL; it = it->next, ++i) {
			Light light = *(Light *)it->data;

			glPushMatrix();
			glTranslatef(this->lighting->lightPosition[i * 3 + 0],
					this->lighting->lightPosition[i * 3 + 1],
					this->lighting->lightPosition[i * 3 + 2]);
			glColor3fv(this->lighting->lightColor + (i * 3));
			renderFontTo(this, ",", this->lighting->lightPosition + (i * 3), (GLfloat[]) {
				*(this->lighting->lightColor + (i * 3)),
				*(this->lighting->lightColor + (i * 3) + 1),
				*(this->lighting->lightColor + (i * 3) + 2), 1.0 }, FS_LOW_DPI);
			glPopMatrix();
		}
	}
#endif

	foreach (it, this->map->objects->staticInstances->first)
		renderStaticObject(this, (StaticObjectInstance *) it->data);
	foreach (it, this->map->objects->dynamicInstances->first)
		renderDynamicObject(this, (DynamicObjectInstance *) it->data);
	foreach (it, this->map->objects->activeInstances->first)
		renderActiveObject(this, (ActiveObjectInstance *) it->data);

	foreach (it, this->map->menu->components->first) {
		Component* comp = it->data;
		if (comp->onRender != NULL)
			comp->onRender(comp, this);
	}

	if (this->map->menu->useCursor)
		renderActiveObject(this, this->cursor->pointer);

    glUseProgram(0);

	updateCamera(this);
}

GLfloat getDistSquared2D(GLfloat a[3], GLfloat b[3]) {
	return (a[X] - b[X]) * (a[X] - b[X]) + (a[Y] - b[Y]) * (a[Y] - b[Y]);
}

GLboolean isActionPerformed(GameInstance *this, InputActionWrapper* iaw) {
	int i;
	for (i = 0; i < 3; ++i) {
		if (iaw->id[i] != -1) {
			if (iaw->id[i] <= GLFW_MOUSE_BUTTON_LAST) {
				if (glfwGetMouseButton(this->window, iaw->id[i]) == GLFW_PRESS)
					return GL_TRUE;
			} else {
				if (glfwGetKey(this->window, iaw->id[i]) == GLFW_PRESS)
					return GL_TRUE;
			}
		}
	}
	return GL_FALSE;
}

void onLogicIngame(GameInstance *this, GLfloat delta) {
	DynamicObjectInstance *obj = this->map->objects->dynamicInstances->first->data;

	float deltaMoveX = 0;
	if (isActionPerformed(this, &this->options->moveLeft)) {
		deltaMoveX += -delta * 2;
		obj->rotation[Y] = 0;
		if (this->options->cameraMovement)
			this->camera->destinationRotation[Y] = 2;
	}

	if (isActionPerformed(this, &this->options->moveRight)) {
		deltaMoveX += delta * 2;
		obj->rotation[Y] = 180;
		if (this->options->cameraMovement)
			this->camera->destinationRotation[Y] = -2;
	}

	if (this->options->cameraMovement &&
			!isActionPerformed(this, &this->options->moveLeft) &&
			!isActionPerformed(this, &this->options->moveRight)) {
		this->camera->destinationRotation[Y] = 0;
	}

	Iterator it;
	foreach (it, this->map->tiles->first) {
		Tile *tile = it->data;
		if ((tile->type & MOVE_BLOCK_X) != 0 &&
				((int) tile->y == (int) this->player->position[Y] ||
					(int) tile->y == (int) (this->player->position[Y] + 1.0f) ||
					(int) tile->y == (int) (this->player->position[Y] + this->player->height)) &&
				((tile->x + 1 < this->player->position[X]
					&& tile->x + 1 >= this->player->position[X] + deltaMoveX) ||
				(tile->x >= this->player->position[X] + this->player->width
					&& tile->x < this->player->position[X] + deltaMoveX + this->player->width))) {

			deltaMoveX = 0;
			break;
		}
	}
	this->player->position[X] += deltaMoveX;

	if (this->player->jump < 2 && this->player->lastJump + 0.3 < glfwGetTime()
			&& isActionPerformed(this, &this->options->jump)) {
		++this->player->jump;
		this->player->velocity[Y] = 8 + (this->player->jump * 2);
		this->player->lastJump = glfwGetTime();
	}

	float deltaMoveY = this->player->velocity[Y] * 0.00981;
	this->player->velocity[Y] -= 0.5;
	if (this->player->velocity[Y] < -15)
		this->player->velocity[Y] = -15;

	foreach (it, this->map->tiles->first) {
		Tile *tile = it->data;
		if ((tile->type & MOVE_BLOCK_Y) != 0 &&
				((int) tile->x == (int) (this->player->position[X]) ||
				(int) tile->x == (int) (this->player->position[X] + this->player->width))) {

			if (tile->y + 1 > deltaMoveY + this->player->position[Y]
					&& tile->y + 1 <= this->player->position[Y]) {
				this->player->position[Y] = tile->y + 1;
				this->player->velocity[Y] = -0.00001;
				deltaMoveY = 0;
				this->player->jump = 0;
				this->player->lastJump = 0;
			} else if (tile->y <= deltaMoveY + this->player->position[Y] + this->player->height
					&& tile->y > this->player->position[Y] + this->player->height) {
				this->player->velocity[Y] = -0.00001;
				deltaMoveY = 0;
				this->player->jump = 2;
			}
		}
	}

	this->player->position[Y] += deltaMoveY;

#ifndef DEBUG_MOVEMENT
	this->camera->position[X] = this->player->position[X] + 2;
	this->camera->position[Y] = this->player->position[Y] + 1;
	this->camera->position[Z] = 5.2; //4.8
#endif

	// DEBUG
	obj->position[X] = this->player->position[X];
	obj->position[Y] = this->player->position[Y];
	// END DEBUG

}

void onLogicMenu(GameInstance *this, GLfloat delta) {
	this->camera->position[Z] = this->map->spawn[Z];

	updateCursor(this);
	this->camera->rotation[Y] = -this->cursor->pointer->position[X] * 2;
	this->camera->rotation[X] = this->cursor->pointer->position[Y] * 2;

	Iterator it;
	foreach (it, this->map->menu->components->first) {
		Component *comp = it->data;
		if (comp->onCalc != NULL)
			comp->onCalc(comp, this);
	}

}

static GLfloat getDelta(void) {
	static GLfloat lastFrame = 0;
	GLfloat time = glfwGetTime();
	GLfloat delta = time - lastFrame;
	lastFrame = time;
	return delta;
}

void onLogic(GameInstance *this) {
	const GLfloat delta = getDelta();

	updateReferencePoint(this, delta);

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

	int i;
	Iterator it;
	foreach (it, this->map->lights->first) {
		Light *light = it->data;
		if (getDistSquared2D(light->position, this->camera->position) > 100)
			continue;

		this->lighting->lightColor[i * 3 + R] = light->color[R];
		this->lighting->lightColor[i * 3 + G] = light->color[G];
		this->lighting->lightColor[i * 3 + B] = light->color[B];
		this->lighting->lightPosition[i * 3 + X] = light->position[X] + light->reference->position[X];
		this->lighting->lightPosition[i * 3 + Y] = light->position[Y] + light->reference->position[Y];
		this->lighting->lightPosition[i * 3 + Z] = light->position[Z] + light->reference->position[Z];
		this->lighting->lightInfo[i * 3 + 0] = light->specular;
		this->lighting->lightInfo[i * 3 + 1] = light->strength;
		this->lighting->lightInfo[i * 3 + 2] = light->intensity;
		++i;
	}
	this->lighting->numLights = i;

	if (this->state == INGAME)
		onLogicIngame(this, delta);
	else
		onLogicMenu(this, delta);

}
