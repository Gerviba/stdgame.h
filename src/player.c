#include <stdlib.h>

#include "player.h"
#include "stdgame.h"

void initPlayer(GameInstance *this) {
	this->player = new(Player);

	this->player->id = 0;
	setPosition(this->player->position, this->map->spawn[X], this->map->spawn[Y], 0);
	this->player->height = 1.3f;
	this->player->width = 0.5f;
	this->player->jumping = GL_FALSE;
	setVelocity(this->player->velocity, 0.0f, 0.0f, 0.0f);
	//TODO: Player model
}
