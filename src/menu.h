#ifndef MENU_H_
#define MENU_H_

struct Menu {
	int id;
	LinkedList /*Component*/ *components;
	GLboolean useCursor;
	GLfloat scrollMin;
	GLfloat scrollMax;
	GLfloat scrollOffset;

	void (*onClick)(GameInstance*);
	void (*onScroll)(GameInstance*, double);
};

/**
 *
 */
void renderIngameGUI(GameInstance* this);

void onClickMenu(GameInstance* this);

void onScrollMenu(GameInstance*, double);

#endif /* MENU_H_ */
