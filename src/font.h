#ifndef FONT_H_
#define FONT_H_

#include "object.h"

typedef enum {
	FS_LOW_DPI = 32,
	FS_NORMAL_DPI = 16,
	FS_HIGH_DPI = 8
} FontSize;

typedef struct {
	int colorId;
	PartType type;
	GLfloat position[3];
} CharPart;

typedef struct {
	char code;
	char name[32];
	GLuint width;
	GLint y;
	LinkedList /*CharPart*/ *parts;
} Char;

typedef struct {
	LinkedList /*Char*/ *chars;
	Char *unknown;
	GLfloat *colors; // 4 * size
} Font;

#include "game.h"

void initFont(GameInstance*);
void renderFontTo(GameInstance*, char[], GLfloat[3], GLfloat[4], FontSize);
void freeFont(GameInstance*);

#endif /* FONT_H_ */
