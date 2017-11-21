#include <stdlib.h>

#include "player.h"
#include "stdgame.h"

void initPlayer(GameInstance *this) {
	this->player = new(Player);

	this->player->id = 0;
	this->player->position[X] = this->map->spawn[X];
	this->player->position[Y] = this->map->spawn[Y];
	this->player->position[Z] = 0;
	this->player->height = 1.3f;
	this->player->width = 0.5f;
	this->player->jumping = GL_FALSE;
	this->player->velocity[X] = 0;
	this->player->velocity[Y] = 0;
	this->player->velocity[Z] = 0;
	//TODO: Player model
}
