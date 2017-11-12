#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include "game.h"

typedef enum {
	OBJECT,
	TEXT,
	IMAGE,
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

typedef struct {
	void *value;
	size_t valueLength;
} GenericType;

typedef struct Component {
	GLuint id;
	GenericType *value;
	GLfloat x;
	GLfloat y;
	RelativeX relativeX;
	RelativeY relativeY;
	ComponentType type;

	union {
		struct TextComponent {
			FontSize fontSize;
			char *text; //TODO: free
			GLfloat color[4];
		} text;
		struct ObjectComponent {
			ActiveObjectInstance *object;
		} object;
		struct ImageComponent {
			GLuint textureId;
		} image;
	};

	void (*onLoad)(struct Component*, GameInstance*);
	void (*onDestroy)(struct Component*, GameInstance*);
	void (*onRender)(struct Component*, GameInstance*);
	void (*onCalc)(struct Component*, GameInstance*);
	void (*onHover)(struct Component*, GameInstance*);
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

void updateCursor(GameInstance*, int);

#endif /* COMPONENTS_H_ */
