/**
 * @file font.c
 * @author Gerviba (Szabo Gergely)
 * @brief Font family loader and renderer
 *
 * @par Header:
 * 		font.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "stdgame.h"

static Char* loadChar(char path[], char charId, GLfloat *colors);
static void renderChar(GameInstance *this, Font *font, Char *c, GLfloat x, GLfloat z, GLfloat *defaultColor);

/**
 * Loads a char
 * @param path Char file path
 * @paran charId Character identifier
 * @param colors Color bank array
 * @returs The loaded Char object
 */
static Char* loadChar(char path[], char charId, GLfloat *colors) {
	FILE *file;
	char buff[255];

	file = fopen(path, "r");
	if (!file)
		return NULL;

	Char *c = new(Char);
	c->parts = newList(CharPart);
	while (fgets(buff, 255, file)) {
		switch (buff[0]) {
			case '$': { // Basic info
				char type[32];
				sscanf(buff, "$ %s", type);
				if (equals(type, "WIDTH")) {
					sscanf(buff, "$ %*s %d", &c->width);
				} else if (equals(type, "Y")) {
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

				sscanf(buff, "B %f %f %f %d %d", &part.position[X], &part.position[Y], &part.position[Z],
						(int*) &part.type, &part.colorId);
				--part.colorId;

				listPush(c->parts, &part);
				break;
			}
		}
	}
	c->code = charId;
	return c;
}

/**
 * Initialize font from `assets/fonts/default.font`
 */
void initFont(GameInstance *this) {
	this->font = new(Font);
	this->font->chars = newList(Char);

	FILE *file;
	char buff[255];
	DEBUG("Font", "Loading font: default");

	file = fopen("assets/fonts/default.font", "r");
	if (!file) {
		ERROR("Failed to load font.");
		return;
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
			ERROR("Failed to load char object from '%s'", finalPath);
			continue;
		}

		listPush(this->font->chars, c);
		free(c);
	}

	Char *space = loadChar("assets/fonts/_space.char", ' ', this->font->colors);
	if (space == NULL) {
		ERROR("Failed to load char object from '%s'", "assets/fonts/_space.char");
	} else {
		listPush(this->font->chars, space);
		free(space);
	}

	this->font->unknown = loadChar("assets/fonts/_unknown.char", '\0', this->font->colors);
	if (this->font->unknown == NULL) {
		ERROR("Failed to load char object from '%s'", "assets/fonts/_unknown.char");
	}

	fclose(file);
}

//TODO: Call
/**
 * Free the loaded fonts <br>
 * Must be called before exit
 *
 * @param this Actual GameInstance instance
 */
void freeFont(GameInstance *this) {
	free(this->font->unknown);
	free(this->font->colors);
	Iterator it;
	for (it = this->font->chars->first; it != NULL; it = it->next)
		listFree(((Char *)it->data)->parts);
	listFree(this->font->chars);
	free(this->font);
}

/**
 * Get Char type pointer from the default.
 *
 * @param font Font family
 * @param c Character identifier
 * @returns Char object pointer
 */
Char* getChar(Font *font, char c) {
	Iterator it;
	for (it = font->chars->first; it != NULL; it = it->next)
		if (c == ((Char *)it->data)->code)
			return it->data;
	return font->unknown;
}

/**
 * Render selected char.
 *
 * @param this Actual GameInstance instance
 * @param font Font family
 * @param c Character pointer
 * @param x Coordinates (x-dim)
 * @param z Coordinates (z-dim)
 * @param defaultColor Default color of the font
 */
static void renderChar(GameInstance *this, Font *font, Char *c, GLfloat x, GLfloat z, GLfloat *defaultColor) {
	Iterator it;
	foreach (it, c->parts->first) {
		CharPart part = *(CharPart *)it->data;
		glUniform4fv(this->shader->baseColor, 1, part.colorId == -1 ?
				defaultColor : &font->colors[4 * part.colorId]);

		if ((part.type & PTMASK_RENDER_UP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, -1, 0.0f,
					0.0f, 1, 0.0f, 0.0f,
					x + part.position[X], part.position[Y] + 1 + c->y, z + part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_DOWN) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, cosf(PI + PI / 2), -sinf(PI + PI / 2), 0.0f,
					0.0f, sinf(PI + PI / 2), cosf(PI + PI / 2), 0.0f,
					x + part.position[X], part.position[Y] + c->y, z + part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_TOP) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI), 0.0f, sinf(PI), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI), 0.0f, cosf(PI), 0.0f,
					x + part.position[X] + 1, part.position[Y] + c->y, z + part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_LEFT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI / 2), 0.0f, sinf(PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI / 2), 0.0f, cosf(PI / 2), 0.0f,
					x + part.position[X], part.position[Y] + c->y, z + part.position[Z] - 1, 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_BOTTOM) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					x + part.position[X], part.position[Y] + c->y, z + part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if ((part.type & PTMASK_RENDER_RIGHT) > 0) {
			glUniformMatrix4fv(this->shader->modelMat, 1, GL_FALSE, (GLfloat[]) {
					cosf(PI + PI / 2), 0.0f, sinf(PI + PI / 2), 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					-sinf(PI + PI / 2), 0.0f, cosf(PI + PI / 2), 0.0f,
					x + part.position[X] + 1, part.position[Y] + c->y, z + part.position[Z], 1.0f});
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

	}
}

/**
 * Renders the entered text.
 *
 * @param this GameInstance pointer
 * @param str String to render
 * @param position Where to render
 * @param color Dynamic color
 * @param size Size of the chars
 *
 * @note This function can be used in both `onLogic` and `onRender` functions.
 *
 * @see renderFontToComponent()
 */
void renderFontTo(GameInstance *this, char str[], GLfloat position[3], GLfloat defaultColor[4], FontSize size) {
	const GLfloat dist = 1.0 / size;

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

/**
 * Renders the entered text and saves the min and the max coordinates into the pass arguments.
 *
 * @param this GameInstance pointer
 * @param str String to render
 * @param position Where to render
 * @param color Dynamic color
 * @param size Size of the chars
 * @param min Minimum coordinate of the rendered object
 * @param max Maximum coordinate of the rendered object
 *
 * @note This function can be used in both `onLogic` and `onRender` functions.
 * @warning It sets the value of the `min` and the `max` arrays.
 *
 * @see renderFontTo()
 */
void renderFontToComponent(GameInstance *this, char str[], GLfloat position[3], GLfloat defaultColor[4],
		FontSize size, GLfloat min[3], GLfloat max[3]) {
	const GLfloat dist = 1.0 / size;

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(position[X], position[Y], position[Z]);
	glScalef(dist, dist, dist);

	GLfloat moveMat[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, moveMat);
	glUniformMatrix4fv(this->shader->moveMat, 1, GL_FALSE, moveMat);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, this->blankTextureId);

	min[X] = position[X];
	min[Y] = position[Y] - dist;

	int i = 0;
	GLfloat x = 0;
	while (str[i] != '\0') {
		Char *c = getChar(this->font, toupper(str[i]));
		renderChar(this, this->font, c, x, 1.0f, defaultColor);
		x += c->width + 1;
		++i;
	}

	max[X] = position[X] + (x * dist);
	max[Y] = position[Y] + (6 * dist);
}
