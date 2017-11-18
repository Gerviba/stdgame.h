#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include "game.h"

typedef enum {
	CT_OBJECT,
	CT_TEXT,
	CT_IMAGE,
} ComponentType;

typedef enum {
	X_LEFT 		= -1,
	X_CENTER 	= 0,
	X_RIGHT 	= 1
} RelativeX;

typedef enum {
	Y_BOTTOM 	= -1,
	Y_CENTER 	= 0,
	Y_TOP 		= 1
} RelativeY;

typedef enum {
	ALIGN_LEFT		= -1,
	ALIGN_CENTER	= 0,
	ALIGN_RIGHT		= 1
} Align;

typedef struct {
	void *value;
	size_t valueLength;
} GenericType;

typedef struct {
	FontSize fontSize;
	char *text; //TODO: free
	GLfloat color[4];
	Align align;
	GLfloat rawMax[3];
	GLfloat rawMin[3];
} TextComponent;

typedef struct {
	ActiveObjectInstance *object;
} ObjectComponent;

typedef struct {
	GLuint textureId;
} ImageComponent;

typedef struct Component {
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
	void (*onCalc)(struct Component*, GameInstance*, ActiveObjectInstance*);
	void (*onClick)(struct Component*, GameInstance*);
} Component;

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

ActiveObjectInstance* updateCursor(GameInstance*, int);
void renderTextComponent(Component*, GameInstance*);
void calcTextButton(Component*, GameInstance*, ActiveObjectInstance*);
void calcObjectComponentPosition(Component*, GameInstance*, ActiveObjectInstance*);
void clickStartButton(Component*, GameInstance*);

#endif /* COMPONENTS_H_ */
