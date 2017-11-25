#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "stdgame.h"

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

GLfloat getAbsoluteX(GameInstance *this, RelativeX relX) {
	if (relX == X_LEFT)
		return this->camera->position[X] - (this->options->tanFov * (this->camera->position[Z] - 1));
	else if (relX == X_RIGHT)
		return this->options->tanFov * (this->camera->position[Z] - 1) + this->camera->position[X];
	else
		return this->camera->position[X];
}

GLfloat getAbsoluteY(GameInstance *this, RelativeY relY) {
	if (relY == Y_BOTTOM)
		return this->camera->position[Y] - ((this->options->tanFov * (this->camera->position[Z] - 1))
				* this->options->aspectRatio);
	else if (relY == Y_TOP)
		return ((this->options->tanFov * (this->camera->position[Z] - 1) * this->options->aspectRatio)
				+ this->camera->position[Y]);
	else
		return this->camera->position[Y];
}

GLfloat getCursorProjectedX(GameInstance *this, double x) {
	return this->camera->position[X] + ((this->options->tanFov * (this->camera->position[Z] - 1)) *
			-((this->options->width / 2 - x) / this->options->width)) * 2;
}

GLfloat getCursorProjectedY(GameInstance *this, double y) {
	return this->camera->position[Y] + ((this->options->tanFov * (this->camera->position[Z] - 1)) *
			((this->options->height / 2 - y) / this->options->height)) * 2 * this->options->aspectRatio;
}

GLfloat getFontAlign(GameInstance *this, char str[], FontSize fontSize, Align align) {
	if (align == ALIGN_LEFT)
		return 0;

	GLfloat size = 0;
	int i = 0;
	while (str[i] != '\0') {
		size += getChar(this->font, toupper(str[i]))->width + 1;
		++i;
	}
	size /= fontSize;

	return align == ALIGN_CENTER ? size / 2 : size;
}

void updateCursor(GameInstance *this) {
	double cursorX, cursorY;
	glfwGetCursorPos(this->window, &cursorX, &cursorY);

	this->cursor->pointer->position[X] = getCursorProjectedX(this, cursorX);
	this->cursor->pointer->position[Y] = getCursorProjectedY(this, cursorY);
	this->cursor->pointer->position[Z] = 1 + (1.0 / 16);

	this->cursor->pointer->activePart =
			glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ? 1 : 0;
}

void renderTextComponent(Component *comp, GameInstance *this) {
	renderFontToComponent(this, comp->text->text, (GLfloat[]) {
			comp->position[X] + getAbsoluteX(this, comp->relativeX)
					- getFontAlign(this, comp->text->text, comp->text->fontSize, comp->text->align),
			comp->position[Y] + getAbsoluteY(this, comp->relativeY), 1.0f},
			comp->text->color, comp->text->fontSize, comp->text->rawMin, comp->text->rawMax);
}

void calcObjectComponentPosition(Component *comp, GameInstance *this) {
	comp->object->object->position[X] = comp->position[X] + getAbsoluteX(this, comp->relativeX);
	comp->object->object->position[Y] = comp->position[Y] + getAbsoluteY(this, comp->relativeY);
}

void calcTextButton(Component *comp, GameInstance *this) {
	if (comp->onClick == NULL)
		return;

	if (comp->text->rawMin[X] <= this->cursor->pointer->position[X] &&
			comp->text->rawMin[Y] <= this->cursor->pointer->position[Y] &&
			comp->text->rawMax[X] >= this->cursor->pointer->position[X] &&
			comp->text->rawMax[Y] >= this->cursor->pointer->position[Y]) {

		if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			setColor(comp->text->color,
					comp->text->baseColor[R] * 0.6,
					comp->text->baseColor[G] * 0.6,
					comp->text->baseColor[B] * 0.6,
					comp->text->baseColor[A]);
		} else {
			setColor(comp->text->color,
					comp->text->baseColor[R] * 0.8,
					comp->text->baseColor[G] * 0.8,
					comp->text->baseColor[B] * 0.8,
					comp->text->baseColor[A]);
		}
	} else {
		setColor(comp->text->color,
				comp->text->baseColor[R],
				comp->text->baseColor[G],
				comp->text->baseColor[B],
				comp->text->baseColor[A]);
	}
}

void clickStartButton(Component *comp, GameInstance *this) {
	freeMap(this->map);
	this->map = loadMap(this, "assets/maps/test2.map");
	initPlayer(this);
	updateCamera(this);
}

void clickOptions(Component *comp, GameInstance *this) {
	freeMap(this->map);
	this->map = loadMap(this, "assets/maps/options.menu");
	updateCamera(this);
}

void clickCredits(Component *comp, GameInstance *this) {
	freeMap(this->map);
	this->map = loadMap(this, "assets/maps/credits.menu");
	updateCamera(this);
}

void clickExit(Component *comp, GameInstance *this) {
	glfwSetWindowShouldClose(this->window, GL_TRUE);
}

void clickBack(Component *comp, GameInstance *this) {
	freeMap(this->map);
	this->map = loadMap(this, "assets/maps/main.menu");
	updateCamera(this);
}

void clickOpenGithub() {

#ifdef  __WIN32
	system("start /c https://github.com/Gerviba/stdgame.h");
#elif defined APPLE
	system("open https://github.com/Gerviba/stdgame.h");
#else
	system("xdg-open https://github.com/Gerviba/stdgame.h &");
#endif
}
