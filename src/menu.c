#include <stdlib.h>
#include "stdgame.h"

void renderIngameGUI(GameInstance* this) {
	GLfloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	renderFontTo(this, "Test message, $ *", (GLfloat[]) {6.0f, 7.0625f, 1.0f}, color, FS_NORMAL_DPI);

}

void onClickMenu(GameInstance* this) {
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

void onScrollMenu(GameInstance* this, double offset) {
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
