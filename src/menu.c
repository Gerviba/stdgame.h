#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include "stdgame.h"

void renderIngameGUI(GameInstance *this) {
	GLfloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	renderFontTo(this, "Test message, $ *", (GLfloat[]) {6.0f, 7.0625f, 1.0f}, color, FS_NORMAL_DPI);

}

void onClickMenu(GameInstance *this) {
	Iterator it;
	foreach (it, this->map->menu->components->first) {
		Component *comp = it->data;
		if (comp->text->rawMin[X] <= this->cursor->pointer->position[X] &&
				comp->text->rawMin[Y] <= this->cursor->pointer->position[Y] &&
				comp->text->rawMax[X] >= this->cursor->pointer->position[X] &&
				comp->text->rawMax[Y] >= this->cursor->pointer->position[Y]) {
			if (comp->onClick != NULL)
				comp->onClick(comp, this);
		}
	}
}

void onScrollMenu(GameInstance *this, double offset) {
	if (this->map->menu->scrollOffset - offset > this->map->menu->scrollMax ||
			this->map->menu->scrollOffset - offset < this->map->menu->scrollMin)
		return;

	this->map->menu->scrollOffset -= offset;
	Iterator it;
	foreach (it, this->map->menu->components->first) {
		Component *comp = it->data;
		comp->position[Y] -= offset;
	}
}

void setOptionsDefaults(GameInstance* this) {
	array3(this->options->moveLeft.id, GLFW_KEY_A, GLFW_KEY_LEFT, -1.0f);
	array3(this->options->moveRight.id, GLFW_KEY_W, GLFW_KEY_RIGHT, -1.0f);
	array3(this->options->jump.id, GLFW_KEY_SPACE, GLFW_KEY_W, GLFW_KEY_UP);
	array3(this->options->sneek.id, GLFW_KEY_S, GLFW_KEY_DOWN, -1.0f);
	array3(this->options->attack.id, GLFW_MOUSE_BUTTON_LEFT, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_RIGHT_CONTROL);
	array3(this->options->spell1.id, GLFW_KEY_F1, GLFW_KEY_1, -1.0f);
	array3(this->options->spell2.id, GLFW_KEY_F2, GLFW_KEY_2, -1.0f);
	array3(this->options->spell3.id, GLFW_KEY_F3, GLFW_KEY_3, -1.0f);
	array3(this->options->spell4.id, GLFW_KEY_F4, GLFW_KEY_4, -1.0f);
	array3(this->options->menu.id, GLFW_KEY_ESCAPE, GLFW_KEY_PAUSE, -1);
}

void loadDefaultOptions(GameInstance *this) {
	if (access("data/options.dat", R_OK) != -1)
		return;

	FILE *file = fopen("data/options.dat", "wb");

	setOptionsDefaults(this);

	InputActionWrapper *setting[] = {
			&this->options->moveLeft,
			&this->options->moveRight,
			&this->options->jump,
			&this->options->sneek,
			&this->options->attack,
			&this->options->spell1,
			&this->options->spell2,
			&this->options->spell3,
			&this->options->spell4,
			&this->options->menu,
	};

	int optionVersion = CURRENT_OPTIONS_VERSION;
	fwrite(&optionVersion, sizeof(int), 1, file);

	int i;
	for (i = 0; i < 10; ++i)
		fwrite(setting[i]->id, sizeof(GLint), 3, file);

	fclose(file);
}

void loadOptions(GameInstance *this) {

	FILE *file = fopen("data/options.dat", "rb");
	if (file == NULL) {
		ERROR("Failed to load settings file");
		return;
	}

	InputActionWrapper *setting[] = {
			&this->options->moveLeft,
			&this->options->moveRight,
			&this->options->jump,
			&this->options->sneek,
			&this->options->attack,
			&this->options->spell1,
			&this->options->spell2,
			&this->options->spell3,
			&this->options->spell4,
			&this->options->menu,
	};

	int optionVersion = 0;
	fread(&optionVersion, sizeof(int), 1, file);

	if (optionVersion != CURRENT_OPTIONS_VERSION) {
		ERROR("Invalid options version");
		setOptionsDefaults(this);
		return;
	}

	int i;
	for (i = 0; i < 10; ++i)
		fread(setting[i]->id, sizeof(GLint), 3, file);

	fclose(file);
}

void saveOptions(GameInstance *this) {
	FILE *file = fopen("data/options.dat", "wb");
	if (file == NULL) {
		ERROR("Failed to load settings file");
		return;
	}
	DEBUG("Options", "Saving options");

	InputActionWrapper *setting[] = {
			&this->options->moveLeft,
			&this->options->moveRight,
			&this->options->jump,
			&this->options->sneek,
			&this->options->attack,
			&this->options->spell1,
			&this->options->spell2,
			&this->options->spell3,
			&this->options->spell4,
			&this->options->menu,
	};

	int optionVersion = CURRENT_OPTIONS_VERSION;
	fwrite(&optionVersion, sizeof(int), 1, file);

	int i;
	for (i = 0; i < 10; ++i)
		fwrite(setting[i]->id, sizeof(GLint), 3, file);

	fclose(file);
}

void updateControlsKey(unsigned int key, GameInstance* this) {
	if (this->options->selectedToSet != NULL) {
		key = toupper(key);

		int id = 0, i = 0;
		while (this->options->selectedToSet->text->text[i] != '\0') {
			if (this->options->selectedToSet->text->text[i] == ' ')
				++id;
			++i;
		}

		if (equals(this->options->selectedToSet->text->text, "[-]"))
			id = 0;

		if (equals(this->options->selectedToSet->value->value, "moveLeft"))
			this->options->moveLeft.id[0] = key;
		else if (equals(this->options->selectedToSet->value->value, "moveRight"))
			this->options->moveRight.id[0] = key;
		else if (equals(this->options->selectedToSet->value->value, "jump"))
			this->options->jump.id[0] = key;
		else if (equals(this->options->selectedToSet->value->value, "sneek"))
			this->options->sneek.id[0] = key;
		else if (equals(this->options->selectedToSet->value->value, "attack"))
			this->options->attack.id[0] = key;
		else if (equals(this->options->selectedToSet->value->value, "spell1"))
			this->options->spell1.id[0] = key;
		else if (equals(this->options->selectedToSet->value->value, "spell2"))
			this->options->spell2.id[0] = key;
		else if (equals(this->options->selectedToSet->value->value, "spell3"))
			this->options->spell3.id[0] = key;
		else if (equals(this->options->selectedToSet->value->value, "spell4"))
			this->options->spell4.id[0] = key;
		else if (equals(this->options->selectedToSet->value->value, "menu"))
			this->options->menu.id[0] = key;

		char str[12], finalStr[37];
		getOptionCaption(this, this->options->selectedToSet->value->value, str, 0);
		strcpy(finalStr, str);
		getOptionCaption(this, this->options->selectedToSet->value->value, str, 1);
		if (!equals(str, "N/A")) {
			strcat(finalStr, " ");
			strcat(finalStr, str);
			getOptionCaption(this, this->options->selectedToSet->value->value, str, 2);
			if (!equals(str, "N/A")) {
				strcat(finalStr, " ");
				strcat(finalStr, str);
			}
		}

		strcpy(this->options->selectedToSet->text->text, finalStr);
		this->options->selectedToSet = NULL;
	}
}

void updateControlsMods(GameInstance* this, int mods, int key) {
	if (this->options->selectedToSet != NULL) {

		int id = 1, i = 0;
		while (this->options->selectedToSet->text->text[i] != '\0') {
			if (this->options->selectedToSet->text->text[i] == ' ')
				++id;
			++i;
		}

		if (equals(this->options->selectedToSet->text->text, "[-]"))
			id = 0;

		if (equals(this->options->selectedToSet->value->value, "moveLeft"))
			this->options->moveLeft.id[id] = key;
		else if (equals(this->options->selectedToSet->value->value, "moveRight"))
			this->options->moveRight.id[id] = key;
		else if (equals(this->options->selectedToSet->value->value, "jump"))
			this->options->jump.id[id] = key;
		else if (equals(this->options->selectedToSet->value->value, "sneek"))
			this->options->sneek.id[id] = key;
		else if (equals(this->options->selectedToSet->value->value, "attack"))
			this->options->attack.id[id] = key;
		else if (equals(this->options->selectedToSet->value->value, "spell1"))
			this->options->spell1.id[id] = key;
		else if (equals(this->options->selectedToSet->value->value, "spell2"))
			this->options->spell2.id[id] = key;
		else if (equals(this->options->selectedToSet->value->value, "spell3"))
			this->options->spell3.id[id] = key;
		else if (equals(this->options->selectedToSet->value->value, "spell4"))
			this->options->spell4.id[id] = key;
		else if (equals(this->options->selectedToSet->value->value, "menu"))
			this->options->menu.id[id] = key;

		char str[12], finalStr[37];
		getOptionCaption(this, this->options->selectedToSet->value->value, str, 0);
		strcpy(finalStr, str);
		getOptionCaption(this, this->options->selectedToSet->value->value, str, 1);
		if (!equals(str, "N/A")) {
			strcat(finalStr, " ");
			strcat(finalStr, str);
			getOptionCaption(this, this->options->selectedToSet->value->value, str, 2);
			if (!equals(str, "N/A")) {
				strcat(finalStr, " ");
				strcat(finalStr, str);
			}
		}

		strcpy(this->options->selectedToSet->text->text, finalStr);
		this->options->selectedToSet = NULL;
	}
}
