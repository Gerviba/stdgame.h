#ifndef EVENTS_H_
#define EVENTS_H_

#include "stdgame.h"

void onClickEvent(GLFWwindow* window, int button, int action, int mods);
void onScrollEvent(GLFWwindow* window, double xoffset, double yoffset);
void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
void onErrorEvent(int error, const char* description);
void onCharModEvent(GLFWwindow* window, unsigned int codepoint, int mods);

#ifdef DEBUG_MOVEMENT
void onDebugKeyPress(const char key, int x, int y);
#endif

#endif /* EVENTS_H_ */
