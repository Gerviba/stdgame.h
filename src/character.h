#ifndef CHARACTER_H_
#define CHARACTER_H_

#include "object.h"

typedef enum {
	MS_STAY,
	MS_WALK,
	MS_JUMP,
	MS_SNEEK
} MovingState;

typedef struct {
	int id;
	GLfloat position[3];
	GLfloat velocity[3];
	GLfloat height;
	GLfloat width;
	GLboolean jumping;
	ActiveObjectInstance *model;
} Player;

#endif /* CHARACTER_H_ */
