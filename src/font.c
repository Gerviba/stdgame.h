#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "includes.h"

static Char* loadChar(char path[], char charId, GLfloat *colors) {
	FILE *file;
	char buff[255];

	file = fopen(path, "r");
	if (!file)
		return NULL;

	Char *c = malloc(sizeof(Char));
	c->parts = newLinkedListPointer(sizeof(CharPart));
	while (fgets(buff, 255, file)) {
		switch (buff[0]) {
			case '$': { // Basic info
				char type[32];
				sscanf(buff, "$ %s", type);
				if (strcmp(type, "WIDTH") == 0) {
					sscanf(buff, "$ %*s %d", &c->width);
				} else if (strcmp(type, "Y") == 0) {
					sscanf(buff, "$ %*s %d", &c->y);
				}
				break;
			}
			case 'C': { // Color
				unsigned int r, g, b, id;
				float a;
				sscanf(buff, "C %d %02x%02x%02x %f", &id, &r, &g, &b, &a);
				--id;
				colors[id * 4] = (float) r / 255;
				colors[id * 4 + 1] = (float) g / 255;
				colors[id * 4 + 2] = (float) b / 255;
				colors[id * 4 + 3] = a;

				break;
			}
			case 'B': { // Block
				CharPart part;

				sscanf(buff, "B %f %f %f %d %d", &part.position[X], &part.position[Y],
						&part.position[Z], &part.type, &part.colorId);
				--part.colorId;

				listPush(c->parts, &part);
				break;
			}
		}
	}
	c->code = charId;
	return c;
}

void initFont(GameInstance *this) {
	this->font = malloc(sizeof(Font));
	this->font->chars = newLinkedListPointer(sizeof(Char));

	FILE *file;
	char buff[255];
	printf("[Font] Loading font: default\n");

	file = fopen("assets/fonts/default.font", "r");
	if (!file) {
		fprintf(stderr, "[Font] Failed to load font.\n");
	}

	while (fgets(buff, 255, file)) {
		int colorSize;
		if (sscanf(buff, "# COLOR_COUNT %d", &colorSize) == 1) {
			this->font->colors = malloc(sizeof(GLfloat) * 4 * colorSize);
			continue;
		}

		char charId;
		char path[237];
		sscanf(buff, "%c %s", &charId, path);
		char finalPath[255] = "assets/fonts/";
		strcat(finalPath, path);

		Char *c = loadChar(finalPath, charId, this->font->colors);
		if (c == NULL) {
			fprintf(stderr, "[Font] Failed to load char object from '%s'\n", finalPath);
			continue;
		}

		listPush(this->font->chars, c);
	}

	this->font->unknown = loadChar("assets/fonts/_unknown.char", '\0', this->font->colors);
	if (this->font->unknown == NULL) {
		fprintf(stderr, "[Font] Failed to load char object from '%s'\n",
				"assets/fonts/_unknown.char");
	}

	Char *space = loadChar("assets/fonts/_space.char", ' ', this->font->colors);
	if (space == NULL) {
		fprintf(stderr, "[Font] Failed to load char object from '%s'\n",
				"assets/fonts/_space.char");
	} else {
		listPush(this->font->chars, space);
	}
}

//TODO: Call
void freeFont(GameInstance *this) {
	free(this->font->unknown);
	free(this->font->colors);
	ListElement *it;
	for (it = this->font->chars->first; it != NULL; it = it->next)
		listClear(((Char *)it->data)->parts);
	listClear(this->font->chars);
	free(this->font);
}

static Char* getChar(Font *font, char c) {
	ListElement *it;
	for (it = font->chars->first; it != NULL; it = it->next)
		if (c == ((Char *)it->data)->code)
			return it->data;
	return font->unknown;
}

static void renderChar(GameInstance *this, Font *font, Char *c, GLfloat x, GLfloat z, GLfloat *defaultColor) {
	ListElement *it;
	for (it = c->parts->first; it != NULL; it = it->next) {
		CharPart part = *(CharPart *)it->data;
		glUniform4fv(this->shader->baseColor, 1, part.colorId == -1 ?
				defaultColor : &font->colors[4 * part.colorId]);

		if ((part.type & PTMASK_RENDER_UP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, -1, 0.0f,
					0.0f, 1, 0.0f, 0.0f,
					x + part.position[X], part.position[Y] + 1, z + part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_DOWN) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, cosf(PI + PI / 2), -sinf(PI + PI / 2), 0.0f,
					0.0f, sinf(PI + PI / 2), cosf(PI + PI / 2), 0.0f,
					x + part.position[X], part.position[Y], z + part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_TOP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI), 0.0f, sinf(PI), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI), 0.0f, cosf(PI), 0.0f,
					x + part.position[X] + 1, part.position[Y], z + part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_LEFT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI / 2), 0.0f, sinf(PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI / 2), 0.0f, cosf(PI / 2), 0.0f,
					x + part.position[X], part.position[Y], z + part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_BOTTOM) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					x + part.position[X], part.position[Y], z + part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_RIGHT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI + PI / 2), 0.0f, sinf(PI + PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI + PI / 2), 0.0f, cosf(PI + PI / 2), 0.0f,
					x + part.position[X] + 1, part.position[Y], z + part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

	}
}

//TODO: Smaller font
void renderFontTo(GameInstance *this, char str[], GLfloat position[3], GLfloat defaultColor[4]) {
	static const GLfloat dist = 1.0 / 16;

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(position[X], position[Y], position[Z]);
	glScalef(dist, dist, dist);

	GLfloat moveMat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, moveMat);
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, moveMat);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, this->blankTextureId);

	int i = 0;
	GLfloat x = 0;
	while (str[i] != '\0') {
		Char *c = getChar(this->font, toupper(str[i]));
		renderChar(this, this->font, c, x, 1.0f, defaultColor);
		x += c->width + 1;
		++i;
	}
}
