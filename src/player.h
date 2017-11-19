/**
 * @file character.h
 * @author Gerviba (Szabo Gergely)
 * @brief Player specific types and functions
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "stdgame.h"

/**
 * Player movement sate
 */
typedef enum {
	MS_STAY,
	MS_WALK,
	MS_JUMP,
	MS_SNEEK
} MovementState;

struct Player {
	int id;
	GLfloat position[3];
	GLfloat velocity[3];
	GLfloat height;
	GLfloat width;
	GLboolean jumping;
	ActiveObjectInstance *model;
};

void initPlayer(GameInstance *this);

#endif /* PLAYER_H_ */
