/**
 * @file font.h
 * @author Gerviba (Szabo Gergely)
 * @brief Font family loader and renderer
 *
 * TODO: Longer description
 */

#ifndef FONT_H_
#define FONT_H_

#include "stdgame.h"
#include "object.h"

/**
 * Font size type
 */
typedef enum {
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

struct Char {
	char code;
	char name[32];
	GLuint width;
	GLint y;
	LinkedList /*CharPart*/ *parts;
};

struct Font {
	LinkedList /*Char*/ *chars;
	Char *unknown;
	GLfloat *colors; // 4 * size TODO: Add pointer to its name
};

/**
 * Initialize font from `assets/fonts/default.font`
 */
void initFont(GameInstance*);

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
void renderFontTo(GameInstance* this, char str[], GLfloat position[3], GLfloat color[4], FontSize size);

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
void renderFontToComponent(GameInstance* this, char str[], GLfloat position[3], GLfloat color[4],
		FontSize size, GLfloat min[3], GLfloat max[3]);

/**
 * Free the loaded fonts
 *
 * Must be called before exit
 */
void freeFont(GameInstance*);


Char* getChar(Font*, char);

#endif /* FONT_H_ */
