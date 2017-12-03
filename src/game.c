/**
 * @file game.c
 * @author Gerviba (Szabo Gergely)
 * @brief Game initialisation and game loop definition (header)
 *
 * @par Header:
 * 		game.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
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
	glGetFloatv(GL_MODELVIEW_MATRIX, this->camera->viewMat);
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

void debugLight(GameInstance *this) {
	Iterator it;
	int i = 0;
	for (it = this->map->lights->first; it != NULL; it = it->next, ++i) {
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
	updateCamera(this);
	glGetFloatv(GL_MODELVIEW_MATRIX, &this->camera->viewMat);
	glUniformMatrix4fv(this->shader->viewMat, 1, GL_FALSE, this->camera->viewMat);
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, TILE_MOVE_MAT);

	glBindVertexArray(this->tileVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(this->shader->texturePosition, 0);

	Iterator it;
	foreach (it, this->map->tiles->first)
		renderTile(this, it->data);

	glBindTexture(GL_TEXTURE_2D, 0);

#ifdef DEBUG_LIGHT
	debugLight(this);
#endif

	foreach (it, this->map->objects->staticInstances->first)
		renderStaticObject(this, it->data);
	foreach (it, this->map->objects->dynamicInstances->first)
		renderDynamicObject(this, it->data);
	foreach (it, this->map->objects->activeInstances->first)
		renderActiveObject(this, it->data);

	foreach (it, this->map->menu->components->first) {
		Component* comp = it->data;
		if (comp->onRender != NULL)
			comp->onRender(comp, this);
	}

	if (this->map->menu->useCursor)
		renderActiveObject(this, this->cursor->pointer);

	foreach (it, this->map->messages->first) {
		Message *m = it->data;
		renderFontTo(this, m->message, m->position.xyz, m->color.rgba, m->size);
	}

    glUseProgram(0);

}

GLfloat getDistSquared2D(GLfloat a[3], GLfloat b[3]) {
	return (a[X] - b[X]) * (a[X] - b[X]) + (a[Y] - b[Y]) * (a[Y] - b[Y]);
}

GLfloat getDistSquared2DDelta(GLfloat a[3], GLfloat deltaA[3], GLfloat b[3]) {
	return (a[X] + deltaA[X] - b[X]) * (a[X] + deltaA[X] - b[X])
			+ (a[Y] + deltaA[Y] - b[Y]) * (a[Y] + deltaA[Y] - b[Y]);
}

GLfloat getDistSquaredXY(GLfloat x, GLfloat y, GLfloat b[3]) {
	return (x - b[X]) * (x - b[X]) + (y - b[Y]) * (y - b[Y]);
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

GLboolean isPlayerInRegion(GameInstance *this, Region *region) {
	return this->player->position[X] >= region->xMin && this->player->position[Y] >= region->yMin
			&& this->player->position[X] <= region->xMax && this->player->position[Y] <= region->yMax;
}

void activateAction(GameInstance *this, GLint id) {
	Iterator it;
	foreach (it, this->map->actions->first) {
		Action *action = it->data;
		if (action->id != id)
			continue;

		if (action->type == ACTION_TELEPORT) {
			float *pos = action->value->value;
			if (pos[X] != ACTION_VALUE_DONT_CARE)
				this->player->position[X] = pos[X];
			if (pos[Y] != ACTION_VALUE_DONT_CARE)
				this->player->position[Y] = pos[Y];
		} else if (action->type == ACTION_DAMAGE) {
			this->map->healt -= *((int *) action->value->value);
		} else if (action->type == ACTION_ADD_SCORE) {
			this->map->score += *((int *) action->value->value);
		} else if (action->type == ACTION_SET_DOBJ) {
			float *data = action->value->value;
			Iterator dobjIt;
			foreach (dobjIt, this->map->objects->dynamicInstances->first) {
				DynamicObjectInstance *dobj = dobjIt->data;
				if ((int) data[0] == dobj->id) {
					if (data[1] != ACTION_VALUE_DONT_CARE)
						dobj->position[X] = data[1];
					if (data[2] != ACTION_VALUE_DONT_CARE)
						dobj->position[Y] = data[2];
					if (data[3] != ACTION_VALUE_DONT_CARE)
						dobj->position[Z] = data[3];

					if (data[4] != ACTION_VALUE_DONT_CARE)
						dobj->rotation[X] = data[4];
					if (data[5] != ACTION_VALUE_DONT_CARE)
						dobj->rotation[Y] = data[5];
					if (data[6] != ACTION_VALUE_DONT_CARE)
						dobj->rotation[Z] = data[6];

					if (data[7] != ACTION_VALUE_DONT_CARE)
						dobj->scale[X] = data[7];
					if (data[8] != ACTION_VALUE_DONT_CARE)
						dobj->scale[Y] = data[8];
					if (data[9] != ACTION_VALUE_DONT_CARE)
						dobj->scale[Z] = data[9];

					if (data[10] != ACTION_VALUE_DONT_CARE)
						dobj->visible = data[10] != 0;

					if (data[11] != ACTION_VALUE_DONT_CARE) {
						Iterator refIt;
						foreach (refIt, this->referencePoints->first) {
							if (((ReferencePoint *) refIt->data)->id == (GLint) data[11]) {
								dobj->reference = refIt->data;
								break;
							}
						}
					}
				}
			}
		} else if (action->type == ACTION_SET_AOBJ) {
			float *data = action->value->value;
			Iterator aobjIt;
			foreach (aobjIt, this->map->objects->dynamicInstances->first) {
				ActiveObjectInstance *aobj = aobjIt->data;
				if ((int) data[0] == aobj->id) {
					if (data[1] != ACTION_VALUE_DONT_CARE)
						aobj->position[X] = data[1];
					if (data[2] != ACTION_VALUE_DONT_CARE)
						aobj->position[Y] = data[2];
					if (data[3] != ACTION_VALUE_DONT_CARE)
						aobj->position[Z] = data[3];

					if (data[4] != ACTION_VALUE_DONT_CARE)
						aobj->rotation[X] = data[4];
					if (data[5] != ACTION_VALUE_DONT_CARE)
						aobj->rotation[Y] = data[5];
					if (data[6] != ACTION_VALUE_DONT_CARE)
						aobj->rotation[Z] = data[6];

					if (data[7] != ACTION_VALUE_DONT_CARE)
						aobj->scale[X] = data[7];
					if (data[8] != ACTION_VALUE_DONT_CARE)
						aobj->scale[Y] = data[8];
					if (data[9] != ACTION_VALUE_DONT_CARE)
						aobj->scale[Z] = data[9];

					if (data[10] != ACTION_VALUE_DONT_CARE)
						aobj->visible = data[10] != 0;
				}
			}
		} else if (action->type == ACTION_SET_ITEM) {
			this->player->item = *((int *) action->value->value);
		} else if (action->type == ACTION_SET_LIGHT) {
			float *data = action->value->value;
			Iterator lightIt;
			foreach (lightIt, this->map->lights->first) {
				Light *light = lightIt->data;
				if ((int) data[0] == light->id) {
					if (data[1] != ACTION_VALUE_DONT_CARE)
						light->position[X] = data[1];
					if (data[2] != ACTION_VALUE_DONT_CARE)
						light->position[Y] = data[2];
					if (data[3] != ACTION_VALUE_DONT_CARE)
						light->position[Z] = data[3];


					if (data[4] != ACTION_VALUE_DONT_CARE)
						light->strength = data[4];
					if (data[5] != ACTION_VALUE_DONT_CARE)
						light->color[R] = data[5];
					if (data[6] != ACTION_VALUE_DONT_CARE)
						light->color[G] = data[6];
					if (data[7] != ACTION_VALUE_DONT_CARE)
						light->color[B] = data[7];

					if (data[8] != ACTION_VALUE_DONT_CARE)
						light->specular = data[8];
					if (data[9] != ACTION_VALUE_DONT_CARE)
						light->intensity = data[9];

					if (data[10] != ACTION_VALUE_DONT_CARE)
						light->visible = data[10] != 0;

					if (data[11] != ACTION_VALUE_DONT_CARE) {
						Iterator refIt;
						foreach (refIt, this->referencePoints->first) {
							if (((ReferencePoint *) refIt->data)->id == (GLint) data[11]) {
								light->reference = refIt->data;
								break;
							}
						}
					}
				}
			}
		} else if (action->type == ACTION_OBJECT_PSX) {
			float *data = action->value->value;
			Iterator psxIt;
			foreach (psxIt, this->map->physics->first) {
				PhysicsArea *psx = psxIt->data;
				if (psx->id == (int) data[0]) {
					if (data[1] != ACTION_VALUE_DONT_CARE)
						psx->x = data[1];
					if (data[2] != ACTION_VALUE_DONT_CARE)
						psx->y = data[2];
					if (data[3] != ACTION_VALUE_DONT_CARE)
						psx->enabled = data[3] != 0;
				}
			}
		}
	}
}

void onLogicIngame(GameInstance *this, GLfloat delta) {
	static const int PLAYER_ANIMATION[4] = {1, 0, 2, 0};
	static float PLAYER_ANIMATION_TIMING = 0;

	ActiveObjectInstance *playerObj = this->map->objects->activeInstances->first->data;

	float deltaMoveX = 0;
	if (isActionPerformed(this, &this->options->moveLeft)) {
		deltaMoveX += -delta * PLAYER_SPEED;
		this->player->leftSide = GL_TRUE;
		if (this->options->cameraMovement)
			this->camera->destinationRotation[Y] = 2;
	}

	if (isActionPerformed(this, &this->options->moveRight)) {
		deltaMoveX += delta * PLAYER_SPEED;
		this->player->leftSide = GL_FALSE;
		if (this->options->cameraMovement)
			this->camera->destinationRotation[Y] = -2;
	}

	playerObj->rotation[Y] = this->player->leftSide ? 0 : 180;

	if (isActionPerformed(this, &this->options->sneek)) {
		playerObj->activePart = 3;
		this->player->sneek = GL_TRUE;
	} else if (isActionPerformed(this, &this->options->moveLeft)
			|| isActionPerformed(this, &this->options->moveRight)) {

		PLAYER_ANIMATION_TIMING += delta * 5;
		if (PLAYER_ANIMATION_TIMING >= 4)
			PLAYER_ANIMATION_TIMING = 0;
		playerObj->activePart = PLAYER_ANIMATION[((int) PLAYER_ANIMATION_TIMING) % 4];
		this->player->sneek = GL_FALSE;
	} else {
		playerObj->activePart = 0;
		this->player->sneek = GL_FALSE;
	}

	if (this->options->cameraMovement &&
			!isActionPerformed(this, &this->options->moveLeft) &&
			!isActionPerformed(this, &this->options->moveRight)) {
		this->camera->destinationRotation[Y] = 0;
	}

	Iterator it;
	if (deltaMoveX != 0) {
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
	}
	if (deltaMoveX != 0) {
		foreach (it, this->map->physics->first) {
			PhysicsArea *pa = it->data;
			if (pa->enabled &&
					((int) pa->y == (int) this->player->position[Y] ||
						(int) pa->y == (int) (this->player->position[Y] + 1.0f) ||
						(int) pa->y == (int) (this->player->position[Y] + this->player->height)) &&
					((pa->x + 1 < this->player->position[X]
						&& pa->x + 1 >= this->player->position[X] + deltaMoveX) ||
					(pa->x >= this->player->position[X] + this->player->width
						&& pa->x < this->player->position[X] + deltaMoveX + this->player->width))) {

				deltaMoveX = 0;
				break;
			}
		}
	}
	this->player->position[X] += deltaMoveX;

	if (this->player->jump < 2 && this->player->lastJump + 0.3 < glfwGetTime()
			&& isActionPerformed(this, &this->options->jump)
			&& !isActionPerformed(this, &this->options->sneek)) {
		++this->player->jump;
		this->player->velocity[Y] = 8 + (this->player->jump * 2);
		this->player->lastJump = glfwGetTime();
		playerObj->activePart = 0;
	}

	float deltaMoveY = this->player->velocity[Y] * PLAYER_JUMP * delta;
	this->player->velocity[Y] -= 0.5;
	if (this->player->velocity[Y] < -15)
		this->player->velocity[Y] = -15;

	if (deltaMoveY != 0) {
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
	}
	if (deltaMoveY != 0) {
		foreach (it, this->map->physics->first) {
			PhysicsArea *pa = it->data;
			if (pa->enabled &&
					((int) pa->x == (int) (this->player->position[X]) ||
					(int) pa->x == (int) (this->player->position[X] + this->player->width))) {

				if (pa->y + 1 > deltaMoveY + this->player->position[Y]
						&& pa->y + 1 <= this->player->position[Y]) {
					this->player->position[Y] = pa->y + 1;
					this->player->velocity[Y] = -0.00001;
					deltaMoveY = 0;
					this->player->jump = 0;
					this->player->lastJump = 0;
				} else if (pa->y <= deltaMoveY + this->player->position[Y] + this->player->height
						&& pa->y > this->player->position[Y] + this->player->height) {
					this->player->velocity[Y] = -0.00001;
					deltaMoveY = 0;
					this->player->jump = 2;
				}
			}
		}
	}

	this->player->position[Y] += deltaMoveY;

#ifndef DEBUG_MOVEMENT
	if (this->map->allowMovement) {
		this->camera->position[X] = this->player->position[X] + 2;
		this->camera->position[Y] = this->player->position[Y] + 1;
		this->camera->position[Z] = CAMERA_DISTANCE;
	} else {
		this->camera->position[X] = this->player->position[X];
		this->camera->position[Y] = this->player->position[Y];
		this->camera->position[Z] = CAMERA_DISTANCE;
	}
#endif

	setPositionArray(playerObj->position, this->player->position);

	foreach (it, this->map->regions->first) {
		Region *region = it->data;
		if (region->maxUse != 0 && isPlayerInRegion(this, region) && (region->itemReq == -1
				|| (this->player->item == region->itemReq && isActionPerformed(this, &this->options->use)))) {

			if (region->notSneek == GL_FALSE || (region->notSneek == GL_TRUE && !isActionPerformed(this, &this->options->sneek))) {
				activateAction(this, region->actionId);
				if (region->maxUse > 0)
					--region->maxUse;
			}
		}
	}

	foreach (it, this->map->menu->components->first) {
		Component *comp = it->data;
		if (comp->id == HEALT_COMPONENT_ID) {
			char str[6] = "\0";
			int i;
			for (i = 0; i < this->map->healt; ++i)
				strcat(str, "$");
			strcpy(comp->text->text, str);
		} else if (comp->id == SCORE_COMPONENT_ID) {
			char str[14] = "\0";
			sprintf(str, "%d *", this->map->score);
			strcpy(comp->text->text, str);
		}
	}

	foreach (it, this->referencePoints->first) {
		ReferencePoint *rp = it->data;
		if (rp->id == 2) {
			rp->position[X] = this->player->leftSide ?
					(this->player->position[X] - 0.1) : (this->player->position[X] + 0.25);
			rp->position[Y] = this->player->position[Y] - 0.2;
			rp->position[Z] = this->player->leftSide ?
					(this->player->position[Z] + 0.8) : (this->player->position[Z] + 0.4);
			rp->rotation[Y] = this->player->leftSide ? 180 : 0;
		} else if (rp->id == 4) {
			rp->position[X] = this->player->leftSide ?
					(this->player->position[X] + 0.2) : (this->player->position[X] + 0.25);
			rp->position[Y] = this->player->position[Y] + 1.3;
			rp->position[Z] = this->player->leftSide ?
					(this->player->position[Z] + 0.7) : (this->player->position[Z] + 0.3);
			rp->rotation[Y] = this->player->leftSide ? 180 : 0;
		} else if (rp->id == 5) {
			rp->position[X] = this->player->leftSide ?
					(this->player->position[X]) : (this->player->position[X] + 0.5);
			rp->position[Y] = this->player->position[Y] + 0.6;
			rp->position[Z] = this->player->leftSide ?
					(this->player->position[Z] - 0.4) : (this->player->position[Z] + 0.4);
			rp->rotation[Y] = this->player->leftSide ? 315 : 135;
			rp->rotation[X] = this->player->leftSide ? 180 : 0;
		}
	}
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

	if (this->state == INGAME)
		onLogicIngame(this, delta);
	else
		onLogicMenu(this, delta);

	int i = 0;
	Iterator it;
	foreach (it, this->map->lights->first) {
		Light *light = it->data;

		if (!light->visible)
			continue;
		if (getDistSquared2DDelta(light->position, light->reference->position, this->camera->position) > 100)
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

}
