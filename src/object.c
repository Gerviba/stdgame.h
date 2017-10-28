#include <stdio.h>
#include "includes.h"

StaticObject *loadStaticObject(char path[]) {
	StaticObject *obj = malloc(sizeof(StaticObject));
	obj->parts = newLinkedListPointer(sizeof(StaticObjectPart));
	obj->colors = newLinkedListPointer(sizeof(PartColor));
	obj->position[0] = 0;
	obj->position[1] = 0;
	obj->position[2] = 0;
	obj->rotation[0] = 0;
	obj->rotation[1] = 0;
	obj->rotation[2] = 0;
	obj->scale[0] = 1.0f / 16;
	obj->scale[1] = 1.0f / 16;
	obj->scale[2] = 1.0f / 16;

	FILE *file;
	char buff[255];
	printf("[Object] Loading static object: %s\n", path);

	file = fopen(path, "r");
	while (fgets(buff, 255, file)) {
		switch (buff[0]) {
			case '$': { // Basic info
				char type[32];
				sscanf(buff, "$ %s", type);
				if (strcmp(type, "POSITION") == 0) {
					sscanf(buff, "$ %*s %f %f %f", &obj->position[0], &obj->position[1], &obj->position[2]);
				} else if (strcmp(type, "ROTATION") == 0) {
					sscanf(buff, "$ %*s %f %f %f", &obj->rotation[0], &obj->rotation[1], &obj->rotation[2]);
				} else if (strcmp(type, "SCALE") == 0) {
					sscanf(buff, "$ %*s %f %f %f", &obj->scale[0], &obj->scale[1], &obj->scale[2]);
					obj->scale[0] *= (1.0f / 16);
					obj->scale[1] *= (1.0f / 16);
					obj->scale[2] *= (1.0f / 16);
				}
				break;
			}
			case 'C': { // Color
				PartColor color;
				unsigned int r, g, b;
				float a;
				sscanf(buff, "C %d %02x%02x%02x %f", &color.id, &r, &g, &b, &a);
				color.color[0] = (float)r / 255;
				color.color[1] = (float)g / 255;
				color.color[2] = (float)b / 255;
				color.color[3] = a;

				listPush(obj->colors, &color);
				break;
			}
			case 'B': { // Block
				StaticObjectPart part;
				int colorId;

				sscanf(buff, "B %f %f %f %d %d", &part.position[0], &part.position[1],
						&part.position[2], &part.type, &colorId);

				ListElement *it;
				for (it = obj->colors->first; it != NULL; it = it->next) {
					if (((TextureBlock *)it->data)->id == colorId) {
						part.color = ((PartColor *)it->data)->color;
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

void renderStaticObject(GameInstance *this, ObjectInstance *instance) {
	StaticObject *obj = instance->object;
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(obj->position[0] + instance->position[0],
			obj->position[1] + instance->position[1],
			obj->position[2] + instance->position[2]);
	glScalef(obj->scale[0] * instance->scale[0],
			obj->scale[1] * instance->scale[1],
			obj->scale[2] * instance->scale[2]);
	glRotatef(-(obj->rotation[0] + instance->rotation[0]), 1.0f, 0.0f, 0.0f);
	glRotatef(-(obj->rotation[1] + instance->rotation[1]), 0.0f, 1.0f, 0.0f);
	glRotatef(-(obj->rotation[2] + instance->rotation[2]), 0.0f, 0.0f, 1.0f);

	glGetFloatv(GL_MODELVIEW_MATRIX, &instance->moveMat);
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, instance->moveMat);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, this->blankTextureId);
	ListElement *it;
	for (it = obj->parts->first; it != NULL; it = it->next) {
		StaticObjectPart part = *(StaticObjectPart *)it->data;
		glUniform4fv(this->shader->baseColor, 1, part.color);

		if ((part.type & PTMASK_RENDER_UP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, -1, 0.0f,
					0.0f, 1, 0.0f, 0.0f,
					part.position[0], part.position[1] + 1, part.position[2], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_DOWN) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, cosf(PI + PI / 2), -sinf(PI + PI / 2), 0.0f,
					0.0f, sinf(PI + PI / 2), cosf(PI + PI / 2), 0.0f,
					part.position[0], part.position[1], part.position[2] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_TOP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI), 0.0f, sinf(PI), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI), 0.0f, cosf(PI), 0.0f,
					part.position[0] + 1, part.position[1], part.position[2] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_LEFT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI / 2), 0.0f, sinf(PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI / 2), 0.0f, cosf(PI / 2), 0.0f,
					part.position[0], part.position[1], part.position[2] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_BOTTOM) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					part.position[0], part.position[1], part.position[2], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_RIGHT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI + PI / 2), 0.0f, sinf(PI + PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI + PI / 2), 0.0f, cosf(PI + PI / 2), 0.0f,
					part.position[0] + 1, part.position[1], part.position[2], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

	}
}
