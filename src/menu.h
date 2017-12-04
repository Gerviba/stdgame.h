/**
 * @file menu.h
 * @author Gerviba (Szabo Gergely)
 * @brief Component, click and scroll manager (header)
 *
 * @par Definition:
 * 		menu.c
 */

#ifndef MENU_H_
#define MENU_H_

/** Used to determine the up-to-date status of the data/options.dat */
#define CURRENT_OPTIONS_VERSION 1

/**
 * Menu object
 */
struct Menu {
	int id;
	LinkedList /*Component*/ *components;
	GLboolean useCursor;
	GLfloat scrollMin;
	GLfloat scrollMax;
	GLfloat scrollOffset;

	void (*onClick)(GameInstance*);
	void (*onScroll)(GameInstance*, GLfloat);
};


void onClickMenu(GameInstance* this);
void onScrollMenu(GameInstance*, GLfloat);

void loadDefaultOptions(GameInstance *this);
void loadOptions(GameInstance *this);
void saveOptions(GameInstance *this);

void updateControlsMods(GameInstance *this, int mods, int key);
void updateControlsKey(GameInstance* this, unsigned int key);
GLboolean updateControllsMouse(GameInstance *this, int button);

#endif /* MENU_H_ */
