#include "includes.h"

void initPlayer(GameInstance *this) {
	this->player = malloc(sizeof(Player));

	this->player->id = 0;
	this->player->position[X] = this->map->spawn[X];
	this->player->position[Y] = this->map->spawn[Y];
	this->player->position[Z] = 0;
	this->player->height = 1.8;
	this->player->width = 0.8;
	this->player->jumping = GL_FALSE;
	this->player->velocity[X] = 0;
	this->player->velocity[Y] = 0;
	this->player->velocity[Z] = 0;
	//TODO: Player model
}
