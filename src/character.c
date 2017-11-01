#include "includes.h"

void initPlayer(GameInstance *this) {
	this->player = malloc(sizeof(Player));

	this->player->id = 0;
	this->player->position[0] = this->map->spawn[0];
	this->player->position[1] = this->map->spawn[1];
	printf("%f %f\n", this->player->position[0], this->player->position[1]);
	this->player->position[2] = 0;
	this->player->height = 1.8;
	this->player->width = 0.8;
	this->player->jumping = GL_FALSE;
	this->player->velocity[0] = 0;
	this->player->velocity[1] = 0;
	this->player->velocity[2] = 0;
	//TODO: Player model
}
