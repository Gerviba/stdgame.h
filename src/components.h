#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include "includes.h"

typedef enum {
	OBJECT,
	TEXT,
	IMAGE,
} ComponentType;

typedef struct {
	void *value;
	size_t valueLength;
} GenericType;

typedef struct ObjectComponent {
	ActiveObjectInstance *object;

	void (*onRender)(struct ObjectComponent*);
	void (*onCalc)(struct ObjectComponent*);
	void (*onHover)(struct ObjectComponent*);
	void (*onClick)(struct ObjectComponent*);
} ObjectComponent;

typedef struct TextComponent {
	FontSize fontSize;
	char *text;
	GLfloat color[4];

	void (*onRender)(struct TextComponent*);
	void (*onCalc)(struct TextComponent*);
	void (*onHover)(struct TextComponent*);
	void (*onClick)(struct TextComponent*);
} TextComponent;

typedef struct ImageComponent {
	GLuint textureId;

	void (*onLoad)(struct ImageComponent*);
	void (*onDestroy)(struct ImageComponent*);
	void (*onRender)(struct ImageComponent*);
	void (*onCalc)(struct ImageComponent*);
	void (*onHover)(struct ImageComponent*);
	void (*onClick)(struct ImageComponent*);
} ImageComponent;

typedef struct {
	GLuint id;
	GenericType *value;
	GLfloat x;
	GLfloat y;
	ComponentType type;

	union {
		ObjectComponent *object;
		TextComponent *text;
		ImageComponent *image;
	};
} Component;

#endif /* COMPONENTS_H_ */
