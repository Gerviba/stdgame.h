#include <stdlib.h>
#include <math.h>
#include "includes.h"

// Generic Value

GenericType* newGenericValue(void *value, size_t size) {
	GenericType *gt = new(GenericType);
	gt->valueLength = size;
	gt->value = malloc(gt->valueLength);
	int i;
	for (i = 0; i < gt->valueLength; ++i)
		*(char *)(gt->value + i) = *(char *)(value + i);
	return gt;
}

GenericType* newGenericIntValue(GLint intValue) {
	GenericType *gt = new(GenericType);
	gt->valueLength = sizeof(GLint);
	gt->value = malloc(gt->valueLength);
	void *value = &intValue;
	int i;
	for (i = 0; i < gt->valueLength; ++i)
		*(char *)(gt->value + i) = *(char *)(value + i);
	return gt;
}

GenericType* newGenericFloatValue(GLfloat floatValue) {
	GenericType *gt = new(GenericType);
	gt->valueLength = sizeof(GLfloat);
	gt->value = malloc(gt->valueLength);
	void *value = &floatValue;
	int i;
	for (i = 0; i < gt->valueLength; ++i)
		*(char *)(gt->value + i) = *(char *)(value + i);
	return gt;
}

void setGenericValue(GenericType *value, void *newValue, size_t newSize) {
	free(value->value);
	value->valueLength = newSize;
	value->value = malloc(value->valueLength);
	int i;
	for (i = 0; i < value->valueLength; ++i)
		*(char *)(value->value + i) = *(char *)(newValue + i);
}

void setGenericIntValue(GenericType *value, GLint newIntValue) {
	free(value->value);
	value->valueLength = sizeof(GLint);
	value->value = malloc(value->valueLength);
	void *newValue = &newIntValue;
	int i;
	for (i = 0; i < value->valueLength; ++i)
		*(char *)(value->value + i) = *(char *)(newValue + i);
}

void setGenericFloatValue(GenericType *value, GLfloat newFloatValue) {
	free(value->value);
	value->valueLength = sizeof(GLfloat);
	value->value = malloc(value->valueLength);
	void *newValue = &newFloatValue;
	int i;
	for (i = 0; i < value->valueLength; ++i)
		*(char *)(value->value + i) = *(char *)(newValue + i);
}

GLint getGenericIntValue(GenericType *value) {
	return *(GLint *)(*value).value;
}

GLfloat getGenericFloatValue(GenericType *value) {
	return *(GLfloat *)(*value).value;
}

void freeGenericValue(GenericType *value) {
	free(value->value);
	free(value);
}

// Absolute Position

GLfloat getAbsoluteX(GameInstance *this, RelativeX relX) {
	if (relX == X_LEFT)
		return this->camera->position[X] - (this->options.tanFov * (this->camera->position[Z] - 1));
	else if (relX == X_RIGHT)
		return this->options.tanFov * (this->camera->position[Z] - 1) + this->camera->position[X];
	else
		return this->camera->position[X];
}

GLfloat getAbsoluteY(GameInstance *this, RelativeY relY) {
	if (relY == Y_BOTTOM)
		return this->camera->position[Y] - ((this->options.tanFov * (this->camera->position[Z] - 1))
				* this->options.aspectRatio);
	else if (relY == Y_TOP)
		return ((this->options.tanFov * (this->camera->position[Z] - 1) * this->options.aspectRatio)
				+ this->camera->position[Y]);
	else
		return this->camera->position[Y];
}

GLfloat getCursorProjectedX(GameInstance *this, double x) {
	return this->camera->position[X] + ((this->options.tanFov * (this->camera->position[Z] - 1)) *
			-((this->options.width / 2 - x) / this->options.width)) * 1.8;
}

GLfloat getCursorProjectedY(GameInstance *this, double y) {
	return this->camera->position[Y] + ((this->options.tanFov * (this->camera->position[Z] - 1)) *
			((this->options.height / 2 - y) / this->options.height)) * 1.8 * this->options.aspectRatio;
}

void updateCursor(GameInstance *this, int cursorId) {
	double cursorX, cursorY;
	glfwGetCursorPos(this->window, &cursorX, &cursorY);

	ListElement *it;
	StaticObjectInstance *cursor;
	for (it = this->map->objects->staticInstances->first; it != NULL; it = it->next)
		if (((StaticObjectInstance *) it->data)->id == cursorId)
			cursor = ((StaticObjectInstance *) it->data);

	cursor->position[X] = getCursorProjectedX(this, cursorX);
	cursor->position[Y] = getCursorProjectedY(this, cursorY);
	cursor->position[Z] = 1 + (1.0 / 16);
	renderStaticObject(this, cursor);
}

// Render Components

void renderTextComponent(Component *comp, GameInstance *this) {
	renderFontTo(this, comp->text.text, (GLfloat[]) {
			comp->x + getAbsoluteX(this, comp->relativeX),
			comp->y + getAbsoluteY(this, comp->relativeY), 1.0f},
			comp->text.color, comp->text.fontSize);
}
