#ifndef MENU_H_
#define MENU_H_

typedef struct Menu {
	int id;
	LinkedList /*Component*/ *components;
	GLboolean useCursor;

//	void (*onEnable)(GameInstance*, struct Menu*);
//	void (*onDisable)(GameInstance*, struct  Menu*);
} Menu;

//#include "game.h"
//
//void onLogicIngame(GameInstance*, float);

#endif /* MENU_H_ */
