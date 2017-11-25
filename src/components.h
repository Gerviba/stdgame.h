/**
 * @file components.h
 * @author Gerviba (Szabo Gergely)
 * @brief Components and event management for menus
 */

#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include "stdgame.h"

/**
 * Component type
 */
typedef enum {
	/**
	 * Object like component.
	 * Requires a loaded ActiveObjectInstance.
	 * @see ActiveObjectInstance
	 */
	CT_OBJECT,
	/**
	 * Text component.
	 * Optimal for messages and buttons.
	 * @see TextComponent
	 */
	CT_TEXT,
	/**
	 * Image component.
	 * A texture will be loaded end rendered to this component.
	 * @see ImageComponent
	 */
	CT_IMAGE,
} ComponentType;

/**
 * Component relative horizontal position
 */
typedef enum {
	/** Relative to the left horizontal side of the screen */
	X_LEFT 		= -1,
	/** Relative to the horizontal center of the screen */
	X_CENTER 	= 0,
	/** Relative to the right horizontal side of the screen */
	X_RIGHT 	= 1
} RelativeX;

/**
 * Component relative vertical position
 */
typedef enum {
	/** Relative to the top vertical side of the screen */
	Y_BOTTOM 	= -1,
	/** Relative to the vertical center of the screen */
	Y_CENTER 	= 0,
	/** Relative to the right vertical side of the screen */
	Y_TOP 		= 1
} RelativeY;

/**
 * Alignment for text components
 */
typedef enum {
	/** The anchor point will be placed to the left side of the object */
	ALIGN_LEFT		= -1,
	/** The anchor point will be placed to the center of the object */
	ALIGN_CENTER	= 0,
	/** The anchor point will be placed to the right side of the object */
	ALIGN_RIGHT		= 1
} Align;

/**
 * The component base
 */
struct Component {
	GLuint id;
	GenericType *value;
	GLfloat position[3];
	RelativeX relativeX;
	RelativeY relativeY;
	ComponentType type;

	union {
		TextComponent *text;
		ObjectComponent *object;
		ImageComponent *image;
	};

	void (*onLoad)(struct Component*, GameInstance*);
	void (*onDestroy)(struct Component*, GameInstance*);
	void (*onRender)(struct Component*, GameInstance*);
	void (*onCalc)(struct Component*, GameInstance*);
	void (*onClick)(struct Component*, GameInstance*);
};

/**
 * Generic type variable
 * @see newGenericValue()
 * @see newGenericIntValue()
 * @see newGenericFloatValue()
 * @see setGenericValue()
 * @see setGenericIntValue()
 * @see setGenericFloatValue()
 * @see getGenericIntValue()
 * @see getGenericFloatValue()
 * @see freeGenericValue()
 */
struct GenericType {
	void *value;
	size_t valueLength;
};

struct TextComponent {
	FontSize fontSize;
	char *text; //TODO: free
	GLfloat color[4];
	GLfloat baseColor[4];
	Align align;
	GLfloat rawMax[3];
	GLfloat rawMin[3];
};

struct ObjectComponent {
	ActiveObjectInstance *object;
};

struct ImageComponent {
	GLuint textureId;
};

GenericType* newGenericValue(void*, size_t);
GenericType* newGenericIntValue(GLint);
GenericType* newGenericFloatValue(GLfloat);
void setGenericValue(GenericType*, void*, size_t);
void setGenericIntValue(GenericType*, GLint);
void setGenericFloatValue(GenericType*, GLfloat);
GLint getGenericIntValue(GenericType*);
GLfloat getGenericFloatValue(GenericType*);
void freeGenericValue(GenericType*);

GLfloat getAbsoluteX(GameInstance*, RelativeX);
GLfloat getAbsoluteY(GameInstance*, RelativeY);
GLfloat getCursorProjectedX(GameInstance*, double);
GLfloat getCursorProjectedY(GameInstance*, double);

void renderTextComponent(Component*, GameInstance*);
void calcTextButton(Component*, GameInstance*);
void calcObjectComponentPosition(Component*, GameInstance*);
void clickStartButton(Component*, GameInstance*);
void updateCursor(GameInstance *this);

#endif /* COMPONENTS_H_ */
