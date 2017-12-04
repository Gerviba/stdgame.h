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
		return this->camera->position[X] -
				(this->options->tanFov / 2 * (this->camera->position[Z] - 1) * this->options->aspectRatio);
	else if (relX == X_RIGHT)
		return this->camera->position[X] +
				(this->options->tanFov / 2 * (this->camera->position[Z] - 1) * this->options->aspectRatio);
	else
		return this->camera->position[X];
}

GLfloat getAbsoluteY(GameInstance *this, RelativeY relY) {
	if (relY == Y_BOTTOM)
		return this->camera->position[Y] - (this->options->tanFov / 2 * (this->camera->position[Z] - 1));
	else if (relY == Y_TOP)
		return this->camera->position[Y] + (this->options->tanFov / 2 * (this->camera->position[Z] - 1));
	else
		return this->camera->position[Y];
}

GLfloat getCursorProjectedX(GameInstance *this, double x) {
	return this->camera->position[X] + ((this->options->tanFov / 2 * (this->camera->position[Z] - 1))
			* (this->options->width / 2 - x) / this->options->width) * -2 * this->options->aspectRatio;
}

GLfloat getCursorProjectedY(GameInstance *this, double y) {
	return this->camera->position[Y] + ((this->options->tanFov / 2 * (this->camera->position[Z] - 1))
			* (this->options->height / 2 - y) / this->options->height) * 2;
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

void calcOptionsGraphicsButton(Component *comp, GameInstance *this) {
	Iterator it;
	foreach (it, this->map->menu->components->first) {
		Component *temp = it->data;
		if (temp->type == CT_TEXT && temp->id == 11) { /**< NO MSAA */
			if (this->options->msaa == 0) {
				setColor(temp->text->baseColor, 0.992156863f, 0.909803922f, 0.529411765f,
						temp->text->baseColor[A]);
			} else {
				setColor(temp->text->baseColor, 1.0f, 1.0f, 1.0f,
						temp->text->baseColor[A]);
			}
		} else if (temp->type == CT_TEXT && temp->id == 12) { /**< x4 MSAA */
			if (this->options->msaa == 4) {
				setColor(temp->text->baseColor, 0.992156863f, 0.909803922f, 0.529411765f,
						temp->text->baseColor[A]);
			} else {
				setColor(temp->text->baseColor, 1.0f, 1.0f, 1.0f,
						temp->text->baseColor[A]);
			}
		} else if (temp->type == CT_TEXT && temp->id == 13) { /**< x8 MSAA */
			if (this->options->msaa == 8) {
				setColor(temp->text->baseColor, 0.992156863f, 0.909803922f, 0.529411765f,
						temp->text->baseColor[A]);
			} else {
				setColor(temp->text->baseColor, 1.0f, 1.0f, 1.0f,
						temp->text->baseColor[A]);
			}
		} else if (temp->type == CT_TEXT && temp->id == 14) { /**< x16 MSAA */
			if (this->options->msaa == 16) {
				setColor(temp->text->baseColor, 0.992156863f, 0.909803922f, 0.529411765f,
						temp->text->baseColor[A]);
			} else {
				setColor(temp->text->baseColor, 1.0f, 1.0f, 1.0f,
						temp->text->baseColor[A]);
			}
		} else if (temp->type == CT_TEXT && temp->id == 21) { /**< NO FULLSCREEN */
			if (this->options->fullscreen) {
				setColor(temp->text->baseColor, 1.0f, 1.0f, 1.0f,
						temp->text->baseColor[A]);
			} else {
				setColor(temp->text->baseColor, 0.992156863f, 0.909803922f, 0.529411765f,
						temp->text->baseColor[A]);
			}
		} else if (temp->type == CT_TEXT && temp->id == 22) { /**< FULLSCREEN */
			if (!this->options->fullscreen) {
				setColor(temp->text->baseColor, 1.0f, 1.0f, 1.0f,
						temp->text->baseColor[A]);
			} else {
				setColor(temp->text->baseColor, 0.992156863f, 0.909803922f, 0.529411765f,
						temp->text->baseColor[A]);
			}
		} else if (temp->type == CT_TEXT && temp->id == 31) { /**< NO CAMERA MOVEMENT */
			if (this->options->cameraMovement) {
				setColor(temp->text->baseColor, 1.0f, 1.0f, 1.0f,
						temp->text->baseColor[A]);
			} else {
				setColor(temp->text->baseColor, 0.992156863f, 0.909803922f, 0.529411765f,
						temp->text->baseColor[A]);
			}
		} else if (temp->type == CT_TEXT && temp->id == 32) { /**< CAMERA MOVEMENT */
			if (!this->options->cameraMovement) {
				setColor(temp->text->baseColor, 1.0f, 1.0f, 1.0f,
						temp->text->baseColor[A]);
			} else {
				setColor(temp->text->baseColor, 0.992156863f, 0.909803922f, 0.529411765f,
						temp->text->baseColor[A]);
			}
		} else if (temp->type == CT_TEXT && temp->id == 41) { /**< RESOLUTION */
			if (this->options->windowedWidth == 0) {
				setColor(temp->text->baseColor, 1.0f, 1.0f, 1.0f,
						temp->text->baseColor[A]);
			} else {
				setColor(temp->text->baseColor, 0.992156863f, 0.909803922f, 0.529411765f,
						temp->text->baseColor[A]);
			}
		} else if (temp->type == CT_TEXT && temp->id == 42) { /**< RESOLUTION */
			if (this->options->windowedWidth != 0) {
				setColor(temp->text->baseColor, 1.0f, 1.0f, 1.0f,
						temp->text->baseColor[A]);
			} else {
				setColor(temp->text->baseColor, 0.992156863f, 0.909803922f, 0.529411765f,
						temp->text->baseColor[A]);
			}
		}

	}

	calcTextButton(comp, this);
}

static Component* getTextComponentById(GameInstance *this, GLint id) {
	Iterator it;
	foreach (it, this->map->menu->components->first) {
		Component *comp = it->data;
		if (comp->type == CT_TEXT && comp->id == id)
			return comp;
	}
	return NULL;
}

void clickGameSelector(Component *comp, GameInstance *this) {
	freeMap(this->map);
	this->map = loadMap(this, "assets/maps/select.menu");
	updateCamera(this);

	FILE *file = fopen("data/records.dat", "r");
	if (file == NULL)
		return;

	char temp[255];
	int type, value;
	while (fscanf(file, "%s %d %d", temp, &type, &value) == 3) {
		if (equals(temp, "Tutorial")) {
			if (type == 0) {
				Component *comp = getTextComponentById(this, 11);
				if (comp != NULL) {
					char str[17];
					sprintf(str, "BEST TIME: %02d:%02d", value / 60, value % 60);
					strcpy(comp->text->text, str);
				}
			} else if (type == 1) {
				Component *comp = getTextComponentById(this, 12);
				if (comp != NULL) {
					char str[27];
					sprintf(str, "BEST SCORE: %d *", value);
					strcpy(comp->text->text, str);
				}
			}
		} else if (equals(temp, "Pyramid")) {
			if (type == 0) {
				Component *comp = getTextComponentById(this, 21);
				if (comp != NULL) {
					char str[17];
					sprintf(str, "BEST TIME: %02d:%02d", value / 60, value % 60);
					strcpy(comp->text->text, str);
				}
			} else if (type == 1) {
				Component *comp = getTextComponentById(this, 22);
				if (comp != NULL) {
					char str[27];
					sprintf(str, "BEST SCORE: %d *", value);
					strcpy(comp->text->text, str);
				}
			}
		}

	}

	fclose(file);
}

void clickStartButton(Component *comp, GameInstance *this) {
	if (comp->id == 10) {
		freeMap(this->map);
		this->map = loadMap(this, "assets/maps/tutorial.map");
		initPlayer(this);
		updateCamera(this);
	} else if (comp->id == 20) {
		freeMap(this->map);
		this->map = loadMap(this, "assets/maps/pyramid.map");
		initPlayer(this);
		updateCamera(this);
	}
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
	if (this->options->reloadProgram) {
		glfwSetWindowShouldClose(this->window, GL_TRUE);
		return;
	}
	freeMap(this->map);
	this->map = loadMap(this, "assets/maps/main.menu");
	updateCamera(this);
}

void clickControllsSet(Component *comp, GameInstance *this) {
	this->options->selectedToSet = comp;
	int id = 0, i = 0;
	while (comp->text->text[i] != '\0') {
		if (comp->text->text[i] == ' ')
			++id;
		++i;
	}

	if (id != 2)
		return;

	strcpy(comp->text->text, "[-]");
	setColor(comp->text->baseColor, 1.0f, 0.0f, 0.0f, comp->text->baseColor[A]);

	if (equals(this->options->selectedToSet->value->value, "moveLeft"))
		array3(this->options->moveLeft.id, -1.0f, -1.0f, -1.0f);
	else if (equals(this->options->selectedToSet->value->value, "moveRight"))
		array3(this->options->moveRight.id, -1.0f, -1.0f, -1.0f);
	else if (equals(this->options->selectedToSet->value->value, "jump"))
		array3(this->options->jump.id, -1.0f, -1.0f, -1.0f);
	else if (equals(this->options->selectedToSet->value->value, "sneek"))
		array3(this->options->sneek.id, -1.0f, -1.0f, -1.0f);
	else if (equals(this->options->selectedToSet->value->value, "attack"))
		array3(this->options->attack.id, -1.0f, -1.0f, -1.0f);
	else if (equals(this->options->selectedToSet->value->value, "use"))
		array3(this->options->use.id, -1.0f, -1.0f, -1.0f);
	else if (equals(this->options->selectedToSet->value->value, "spell1"))
		array3(this->options->spell1.id, -1.0f, -1.0f, -1.0f);
	else if (equals(this->options->selectedToSet->value->value, "spell2"))
		array3(this->options->spell2.id, -1.0f, -1.0f, -1.0f);
	else if (equals(this->options->selectedToSet->value->value, "spell3"))
		array3(this->options->spell3.id, -1.0f, -1.0f, -1.0f);
	else if (equals(this->options->selectedToSet->value->value, "menu"))
		array3(this->options->menu.id, -1.0f, -1.0f, -1.0f);
}

void clickGraphicsSet(Component *comp, GameInstance *this) {
	if (comp->id == 11) {
		this->options->msaa = 0;
		this->options->reloadProgram = GL_TRUE;
	} else if (comp->id == 12) {
		this->options->msaa = 4;
		this->options->reloadProgram = GL_TRUE;
	} else if (comp->id == 13) {
		this->options->msaa = 8;
		this->options->reloadProgram = GL_TRUE;
	} else if (comp->id == 14) {
		this->options->msaa = 16;
		this->options->reloadProgram = GL_TRUE;
	} else if (comp->id == 21) {
		this->options->fullscreen = GL_FALSE;
		this->options->reloadProgram = GL_TRUE;
	} else if (comp->id == 22) {
		this->options->windowedWidth = 0;
		this->options->windowedHeight = 0;
		this->options->fullscreen = GL_TRUE;
		this->options->reloadProgram = GL_TRUE;
	} else if (comp->id == 31) {
		this->options->cameraMovement = GL_FALSE;
	} else if (comp->id == 32) {
		this->options->cameraMovement = GL_TRUE;
	} else if (comp->id == 41) {
		this->options->windowedWidth = 1280;
		this->options->windowedHeight = 720;
		this->options->fullscreen = GL_FALSE;
		this->options->reloadProgram = GL_TRUE;
	} else if (comp->id == 42) {
		this->options->windowedWidth = 0;
		this->options->windowedHeight = 0;
		this->options->reloadProgram = GL_TRUE;
	}
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
