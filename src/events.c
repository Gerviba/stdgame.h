/**
 * @file events.c
 * @author Gerviba (Szabo Gergely)
 * @brief Event handler methods
 *
 * @par Header:
 * 		events.h
 */

#include "stdgame.h"

/**
 * GLFW Error Event
 *
 * @warning Don't call this directly. GLFW will manage it.
 *
 * @param error Error code
 * @param description The detailed description of the error
 */
void onErrorEvent(int error, const char* description) {
	ERROR("[Error] [%d] %s", error, description);
}

/**
 * GLFW Mouse click event
 *
 * @warning Don't call this directly. GLFW will manage it.
 *
 * @param window The active (clicked) window
 * @param button Mouse button identifier
 * @param action Action id
 * @param mods Modifiers
 */
void onClickEvent(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_RELEASE) {
		GameInstance *this = NULL;
		getGameInstance(&this);

		if (updateControllsMouse(this, button))
			return;
	}

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    	GameInstance *this = NULL;
		getGameInstance(&this);
		if (this->map->menu->onClick != NULL)
			this->map->menu->onClick(this);
    }

}

/**
 * GLFW Mouse wheel scroll event
 *
 * @warning Don't call this directly. GLFW will manage it.
 *
 * @param window The active window
 * @param xOffset Wheel x-dim offset
 * @param yOffset Wheel y-dim offset
 */
void onScrollEvent(GLFWwindow* window, double xOffset, double yOffset) {
	GameInstance *this = NULL;
	getGameInstance(&this);

	if (this->map->menu->onScroll != NULL)
		this->map->menu->onScroll(this, yOffset / 10);
}

/**
 * GLFW Keyboard mods down event
 *
 * @param window The active window
 * @param key Key code
 * @param mods Modifiers
 */
void onCharModEvent(GLFWwindow* window, unsigned int key, int mods) {
	GameInstance *this = NULL;
	getGameInstance(&this);
	updateControlsKey(this, key);
}

/**
 * GLFW Key down event
 *
 * @warning Don't call this directly. GLFW will manage it.
 *
 * @param window The active window
 * @param key Key code
 * @param scancode Scan code
 * @param action Action
 * @param mods Modifiers
 */
void onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	GameInstance *this = NULL;
	getGameInstance(&this);

	if (isActionPerformed(this, &this->options->menu)) {
		if (this->state == INGAME) {
			freeMap(this->map);
			this->map = loadMap(this, "assets/maps/main.menu");
			initPlayer(this);
			updateCamera(this);
		} else {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS && (mods & GLFW_MOD_SHIFT) != 0)
		glfwSetWindowShouldClose(window, GL_TRUE);

	updateControlsMods(this, mods, key);

	if (key == GLFW_KEY_H && action == GLFW_PRESS) {
		printf("%g %g\n", this->player->position[X], this->player->position[Y]);
	}
#ifdef DEBUG_MOVEMENT
	onDebugKeyPress(key, 0, 0);
#endif
}

#ifdef DEBUG_MOVEMENT
void onDebugKeyPress(const char key, int x, int y) {
	static int mode = 0;
	static int modelId = 0;
	GameInstance *gi = NULL;
	getGameInstance(&gi);

	if (mode == 0) {
		switch (key) {
		case 'A': gi->camera->rotation[1] += 2; break;
		case 'D': gi->camera->rotation[1] -= 2; break;
		case 'W': gi->camera->rotation[0] += 2; break;
		case 'S': gi->camera->rotation[0] -= 2; break;
		case 'Q': gi->camera->rotation[2] += 2; break;
		case 'E': gi->camera->rotation[2] -= 2; break;
		case 'J': gi->camera->position[0] -= 0.2; break;
		case 'L': gi->camera->position[0] += 0.2; break;
		case 'K': gi->camera->position[1] -= 0.2; break;
		case 'I': gi->camera->position[1] += 0.2; break;
		case 'U': gi->camera->position[2] -= 0.2; break;
		case 'O': gi->camera->position[2] += 0.2; break;

		case 'N': mode = 2; break;
		case 'M': mode = 1; break;

		default:
//			printf("Unregistered KEY: %d %c\n", key, key);
			break;
		}
	} else if (mode == 1) {
		StaticObjectInstance *obj;
		Iterator it;
		for (it = gi->map->objects->staticInstances->first; it != NULL; it = it->next) {
			if (((StaticObjectInstance *)it->data)->id == modelId) {
				obj = (StaticObjectInstance *)it->data;
				break;
			}
		}
		switch (key) {
		case 'S': obj->position[1] += 1.0f / 16; break;
		case 'W': obj->position[1] -= 1.0f / 16; break;
		case 'A': obj->position[0] += 1.0f / 16; break;
		case 'D': obj->position[0] -= 1.0f / 16; break;
		case 'Q': obj->position[2] += 1.0f / 16; break;
		case 'E': obj->position[2] -= 1.0f / 16; break;

		case 'F': obj->scale[0] -= 0.1; break;
		case 'H': obj->scale[0] += 0.1; break;
		case 'G': obj->scale[1] -= 0.1; break;
		case 'T': obj->scale[1] += 0.1; break;
		case 'R': obj->scale[2] -= 0.1; break;
		case 'Z': obj->scale[2] += 0.1; break;

		case 'J': obj->rotation[0] -= PI / 4; break;
		case 'L': obj->rotation[0] += PI / 4; break;
		case 'K': obj->rotation[1] -= PI / 4; break;
		case 'I': obj->rotation[1] += PI / 4; break;
		case 'U': obj->rotation[2] -= PI / 4; break;
		case 'O': obj->rotation[2] += PI / 4; break;

		case 'N': mode = 0; break;
		case 'M': mode = 2; break;
		case 'B': scanf("%d", &modelId); break;
		case 'V': printf("POS: %f %f %f\nROT: %f %f %f\nSCL: %f %f %f\n",
				obj->position[X], obj->position[Y], obj->position[Z],
				obj->rotation[X], obj->rotation[Y], obj->rotation[Z],
				obj->scale[X], obj->scale[Y], obj->scale[Z]);
		break;

		default:
//			printf("Unregistered KEY: %d %c\n", key, key);
			break;
		}
	}
}
#endif
