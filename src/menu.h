#ifndef MENU_H_
#define MENU_H_

#define CURRENT_OPTIONS_VERSION 1

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

/**
 *
 */
void renderIngameGUI(GameInstance* this);

void onClickMenu(GameInstance* this);

void onScrollMenu(GameInstance*, GLfloat);

void loadDefaultOptions(GameInstance* this);

void loadOptions(GameInstance *this);

void saveOptions(GameInstance *this);

void updateControlsMods(GameInstance *this, int mods, int key);

void updateControlsKey(unsigned int key, GameInstance *this);

GLboolean updateControlsMouse(GameInstance *this, int button);

#endif /* MENU_H_ */
