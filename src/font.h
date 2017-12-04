/**
 * @file font.h
 * @author Gerviba (Szabo Gergely)
 * @brief Font family loader and renderer (header)
 *
 * @par Definition:
 * 		font.c
 */

#ifndef FONT_H_
#define FONT_H_

#include "stdgame.h"
#include "object.h"

/**
 * Font size enumeration
 */
typedef enum {
	/** 1/64 unit per cube */
	FS_EXTRALOW_DPI = 64,
	/** 1/32 unit per cube */
	FS_LOW_DPI = 32,
	/** 1/16 unit per cube */
	FS_NORMAL_DPI = 16,
	/** 1/8 unit per cube */
	FS_HIGH_DPI = 8
} FontSize;

/**
 * Cube structure for character
 */
struct CharPart {
	/** Index of the cube color.
	 * Index 0 means dynamic.
	 * Dynamic color must be pass as an argument to the renderer function
	 */
	int colorId;
	/** Type of cube */
	PartType type;
	/** Position of the cube in the character */
	GLfloat position[3];
};

/**
 * Character object (used in Font families)
 * @see Font
 */
struct Char {
	char code;
	char name[32];
	GLuint width;
	GLint y;
	LinkedList /*CharPart*/ *parts;
};

/**
 * Font family
 */
struct Font {
	LinkedList /*Char*/ *chars;
	Char *unknown;
	GLfloat *colors; // 4 * size TODO: Add pointer to its name
};


void initFont(GameInstance*);

void renderFontTo(GameInstance* this, char str[], GLfloat position[3], GLfloat color[4], FontSize size);

void renderFontToComponent(GameInstance* this, char str[], GLfloat position[3], GLfloat color[4],
		FontSize size, GLfloat min[3], GLfloat max[3]);

void freeFont(GameInstance*);

Char* getChar(Font*, char);

#endif /* FONT_H_ */
