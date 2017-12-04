/**
 * @file player.h
 * @author Gerviba (Szabo Gergely)
 * @brief Player specific types and functions (header)
 *
 * @par Definition:
 * 		player.c
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <time.h>
#include "stdgame.h"

#define PLAYER_SPEED 2.3
#define PLAYER_JUMP 0.6

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
	GLint item;
};

/**
 * Spell type
 */
struct Spell {
	GLint id;
	DynamicObject *spellObj;
	DynamicObjectInstance *spellObjInstance;
	GLfloat speed;
	GLfloat damage;
	time_t lastUse;
	time_t reloadTime;
	Position position;
};

/**
 * Spells storage
 */
struct Spells {
	Spell spell1;
	Spell spell2;
	Spell spell3;

	LinkedList /*Spell*/ mobSpells;
};

void initPlayer(GameInstance *this);
void freePlayer(GameInstance *this);

#endif /* PLAYER_H_ */
