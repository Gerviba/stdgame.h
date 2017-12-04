/**
 * @file player.c
 * @author Gerviba (Szabo Gergely)
 * @brief Player specific types and functions
 *
 * @par Header:
 * 		player.h
 */

#include <stdlib.h>
#include "stdgame.h"

/**
 * Initialze player
 *
 * @param this Actual GameInstance instance
 */
void initPlayer(GameInstance *this) {
	if (this->player != NULL)
		free(this->player);
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
	this->player->item = -1;
}

/**
 * Free player
 *
 * XXX: free
 *
 * @param this Actual GameInstance instance
 */
void freePlayer(GameInstance *this) {
	free(this->player);
}
