#ifndef FONT_H_
#define FONT_H_

#include "object.h"

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

#endif /* FONT_H_ */
