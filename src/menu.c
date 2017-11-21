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

//	char str[16];
//	if (glfwGetKey(this->window, GLFW_KEY_UP) == GLFW_PRESS)
//		++this->score;
//	sprintf(str, "%d *", this->score);
//
//	renderFontTo(this, str, (GLfloat[]) {
//			this->camera->position[X] + 2.0f - (strlen(str) * 0.0625f * 4),
//			this->camera->position[Y] + 1.0625f, 1.0f}, color, FS_NORMAL_DPI);
//	renderFontTo(this, "$$$", (GLfloat[]) {
//			this->camera->position[X] - 2.0f,
//			this->camera->position[Y] + 1.0625f,
//			1.0f}, color, FS_NORMAL_DPI);
}

void onLogicIngame(GameInstance *this, float delta) {
	DynamicObjectInstance *obj = ((DynamicObjectInstance *) this->map->objects->dynamicInstances->first->data);

	float deltaMoveX = 0;
	if (glfwGetKey(this->window, this->options->moveLeft[0]) == GLFW_PRESS
			|| glfwGetKey(this->window, this->options->moveLeft[1]) == GLFW_PRESS) {
		deltaMoveX += -delta * 2;
		obj->rotation[Y] = 0;
		this->camera->destinationRotation[Y] = 2;
	}

	if (glfwGetKey(this->window, this->options->moveRight[0]) == GLFW_PRESS
			|| glfwGetKey(this->window, this->options->moveRight[1]) == GLFW_PRESS) {
		deltaMoveX += delta * 2;
		obj->rotation[Y] = 180;
		this->camera->destinationRotation[Y] = -2;
	}

	if (!(glfwGetKey(this->window, this->options->moveLeft[0]) == GLFW_PRESS
			|| glfwGetKey(this->window, this->options->moveLeft[1]) == GLFW_PRESS) &&
			!(glfwGetKey(this->window, this->options->moveRight[0]) == GLFW_PRESS
			|| glfwGetKey(this->window, this->options->moveRight[1]) == GLFW_PRESS)) {
		this->camera->destinationRotation[Y] = 0;
	}

	Iterator it;
	for (it = this->map->tiles->first; it != NULL; it = it->next) {
		Tile *tile = (Tile *) it->data;
		if ((tile->type & MOVE_BLOCK_X) != 0 &&
				((int) tile->y == (int) this->player->position[Y] ||
					(int) tile->y == (int) (this->player->position[Y] + this->player->height)) &&
				((tile->x + 1 < this->player->position[X]
					&& tile->x + 1 >= this->player->position[X] + deltaMoveX) ||
				(tile->x >= this->player->position[X] + this->player->width
					&& tile->x < this->player->position[X] + deltaMoveX + this->player->width))) {

			deltaMoveX = 0;
			break;
		}
	}
	this->player->position[X] += deltaMoveX;

	if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS
			|| glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) {
		this->player->velocity[Y] = 10;
	}

	float deltaMoveY = this->player->velocity[Y] * 0.00981;
	this->player->velocity[Y] -= 0.5;
	if (this->player->velocity[Y] < -15)
		this->player->velocity[Y] = -15;


	for (it = this->map->tiles->first; it != NULL; it = it->next) {
		Tile *tile = (Tile *) it->data;
		if ((tile->type & MOVE_BLOCK_Y) != 0 &&
				((int) tile->x == (int) (this->player->position[X]) ||
				(int) tile->x == (int) (this->player->position[X] + this->player->width))) {

			if (tile->y + 1 > deltaMoveY + this->player->position[Y]
					&& tile->y + 1 <= this->player->position[Y]) {
				this->player->position[Y] = tile->y + 1;
				this->player->velocity[Y] = -0.00001;
				deltaMoveY = 0;
			} else if (tile->y <= deltaMoveY + this->player->position[Y] + this->player->height
					&& tile->y > this->player->position[Y] + this->player->height) {
				this->player->velocity[Y] = -0.00001;
				deltaMoveY = 0;
			}
		}
	}

	this->player->position[Y] += deltaMoveY;

#ifndef DEBUG_MOVEMENT
	this->camera->position[X] = this->player->position[X] + 2;
	this->camera->position[Y] = this->player->position[Y] + 1;
	this->camera->position[Z] = 5.2; //4.8
#endif

	// DEBUG
	obj->position[X] = this->player->position[X];
	obj->position[Y] = this->player->position[Y];
	// END DEBUG

}

void onLogicMenu(GameInstance *this, float delta) {
	this->camera->position[Z] = this->map->spawn[Z];

	ActiveObjectInstance *cursor = updateCursor(this, 1);
	if (cursor != NULL) {
		Iterator it;
		for (it = this->map->menu->components->first; it != NULL; it = it->next) {
			Component *comp = (Component *) it->data;
			if (comp->onCalc != NULL)
				comp->onCalc(comp, this, cursor);
		}
	}

}
