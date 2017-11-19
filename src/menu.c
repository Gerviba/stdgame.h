#include <stdlib.h>

#include "stdgame.h"

void enableOptions(GameInstance *this, Menu *menu) {
	this->camera->destinationRotation[Y] = 90.0f;
}

void loadIngameGUI() {
	Menu *menu = new(Menu);
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

	ListItem *it;
	for (it = this->map->tiles->first; it != NULL; it = it->next) {
		Tile *tile = (Tile *) it->data;
		if ((int) tile->y == (int) this->player->position[Y]
			  && (int) tile->x == (int) (deltaMoveX + this->player->position[X])) {
			if (tile->type != 0)
				deltaMoveX = 0;
			break;
		}
	}
	this->player->position[X] += deltaMoveX;

	if (this->player->velocity[Y] == 0 && (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS
			|| glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)) {
		this->player->velocity[Y] = 10;
		printf("jump");
	}

	float deltaMoveY = this->player->velocity[Y] * (9.81 / 1000);
	this->player->velocity[Y] -= 0.5;
	if (this->player->velocity[Y] < -8)
		this->player->velocity[Y] = -8;


	for (it = this->map->tiles->first; it != NULL; it = it->next) {
		Tile *tile = (Tile *) it->data;
		if ((int) tile->y == (int) (deltaMoveY + this->player->position[Y])
			  && (int) tile->x == (int) (this->player->position[X])) {
			if (tile->type != 0)
				deltaMoveY = 0;
			break;
		}
	}
	this->player->position[Y] += deltaMoveY;

//	ListElement *it;
//	for (it = this->map->tiles->first; it != NULL; it = it->next) {
//		Tile *tile = (Tile *)it->data;
//		if (tile->y >= this->player->position[Y] - 1 && tile->y <= this->player->position[Y] - 1
//			  && (int) tile->x == (int) (this->player->position[X] + 0.5)) {
//
//			if ((tile->type & MOVE_BLOCK_Y) != 0) {
//				this->player->velocity[Y] = 0;
////				this->player->position[Y] = (int) tile->y + 1;
//				deltaMoveY = 0;
//				printf("break\n");
//				break;
//			}
//		}
//	}
//	this->player->position[Y] += deltaMoveY;

#ifndef DEBUG_MOVEMENT
	this->camera->position[X] = this->player->position[X] + 2;
	this->camera->position[Y] = this->player->position[Y] + 1;
	this->camera->position[Z] = 4.8;
#endif

	// DEBUG
	obj->position[X] = this->player->position[X];
	obj->position[Y] = this->player->position[Y];
//	obj->rotation[Z] = 270;
	// END DEBUG

}

void onLogicMenu(GameInstance *this, float delta) {
	this->camera->position[Z] = this->map->spawn[Z];

	ActiveObjectInstance *cursor = updateCursor(this, 1);
	if (cursor != NULL) {
		ListItem *it;
		for (it = this->map->menu->components->first; it != NULL; it = it->next) {
			Component *comp = (Component *) it->data;
			if (comp->onCalc != NULL)
				comp->onCalc(comp, this, cursor);
		}
	}

}
