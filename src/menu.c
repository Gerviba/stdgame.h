#include <stdlib.h>

#include "stdgame.h"

void enableOptions(GameInstance *this, Menu *menu) {
//	this->camera->destinationRotation[Y] = 90.0f;
}

void loadIngameGUI() {
	Menu *menu = new(Menu);
}

void renderIngameGUI(GameInstance* this) {
	GLfloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	renderFontTo(this, "Test message, $ *", (GLfloat[]) {6.0f, 7.0625f, 1.0f}, color, FS_NORMAL_DPI);

}

