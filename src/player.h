/**
 * @file player.h
 * @author Gerviba (Szabo Gergely)
 * @brief Player specific types and functions
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "stdgame.h"

/**
 * Player type
 */
struct Player {
	int id;
	GLfloat position[3];
	GLfloat velocity[3];
	GLfloat height;
	GLfloat width;
	GLuint jump;
	double lastJump;
	ActiveObjectInstance *model;
	GLboolean sneek;
	GLboolean leftSide;
};

/**
 * Initialise player instance
 */
void initPlayer(GameInstance *this);

#endif /* PLAYER_H_ */
