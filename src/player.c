#include <stdlib.h>
#include "stdgame.h"

void initPlayer(GameInstance *this) {
	this->player = new(Player);

	this->player->id = 0;
	setPosition(this->player->position, this->map->spawn[X], this->map->spawn[Y], 0.3);
	this->player->height = 1.25f;
	this->player->width = 0.5f;
	this->player->jump = 0;
	this->player->lastJump = glfwGetTime();
	setVelocity(this->player->velocity, 0.0f, 0.0f, 0.0f);
	this->player->sneek = GL_FALSE;
	this->player->leftSide = GL_FALSE;
}
