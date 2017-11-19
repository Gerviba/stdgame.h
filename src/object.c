#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "stdgame.h"

StaticObject *loadStaticObject(char path[]) {
	StaticObject *obj = new(StaticObject);
	obj->parts = newList(StaticObjectPart);
	obj->colors = newList(PartColor);
	obj->position[X] = 0;
	obj->position[Y] = 0;
	obj->position[Z] = 0;
	obj->rotation[X] = 0;
	obj->rotation[Y] = 0;
	obj->rotation[Z] = 0;
	obj->scale[X] = 1.0f / 16;
	obj->scale[Y] = 1.0f / 16;
	obj->scale[Z] = 1.0f / 16;

	FILE *file;
	char buff[255];
	printf("[Object] Loading object: %s\n", path);

	file = fopen(path, "r");
	if (!file) {
		printf("[Object] Failed to load. File not accessable.\n");
		return NULL;
	}

	while (fgets(buff, 255, file)) {
		switch (buff[0]) {
			case '$': { // Basic info
				char type[32];
				sscanf(buff, "$ %s", type);
				if (equals(type, "POSITION")) {
					sscanf(buff, "$ %*s %f %f %f", &obj->position[X], &obj->position[Y], &obj->position[Z]);
				} else if (equals(type, "ROTATION")) {
					sscanf(buff, "$ %*s %f %f %f", &obj->rotation[X], &obj->rotation[Y], &obj->rotation[Z]);
				} else if (equals(type, "SCALE")) {
					sscanf(buff, "$ %*s %f %f %f", &obj->scale[X], &obj->scale[Y], &obj->scale[Z]);
					obj->scale[X] *= (1.0f / 16);
					obj->scale[Y] *= (1.0f / 16);
					obj->scale[Z] *= (1.0f / 16);
				}
				break;
			}
			case 'C': { // Color
				PartColor color;
				unsigned int r, g, b;
				sscanf(buff, "C %d %02x%02x%02x %f", &color.id, &r, &g, &b, &color.color[A]);
				color.color[R] = (float) r / 255;
				color.color[G] = (float) g / 255;
				color.color[B] = (float) b / 255;

				listPush(obj->colors, &color);
				break;
			}
			case 'B': { // Block
				StaticObjectPart part;
				int colorId;

				sscanf(buff, "B %f %f %f %d %d", &part.position[X], &part.position[Y],
						&part.position[Z], &part.type, &colorId);

				ListItem *it;
				for (it = obj->colors->first; it != NULL; it = it->next) {
					if (((TextureBlock *) it->data)->id == colorId) {
						part.color = ((PartColor *) it->data)->color;
						break;
					}
				}

				listPush(obj->parts, &part);
				break;
			}
		}
	}

	fclose(file);
	return obj;
}

DynamicObject *loadDynamicObject(char path[]) {
	DynamicObject *obj = new(DynamicObject);
	obj->parts = newList(StaticObjectPart);
	obj->colors = newList(PartColor);
	obj->position[X] = 0;
	obj->position[Y] = 0;
	obj->position[Z] = 0;
	obj->rotation[X] = 0;
	obj->rotation[Y] = 0;
	obj->rotation[Z] = 0;
	obj->scale[X] = 1.0f / 16;
	obj->scale[Y] = 1.0f / 16;
	obj->scale[Z] = 1.0f / 16;

	FILE *file;
	char buff[255];
	printf("[Object] Loading object: %s\n", path);

	file = fopen(path, "r");
	if (!file) {
		printf("[Object] Failed to load. File not accessable.\n");
		return NULL;
	}

	while (fgets(buff, 255, file)) {
		switch (buff[0]) {
			case '$': { // Basic info
				char type[32];
				sscanf(buff, "$ %s", type);
				if (equals(type, "POSITION")) {
					sscanf(buff, "$ %*s %f %f %f", &obj->position[X], &obj->position[Y], &obj->position[Z]);
				} else if (equals(type, "ROTATION")) {
					sscanf(buff, "$ %*s %f %f %f", &obj->rotation[X], &obj->rotation[Y], &obj->rotation[Z]);
				} else if (equals(type, "SCALE")) {
					sscanf(buff, "$ %*s %f %f %f", &obj->scale[X], &obj->scale[Y], &obj->scale[Z]);
					obj->scale[X] *= (1.0f / 16);
					obj->scale[Y] *= (1.0f / 16);
					obj->scale[Z] *= (1.0f / 16);
				}
				break;
			}
			case 'C': { // Color
				PartColor color;
				unsigned int r, g, b;
				sscanf(buff, "C %d %02x%02x%02x %f", &color.id, &r, &g, &b, &color.color[A]);
				color.color[R] = (float) r / 255;
				color.color[G] = (float) g / 255;
				color.color[B] = (float) b / 255;

				listPush(obj->colors, &color);
				break;
			}
			case 'B': { // Block
				StaticObjectPart part;
				int colorId;

				sscanf(buff, "B %f %f %f %d %d", &part.position[X], &part.position[Y],
						&part.position[Z], &part.type, &colorId);

				ListItem *it;
				for (it = obj->colors->first; it != NULL; it = it->next) {
					if (((TextureBlock *) it->data)->id == colorId) {
						part.color = ((PartColor *) it->data)->color;
						break;
					}
				}

				listPush(obj->parts, &part);
				break;
			}
		}
	}

	fclose(file);
	return obj;
}

ActiveObject *loadActiveObject(char path[]) {
	ActiveObject *aobj = new(ActiveObject);

	GLfloat position[3] = {0.0f, 0.0f, 0.0f};
	GLfloat rotation[3] = {0.0f, 0.0f, 0.0f};
	GLfloat scale[3] = {0.0f, 0.0f, 0.0f};
	LinkedList /*ColorPart*/ *colors = newList(PartColor);

	FILE *file;
	char buff[255];
	printf("[Object] Loading object: %s\n", path);

	file = fopen(path, "r");
	if (!file) {
		printf("[Object] Failed to load. File not accessable.\n");
		return NULL;
	}

	while (fgets(buff, 255, file)) {
		switch (buff[0]) {
			case '$': { // Basic info
				char type[32];
				sscanf(buff, "$ %s", type);
				if (equals(type, "POSITION")) {
					sscanf(buff, "$ %*s %f %f %f", &position[X], &position[Y], &position[Z]);
				} else if (equals(type, "ROTATION")) {
					sscanf(buff, "$ %*s %f %f %f", &rotation[X], &rotation[Y], &rotation[Z]);
				} else if (equals(type, "SCALE")) {
					sscanf(buff, "$ %*s %f %f %f", &scale[X], &scale[Y], &scale[Z]);
					scale[X] *= (1.0f / 16);
					scale[Y] *= (1.0f / 16);
					scale[Z] *= (1.0f / 16);
				} else if (equals(type, "SIZE")) {
					sscanf(buff, "$ SIZE %d", &aobj->size);
					aobj->parts = malloc(sizeof(DynamicObject) * aobj->size);

					int i;
					for (i = 0; i < aobj->size; ++i) {
						aobj->parts[i].parts = newList(StaticObjectPart);
						aobj->parts[i].colors = colors;
						aobj->parts[i].position[X] = position[X];
						aobj->parts[i].position[Y] = position[Y];
						aobj->parts[i].position[Z] = position[Z];
						aobj->parts[i].rotation[X] = rotation[X];
						aobj->parts[i].rotation[Y] = rotation[Y];
						aobj->parts[i].rotation[Z] = rotation[Z];
						aobj->parts[i].scale[X] = scale[X];
						aobj->parts[i].scale[Y] = scale[Y];
						aobj->parts[i].scale[Z] = scale[Z];
					}
				}
				break;
			}
			case 'C': { // Color
				PartColor color;
				unsigned int r, g, b;
				sscanf(buff, "C %d %02x%02x%02x %f", &color.id, &r, &g, &b, &color.color[A]);
				color.color[R] = (float) r / 255;
				color.color[G] = (float) g / 255;
				color.color[B] = (float) b / 255;

				listPush(colors, &color);
				break;
			}
			case 'B': { // Block
				StaticObjectPart part;
				int colorId, state;

				sscanf(buff, "B %d %f %f %f %d %d", &state, &part.position[X], &part.position[Y],
						&part.position[Z], &part.type, &colorId);

				ListItem *it;
				for (it = colors->first; it != NULL; it = it->next) {
					if (((PartColor *) it->data)->id == colorId) {
						part.color = ((PartColor *) it->data)->color;
						break;
					}
				}

				listPush(aobj->parts[state].parts, &part);
				break;
			}
		}
	}

	fclose(file);
	return aobj;
}

void renderStaticObject(GameInstance *this, StaticObjectInstance *instance) {
	StaticObject *obj = instance->object;
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, instance->moveMat);

	glBindTexture(GL_TEXTURE_2D, this->blankTextureId);
	ListItem *it;
	for (it = obj->parts->first; it != NULL; it = it->next) {
		StaticObjectPart part = *(StaticObjectPart *) it->data;
		glUniform4fv(this->shader->baseColor, 1, part.color);

		if ((part.type & PTMASK_RENDER_UP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, -1, 0.0f,
					0.0f, 1, 0.0f, 0.0f,
					part.position[X], part.position[Y] + 1, part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_DOWN) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, cosf(PI + PI / 2), -sinf(PI + PI / 2), 0.0f,
					0.0f, sinf(PI + PI / 2), cosf(PI + PI / 2), 0.0f,
					part.position[X], part.position[Y], part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_TOP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI), 0.0f, sinf(PI), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI), 0.0f, cosf(PI), 0.0f,
					part.position[X] + 1, part.position[Y], part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_LEFT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI / 2), 0.0f, sinf(PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI / 2), 0.0f, cosf(PI / 2), 0.0f,
					part.position[X], part.position[Y], part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_BOTTOM) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					part.position[X], part.position[Y], part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_RIGHT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI + PI / 2), 0.0f, sinf(PI + PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI + PI / 2), 0.0f, cosf(PI + PI / 2), 0.0f,
					part.position[X] + 1, part.position[Y], part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

	}
}

void renderDynamicObject(GameInstance *this, DynamicObjectInstance *instance) {
	DynamicObject *obj = instance->object;
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(obj->position[X] + instance->position[X],
			obj->position[Y] + instance->position[Y],
			obj->position[Z] + instance->position[Z]);
	glScalef(obj->scale[X] * instance->scale[X],
			obj->scale[Y] * instance->scale[Y],
			obj->scale[Z] * instance->scale[Z]);
	glRotatef(-(obj->rotation[X] + instance->rotation[X]), 1.0f, 0.0f, 0.0f);
	glRotatef(-(obj->rotation[Y] + instance->rotation[Y]), 0.0f, 1.0f, 0.0f);
	glRotatef(-(obj->rotation[Z] + instance->rotation[Z]), 0.0f, 0.0f, 1.0f);

	glGetFloatv(GL_MODELVIEW_MATRIX, &instance->moveMat);
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, instance->moveMat);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, this->blankTextureId);
	ListItem *it;
	for (it = obj->parts->first; it != NULL; it = it->next) {
		StaticObjectPart part = *(StaticObjectPart *) it->data;
		glUniform4fv(this->shader->baseColor, 1, part.color);

		if ((part.type & PTMASK_RENDER_UP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, -1, 0.0f,
					0.0f, 1, 0.0f, 0.0f,
					part.position[X], part.position[Y] + 1, part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_DOWN) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, cosf(PI + PI / 2), -sinf(PI + PI / 2), 0.0f,
					0.0f, sinf(PI + PI / 2), cosf(PI + PI / 2), 0.0f,
					part.position[X], part.position[Y], part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_TOP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI), 0.0f, sinf(PI), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI), 0.0f, cosf(PI), 0.0f,
					part.position[X] + 1, part.position[Y], part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_LEFT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI / 2), 0.0f, sinf(PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI / 2), 0.0f, cosf(PI / 2), 0.0f,
					part.position[X], part.position[Y], part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_BOTTOM) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					part.position[X], part.position[Y], part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_RIGHT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI + PI / 2), 0.0f, sinf(PI + PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI + PI / 2), 0.0f, cosf(PI + PI / 2), 0.0f,
					part.position[X] + 1, part.position[Y], part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

	}
}

void renderActiveObject(GameInstance *this, ActiveObjectInstance *instance) {
	DynamicObject *obj = &instance->object->parts[instance->activePart];
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(obj->position[X] + instance->position[X],
			obj->position[Y] + instance->position[Y],
			obj->position[Z] + instance->position[Z]);
	glScalef(obj->scale[X] * instance->scale[X],
			obj->scale[Y] * instance->scale[Y],
			obj->scale[Z] * instance->scale[Z]);
	glRotatef(-(obj->rotation[X] + instance->rotation[X]), 1.0f, 0.0f, 0.0f);
	glRotatef(-(obj->rotation[Y] + instance->rotation[Y]), 0.0f, 1.0f, 0.0f);
	glRotatef(-(obj->rotation[Z] + instance->rotation[Z]), 0.0f, 0.0f, 1.0f);

	glGetFloatv(GL_MODELVIEW_MATRIX, &instance->moveMat);
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, instance->moveMat);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, this->blankTextureId);
	ListItem *it;
	for (it = obj->parts->first; it != NULL; it = it->next) {
		StaticObjectPart part = *(StaticObjectPart *) it->data; // TODO: . to ->
		glUniform4fv(this->shader->baseColor, 1, part.color);

		if ((part.type & PTMASK_RENDER_UP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, -1, 0.0f,
					0.0f, 1, 0.0f, 0.0f,
					part.position[X], part.position[Y] + 1, part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_DOWN) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, cosf(PI + PI / 2), -sinf(PI + PI / 2), 0.0f,
					0.0f, sinf(PI + PI / 2), cosf(PI + PI / 2), 0.0f,
					part.position[X], part.position[Y], part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_TOP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI), 0.0f, sinf(PI), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI), 0.0f, cosf(PI), 0.0f,
					part.position[X] + 1, part.position[Y], part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_LEFT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI / 2), 0.0f, sinf(PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI / 2), 0.0f, cosf(PI / 2), 0.0f,
					part.position[X], part.position[Y], part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_BOTTOM) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					part.position[X], part.position[Y], part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_RIGHT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI + PI / 2), 0.0f, sinf(PI + PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI + PI / 2), 0.0f, cosf(PI + PI / 2), 0.0f,
					part.position[X] + 1, part.position[Y], part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

	}
}

void renderTile(GameInstance *this, Tile *tile) {
	glBindTexture(GL_TEXTURE_2D, tile->texture->base->textureId);
	glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			tile->x, tile->y, (tile->type & TTMASK_RENDER_FRONT) == TTMASK_RENDER_FRONT ? 1.0f : 0.0f, 1.0f});
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	if (tile->type != TT_BACKGROUND) {
		if ((tile->type & TTMASK_RENDER_TOP) > 1) {
			glBindTexture(GL_TEXTURE_2D, tile->texture->top->textureId);
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, cosf(PI / 2), -sinf(PI / 2), 0.0f,
					0.0f, sinf(PI / 2), cosf(PI / 2), 0.0f,
					tile->x, tile->y + 1, 1.0f, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((tile->type & TTMASK_RENDER_LEFT) > 1) {
			glBindTexture(GL_TEXTURE_2D, tile->texture->left->textureId);
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI + PI / 2), 0.0f, sinf(PI + PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI + PI / 2), 0.0f, cosf(PI + PI / 2), 0.0f,
					tile->x + 1, tile->y, 1.0f, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((tile->type & TTMASK_RENDER_BOTTOM) > 1) {
			glBindTexture(GL_TEXTURE_2D, tile->texture->bottom->textureId);
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, cosf(PI + PI / 2), -sinf(PI + PI / 2), 0.0f,
					0.0f, sinf(PI + PI / 2), cosf(PI + PI / 2), 0.0f,
					tile->x, tile->y, 0.0f, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((tile->type & TTMASK_RENDER_RIGHT) > 1) {
			glBindTexture(GL_TEXTURE_2D, tile->texture->right->textureId);
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI / 2), 0.0f, sinf(PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI / 2), 0.0f, cosf(PI / 2), 0.0f,
					tile->x, tile->y, 0.0f, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}
	}
}

void initStraticInstance(GameInstance *this, StaticObjectInstance *instance) {
	StaticObject *obj = instance->object;
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(obj->position[X] + instance->position[X],
			obj->position[Y] + instance->position[Y],
			obj->position[Z] + instance->position[Z]);
	glScalef(obj->scale[X] * instance->scale[X],
			obj->scale[Y] * instance->scale[Y],
			obj->scale[Z] * instance->scale[Z]);
	glRotatef(-(obj->rotation[X] + instance->rotation[X]), 1.0f, 0.0f, 0.0f);
	glRotatef(-(obj->rotation[Y] + instance->rotation[Y]), 0.0f, 1.0f, 0.0f);
	glRotatef(-(obj->rotation[Z] + instance->rotation[Z]), 0.0f, 0.0f, 1.0f);

	glGetFloatv(GL_MODELVIEW_MATRIX, &instance->moveMat);
	glPopMatrix();
}
