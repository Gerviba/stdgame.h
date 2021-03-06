/**
 * @file map.c
 * @author Gerviba (Szabo Gergely)
 * @brief Map and menu loader and utilities
 *
 * @par Header:
 * 		map.h
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "stdgame.h"

static void (*getAction(int id)) (Component*, GameInstance*);

static void initMapDefaults(Map* map);
static void initLists(Map* map);
static void initObjects(Map* map);
static void initMenu(Map* map);

static void processMeta(GameInstance *this, Map *map, char buff[255]);
static void processTexture(GameInstance *this, Map *map, char buff[255]);
static void processTextureBlock(GameInstance *this, Map *map, char buff[255]);
static void processTile(GameInstance *this, Map *map, char buff[255]);
static void processStaticLight(GameInstance *this, Map *map, char buff[255]);
static void processObject(GameInstance *this, Map *map, char buff[255]);
static void processObjectInstance(GameInstance *this, Map *map, char buff[255]);
static void processTextComponent(GameInstance *this, Map *map, char buff[255]);
static void processObjectComponent(GameInstance *this, Map *map, char buff[255]);
static void processAction(GameInstance *this, Map *map, char buff[255]);
static void processRegion(GameInstance *this, Map *map, char buff[255]);
static void processMessage(GameInstance *this, Map *map, char buff[255]);
static void processEntity(GameInstance *this, Map *map, char buff[255]);
static void processPhysics(GameInstance *this, Map *map, char buff[255]);

/**
 * Loads a texture
 *
 * @note It uses SOIL2
 *
 * @param textureId The texture id pointer
 * @param path Texture file path
 */
void loadTexture(GLuint *textureId, char path[]) {
	glGenTextures(1, textureId);
	glBindTexture(GL_TEXTURE_2D, *textureId);

	char finalPath[255] = "assets/textures/";
	strcat(finalPath, path);
	DEBUG("Map", "Loading texture: %s", finalPath);

	int width, height;
	unsigned char* image = SOIL_load_image(finalPath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

/**
 * Adds a TextComponent
 *
 * @param map Current map
 * @param text Caption
 * @param id Identifier
 * @param relX Relative X point
 * @param relY Relative Y point
 * @param align Alignment
 * @param x X coordinate
 * @param y Y coordinate
 * @param size Size of the font
 */
void addTextComponent(Map* map, char text[100], int id, RelativeX relX, RelativeY relY, Align align,
		GLfloat x, GLfloat y, FontSize size) {

	Component comp;
	comp.id = id;
	comp.value = NULL;
	comp.text = new(TextComponent);
	comp.type = CT_TEXT;
	comp.relativeX = relX;
	comp.relativeY = relY;
	comp.text->fontSize = size;
	setPosition(comp.position, x, y, 1);
	comp.text->align = align;
	setColor(comp.text->color, 1, 1, 1, 1);

	comp.text->text = (char*) malloc(sizeof(char) * (strlen(text) + 16));
	strcpy(comp.text->text, text);
	int i, length;
	for (i = 0, length = strlen(comp.text->text); i < length; ++i)
		if (comp.text->text[i] == '_')
			comp.text->text[i] = ' ';

	comp.onRender = renderTextComponent;
	listPush(map->menu->components, &comp);
}

/**
 * Adds a TextComponent
 *
 * @param map Current map
 * @param text Caption
 * @param id Identifier
 * @param relX Relative X point
 * @param relY Relative Y point
 * @param align Alignment
 * @param pos Coordinates (3D)
 * @param color Font default color (RGBA)
 * @param size Size of the font
 */
void addTextComponentColor(Map* map, char text[100], int id, RelativeX relX, RelativeY relY, Align align,
		GLfloat pos[3], GLfloat color[4], FontSize size) {

	Component comp;
	comp.id = id;
	comp.value = NULL;
	comp.text = new(TextComponent);
	comp.type = CT_TEXT;
	comp.relativeX = relX;
	comp.relativeY = relY;
	comp.text->fontSize = size;
	setPosition(comp.position, pos[X], pos[Y], pos[Z]);
	comp.text->align = align;
	setColor(comp.text->color, color[R], color[G], color[B], color[A]);

	comp.text->text = malloc(sizeof(char) * (strlen(text) + 1));
	strcpy(comp.text->text, text);
	int i, length;
	for (i = 0, length = strlen(comp.text->text); i < length; ++i)
		if (comp.text->text[i] == '_')
			comp.text->text[i] = ' ';

	comp.onRender = renderTextComponent;
	listPush(map->menu->components, &comp);
}

/**
 * Action method getter
 *
 * @see fileformats.md -> Component action types
 * @returns The selected function pointer
 */
static void (*getAction(int id)) (Component*, GameInstance*) {
	switch (id) {
	case 0: return NULL;
	case 1: return clickGameSelector;
	case 2: return clickOptions;
	case 21: return clickGraphicsSet;
	case 22: return clickControllsSet;
	case 3: return clickCredits;
	case 31: return clickOpenGithub;
	case 4: return clickBack;
	case 5: return clickStartButton;
	case -1: return clickExit;
	default: return NULL;
	}
}

/**
 * Copy a key name into the str value
 *
 * @param this Actual GameInstance instance
 * @param str This is where the name will be copied (The longest will be: 9 + '\0')
 * @param key Key id
 */
void getKeyName(GameInstance *this, char *str, int key) {
	if (key >= GLFW_KEY_APOSTROPHE && key <= GLFW_KEY_GRAVE_ACCENT) {
		char temp[2];
		sprintf(temp, "%c", key);
		strcpy(str, temp);
	} else {
		switch (key) {
		case GLFW_KEY_SPACE: strcpy(str, "SPACE"); break;
		case GLFW_KEY_UP: strcpy(str, "~"); break;
		case GLFW_KEY_DOWN: strcpy(str, "`"); break;
		case GLFW_KEY_LEFT: strcpy(str, "{"); break;
		case GLFW_KEY_RIGHT: strcpy(str, "}"); break;
		case GLFW_KEY_PAGE_UP: strcpy(str, "PAGE-UP"); break;
		case GLFW_KEY_PAGE_DOWN: strcpy(str, "PAGE-DOWN"); break;

		case GLFW_KEY_APOSTROPHE: strcpy(str, "'"); break;
		case GLFW_KEY_COMMA: strcpy(str, ","); break;
		case GLFW_KEY_PERIOD: strcpy(str, "."); break;
		case GLFW_KEY_TAB: strcpy(str, "TAB"); break;
		case GLFW_KEY_SLASH: strcpy(str, "/"); break;
		case GLFW_KEY_SEMICOLON: strcpy(str, ";"); break;
		case GLFW_KEY_BACKSLASH: strcpy(str, "\\"); break;
		case GLFW_KEY_BACKSPACE: strcpy(str, "BSPACE"); break;
		case GLFW_KEY_CAPS_LOCK: strcpy(str, "C-LOCK"); break;
		case GLFW_KEY_DELETE: strcpy(str, "DELETE"); break;
		case GLFW_KEY_END: strcpy(str, "END"); break;
		case GLFW_KEY_ENTER: strcpy(str, "ENTER"); break;
		case GLFW_KEY_EQUAL: strcpy(str, "EQUAL"); break;
		case GLFW_KEY_ESCAPE: strcpy(str, "ESC"); break;

		case GLFW_KEY_F1: strcpy(str, "F1"); break;
		case GLFW_KEY_F2: strcpy(str, "F2"); break;
		case GLFW_KEY_F3: strcpy(str, "F3"); break;
		case GLFW_KEY_F4: strcpy(str, "F4"); break;
		case GLFW_KEY_F5: strcpy(str, "F5"); break;
		case GLFW_KEY_F6: strcpy(str, "F6"); break;
		case GLFW_KEY_F7: strcpy(str, "F7"); break;
		case GLFW_KEY_F8: strcpy(str, "F8"); break;
		case GLFW_KEY_F9: strcpy(str, "F9"); break;
		case GLFW_KEY_F10: strcpy(str, "F10"); break;
		case GLFW_KEY_F11: strcpy(str, "F11"); break;
		case GLFW_KEY_F12: strcpy(str, "F12"); break;

		case GLFW_KEY_HOME: strcpy(str, "HOME"); break;
		case GLFW_KEY_INSERT: strcpy(str, "INSERT"); break;
		case GLFW_KEY_LEFT_ALT: strcpy(str, "L-ALT"); break;
		case GLFW_KEY_RIGHT_ALT: strcpy(str, "R-ALT"); break;
		case GLFW_KEY_LEFT_BRACKET: strcpy(str, "("); break;
		case GLFW_KEY_RIGHT_BRACKET: strcpy(str, ")"); break;
		case GLFW_KEY_LEFT_SHIFT: strcpy(str, "L-SHIFT"); break;
		case GLFW_KEY_RIGHT_SHIFT: strcpy(str, "R-SHIFT"); break;
		case GLFW_KEY_LEFT_CONTROL: strcpy(str, "L-CTRL"); break;
		case GLFW_KEY_RIGHT_CONTROL: strcpy(str, "R-CTRL"); break;

		case GLFW_KEY_KP_0: strcpy(str, "NUM-0"); break;
		case GLFW_KEY_KP_1: strcpy(str, "NUM-1"); break;
		case GLFW_KEY_KP_2: strcpy(str, "NUM-2"); break;
		case GLFW_KEY_KP_3: strcpy(str, "NUM-3"); break;
		case GLFW_KEY_KP_4: strcpy(str, "NUM-4"); break;
		case GLFW_KEY_KP_5: strcpy(str, "NUM-5"); break;
		case GLFW_KEY_KP_6: strcpy(str, "NUM-6"); break;
		case GLFW_KEY_KP_7: strcpy(str, "NUM-7"); break;
		case GLFW_KEY_KP_8: strcpy(str, "NUM-8"); break;
		case GLFW_KEY_KP_9: strcpy(str, "NUM-9"); break;
		case GLFW_KEY_KP_ADD: strcpy(str, "NUM-ADD"); break;
		case GLFW_KEY_KP_SUBTRACT: strcpy(str, "NUM-SUB"); break;
		case GLFW_KEY_KP_MULTIPLY: strcpy(str, "NUM-MUL"); break;
		case GLFW_KEY_KP_DIVIDE: strcpy(str, "NUM-DIV"); break;
		case GLFW_KEY_KP_ENTER: strcpy(str, "NUM-ENTER"); break;
		case GLFW_KEY_KP_EQUAL: strcpy(str, "NUM-EQUAL"); break;
		case GLFW_KEY_KP_DECIMAL: strcpy(str, "NUM-DEC"); break;

		case GLFW_KEY_MENU: strcpy(str, "MENU"); break;
		case GLFW_KEY_MINUS: strcpy(str, "-"); break;
		case GLFW_KEY_NUM_LOCK: strcpy(str, "NUM-LOCK"); break;
		case GLFW_KEY_PAUSE: strcpy(str, "PAUSE"); break;
		case GLFW_KEY_PRINT_SCREEN: strcpy(str, "PRNTSCR"); break;

		case GLFW_MOUSE_BUTTON_1: strcpy(str, "M-LEFT"); break;
		case GLFW_MOUSE_BUTTON_2: strcpy(str, "M-RIGHT"); break;
		case GLFW_MOUSE_BUTTON_3: strcpy(str, "M-MID"); break;
		case GLFW_MOUSE_BUTTON_4: strcpy(str, "M-4"); break;
		case GLFW_MOUSE_BUTTON_5: strcpy(str, "M-5"); break;
		case GLFW_MOUSE_BUTTON_6: strcpy(str, "M-6"); break;
		case GLFW_MOUSE_BUTTON_7: strcpy(str, "M-7"); break;
		case GLFW_MOUSE_BUTTON_8: strcpy(str, "M-8"); break;
		default: strcpy(str, "N/A"); break;
		}
	}
}

/**
 * Find and copy a key name into the str value by option name and index
 *
 * @param this Actual GameInstance instance
 * @param name Option name
 * @param str This is where the name will be copied (The longest will be: 9 + '\0')
 * @param id Index of the key (0-2)
 */
void getOptionCaption(GameInstance *this, char *name, char *str, int id) {
	if (equals(name, "moveLeft")) {
		getKeyName(this, str, this->options->moveLeft.id[id]);
	} else if (equals(name, "moveRight")) {
		getKeyName(this, str, this->options->moveRight.id[id]);
	} else if (equals(name, "jump")) {
		getKeyName(this, str, this->options->jump.id[id]);
	} else if (equals(name, "sneek")) {
		getKeyName(this, str, this->options->sneek.id[id]);
	} else if (equals(name, "attack")) {
		getKeyName(this, str, this->options->attack.id[id]);
	} else if (equals(name, "use")) {
		getKeyName(this, str, this->options->use.id[id]);
	} else if (equals(name, "spell1")) {
		getKeyName(this, str, this->options->spell1.id[id]);
	} else if (equals(name, "spell2")) {
		getKeyName(this, str, this->options->spell2.id[id]);
	} else if (equals(name, "spell3")) {
		getKeyName(this, str, this->options->spell3.id[id]);
	} else if (equals(name, "menu")) {
		getKeyName(this, str, this->options->menu.id[id]);
	}
}

/**
 * Initialize the default values of the map
 *
 * @param map The map
 */
static void initMapDefaults(Map* map) {
	map->score = 0;
	map->healt = 3.0f;
	map->allowMovement = GL_TRUE;
	map->startTime = time(NULL);
}

/**
 * Initialize linked lists
 *
 * @param map The map
 */
static void initLists(Map* map) {
	map->tiles = newList(Tile);
	map->lights = newList(Light);
	map->textures = newList(Texture);
	map->textureBlocks = newList(TextureBlock);
	map->actions = newList(Action);
	map->regions = newList(Region);
	map->messages = newList(Message);
	map->physics = newList(PhysicsArea);
	map->entities = newList(Entity);
}

/**
 * Initialize objects storage
 *
 * @param map The map
 */
static void initObjects(Map* map) {
	map->objects = new(ObjectInfo);
	map->objects->staticObjects = newList(StaticObject);
	map->objects->staticInstances = newList(StaticObjectInstance);
	map->objects->dynamicObjects = newList(DynamicObject);
	map->objects->dynamicInstances = newList(DynamicObjectInstance);
	map->objects->activeObjects = newList(ActiveObject);
	map->objects->activeInstances = newList(ActiveObjectInstance);
}

/**
 * Initialize menu
 *
 * @param map The map
 */
static void initMenu(Map* map) {
	map->menu = new(Menu);
	map->menu->components = newList(Component);
	map->menu->useCursor = GL_FALSE;
	map->menu->onClick = NULL;
	map->menu->onScroll = NULL;
	map->menu->scrollMin = 0;
	map->menu->scrollMax = 0;
	map->menu->scrollOffset = 0;
}

/**
 * Meta loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processMeta(GameInstance *this, Map *map, char buff[255]) {
	char type[32];
	sscanf(buff, "$ %s", type);
	if (equals(type, "NAME")) {
		sscanf(buff, "$ %*s %s", map->name);
	} else if (equals(type, "AUTHOR")) {
		sscanf(buff, "$ %*s %s", map->author);
	} else if (equals(type, "DESCRIPTION")) {
		sscanf(buff, "$ %*s %s", map->description);
		char *p;
		for (p = map->description; *p != '\0'; ++p)
			if (*p == '_')
				*p = ' ';
	} else if (equals(type, "AMBIENT")) {
		sscanf(buff, "$ %*s %f %f %f", &map->ambient[R], &map->ambient[G], &map->ambient[B]);
	} else if (equals(type, "SPAWN")) {
		sscanf(buff, "$ %*s %f %f %f", &map->spawn[X], &map->spawn[Y], &map->spawn[Z]);
	} else if (equals(type, "STATE")) {
		sscanf(buff, "$ %*s %d", (int*) &this->state);
		if (this->state == INGAME) {
			addTextComponent(map, "$$$", HEALT_COMPONENT_ID, X_LEFT, Y_TOP, ALIGN_LEFT,
					(1.0 / 16) * 21, -(1.0 / 16) * 14, FS_NORMAL_DPI);
			addTextComponent(map, "0 *", SCORE_COMPONENT_ID, X_RIGHT, Y_TOP, ALIGN_RIGHT,
					-(1.0 / 16) * 21, -(1.0 / 16) * 14, FS_NORMAL_DPI);
		}
		map->menu->onClick = this->state == INGAME ? NULL : onClickMenu;
	} else if (equals(type, "CURSOR")) {
		char value[6];
		map->menu->useCursor = sscanf(buff, "$ %*s %s", value) == 1 && equals(value, "true");
	} else if (equals(type, "SCROLL")) {
		char value[6];
		map->menu->onScroll = sscanf(buff, "$ %*s %s %f %f", value,
				&map->menu->scrollMin, &map->menu->scrollMax) == 3
				&& equals(value, "true") ? onScrollMenu : NULL;
	}
}

/**
 * Texture loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processTexture(GameInstance *this, Map *map, char buff[255]) {
	Texture texture;
	char path[255];

	sscanf(buff, "X %d %s", &texture.id, path);
	loadTexture(&texture.textureId, path);
	listPush(map->textures, &texture);
}

/**
 * TextureBlock loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processTextureBlock(GameInstance *this, Map *map, char buff[255]) {
	TextureBlock block;
	int side[5];

	sscanf(buff, "Y %d %d %d %d %d %d", &block.id, &side[0], &side[1], &side[2], &side[3], &side[4]);

	Texture **pointers[5] = {&block.base, &block.top, &block.right, &block.bottom, &block.left};
	int i;
	for (i = 0; i < 5; ++i) {
		*pointers[i] = NULL;
		if (side[i] < 0)
			continue;

		Iterator it;
		foreach (it, map->textures->first) {
			if (((Texture *)it->data)->id == side[i]) {
				*pointers[i] = (Texture *)it->data;
				break;
			}
		}

		if (*pointers[i] == NULL)
			printf("[Map] Texture with id '%d' not found\n", side[i]);
	}

	listPush(map->textureBlocks, &block);
}

/**
 * Tile loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processTile(GameInstance *this, Map *map, char buff[255]) {
	Tile tile;
	int texBlockId, tileType;

	sscanf(buff, "T %f %f %d %d", &tile.x, &tile.y, &texBlockId, &tileType);
	tile.type = (TileType) tileType;
	Iterator it;
	for (it = map->textureBlocks->first; it != NULL; it = it->next) {
		if (((TextureBlock *)it->data)->id == texBlockId) {
			tile.texture = (TextureBlock *)it->data;
			break;
		}
	}

	listPush(map->tiles, &tile);
}

/**
 * Light loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processStaticLight(GameInstance *this, Map *map, char buff[255]) {
	Light light;
	unsigned int r, g, b;
	int referencePoint, visible;
	sscanf(buff, "S %d %f %f %f %f %02x%02x%02x %f %f %d %d", &light.id,
			&light.position[X], &light.position[Y], &light.position[Z],
			&light.strength, &r, &g, &b, &light.specular, &light.intensity,
			&referencePoint, &visible);
	light.color[R] = (float) r / 255;
	light.color[G] = (float) g / 255;
	light.color[B] = (float) b / 255;
	light.visible = visible == 1;

	Iterator it;
	foreach (it, this->referencePoints->first) {
		if (((ReferencePoint *) it->data)->id == referencePoint) {
			light.reference = it->data;
			break;
		}
	}

	listPush(map->lights, &light);
}

/**
 * Object loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processObject(GameInstance *this, Map *map, char buff[255]) {
	char type[32];
	sscanf(buff, "O %*d %s", type);
	int id;

	if (equals(type, "STATIC")) {
		char path[255];
		sscanf(buff, "O %d STATIC %s", &id, path);
		char finalPath[255] = "assets/objects/";
		strcat(finalPath, path);

		StaticObject *sobj = loadStaticObject(finalPath);
		sobj->id = id;
		listPush(map->objects->staticObjects, sobj);
		free(sobj);

	} else if (equals(type, "DYNAMIC")) {
		char path[255];
		sscanf(buff, "O %d DYNAMIC %s", &id, path);
		char finalPath[255] = "assets/objects/";
		strcat(finalPath, path);

		DynamicObject *dobj = loadDynamicObject(finalPath);
		dobj->id = id;
		listPush(map->objects->dynamicObjects, dobj);
		free(dobj);

	} else if (equals(type, "ACTIVE")) {
		char path[255];
		sscanf(buff, "O %d ACTIVE %s", &id, path);
		char finalPath[255] = "assets/objects/";
		strcat(finalPath, path);

		ActiveObject *aobj = loadActiveObject(finalPath);
		aobj->id = id;
		listPush(map->objects->activeObjects, aobj);
		free(aobj);

	} else {
		WARNING("Invalid object type: '%s'", type);
	}
}

/**
 * ObjectInstance loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processObjectInstance(GameInstance *this, Map *map, char buff[255]) {
	char type[32];
	sscanf(buff, "I %*d %*d %s", type);
	int visible;

	if (equals(type, "STATIC")) {
		StaticObjectInstance soi;
		int objectId;
		sscanf(buff, "I %d %d STATIC %f %f %f %f %f %f %f %f %f %d", &soi.id, &objectId,
				&soi.position[X], &soi.position[Y], &soi.position[Z],
				&soi.rotation[X], &soi.rotation[Y], &soi.rotation[Z],
				&soi.scale[X], &soi.scale[Y], &soi.scale[Z], &visible);
		soi.visible = visible ? GL_TRUE : GL_FALSE;

		Iterator it;
		for (it = map->objects->staticObjects->first; it != NULL; it = it->next) {
			if (((StaticObject *) it->data)->id == objectId) {
				soi.object = (StaticObject *) it->data;
				break;
			}
		}
		initStraticInstance(&soi);
		listPush(map->objects->staticInstances, &soi);

	} else if (equals(type, "DYNAMIC")) {
		DynamicObjectInstance doi;
		int objectId, referencePoint;
		sscanf(buff, "I %d %d DYNAMIC %f %f %f %f %f %f %f %f %f %d %d", &doi.id, &objectId,
				&doi.position[X], &doi.position[Y], &doi.position[Z],
				&doi.rotation[X], &doi.rotation[Y], &doi.rotation[Z],
				&doi.scale[X], &doi.scale[Y], &doi.scale[Z], &visible, &referencePoint);
		doi.visible = visible ? GL_TRUE : GL_FALSE;

		Iterator it;
		foreach (it, map->objects->dynamicObjects->first) {
			if (((DynamicObject *) it->data)->id == objectId) {
				doi.object = it->data;
				break;
			}
		}

		foreach (it, this->referencePoints->first) {
			if (((ReferencePoint *) it->data)->id == referencePoint) {
				doi.reference = it->data;
				break;
			}
		}

		listPush(map->objects->dynamicInstances, &doi);

	} else if (equals(type, "ACTIVE")) {
		ActiveObjectInstance aoi;
		int objectId;
		sscanf(buff, "I %d %d ACTIVE %f %f %f %f %f %f %f %f %f %d", &aoi.id, &objectId,
				&aoi.position[X], &aoi.position[Y], &aoi.position[Z],
				&aoi.rotation[X], &aoi.rotation[Y], &aoi.rotation[Z],
				&aoi.scale[X], &aoi.scale[Y], &aoi.scale[Z], &visible);
		aoi.visible = visible ? GL_TRUE : GL_FALSE;
		aoi.activePart = 0;
		aoi.reference = (ReferencePoint *) this->referencePoints->first->data;

		Iterator it;
		foreach (it, map->objects->activeObjects->first) {
			if (((ActiveObject *) it->data)->id == objectId) {
				aoi.object = (ActiveObject *) it->data;
				break;
			}
		}

		listPush(map->objects->activeInstances, &aoi);

	} else {
		WARNING("Invalid object type: '%s'", type);
	}
}

/**
 * TextComponent loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processTextComponent(GameInstance *this, Map *map, char buff[255]) {
	Component comp;
	comp.text = new(TextComponent);
	comp.type = CT_TEXT;
	comp.value = NULL;

	char text[255];
	unsigned int r, g, b;
	int action;

	sscanf(buff, "A %u %f %f %f %d %d %d %s %02x%02x%02x %f %d %d", &comp.id,
			&comp.position[X], &comp.position[Y], &comp.position[Z],
			(int*) &comp.relativeX, (int*) &comp.relativeY, (int*) &comp.text->align,
			text, &r, &g, &b, &comp.text->baseColor[A], (int*) &comp.text->fontSize, &action);
	comp.text->baseColor[R] = (float) r / 255;
	comp.text->baseColor[G] = (float) g / 255;
	comp.text->baseColor[B] = (float) b / 255;
	setColor(comp.text->color,
			comp.text->baseColor[R],
			comp.text->baseColor[G],
			comp.text->baseColor[B],
			comp.text->baseColor[A]);
	comp.text->text = malloc(sizeof(char) * (strlen(text) + 1));
	strcpy(comp.text->text, text);

	int i, length;
	for (i = 0, length = strlen(comp.text->text); i < length; ++i)
		if (comp.text->text[i] == '_')
			comp.text->text[i] = ' ';

	comp.onRender = renderTextComponent;
	comp.onCalc = action == 21 ? calcOptionsGraphicsButton : calcTextButton;
	comp.onClick = getAction(action);

	if (action == 22) {
		comp.value = newGenericValue(comp.text->text, sizeof(char) * (strlen(comp.text->text) + 1));
		char str[12], finalStr[37];
		getOptionCaption(this, comp.text->text, str, 0);
		strcpy(finalStr, str);
		getOptionCaption(this, comp.text->text, str, 1);
		if (!equals(str, "N/A")) {
			strcat(finalStr, " ");
			strcat(finalStr, str);
			getOptionCaption(this, comp.text->text, str, 2);
			if (!equals(str, "N/A")) {
				strcat(finalStr, " ");
				strcat(finalStr, str);
			}
		}
		free(comp.text->text);
		comp.text->text = malloc(sizeof(char) * 37);
		strcpy(comp.text->text, finalStr);
	}

	listPush(map->menu->components, &comp);
}


/**
 * ObjectComponent loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processObjectComponent(GameInstance *this, Map *map, char buff[255]) {
	Component comp;
	comp.type = CT_OBJECT;
	comp.object = new(ObjectComponent);
	comp.value = NULL;

	int objectId;
	sscanf(buff, "B %d %f %f %f %d %d %d", &comp.id,
			&comp.position[X], &comp.position[Y], &comp.position[Z],
			(int*) &comp.relativeX, (int*) &comp.relativeY, &objectId);

	Iterator it;
	foreach (it, map->objects->activeInstances->first) {
		if (((ActiveObjectInstance *) it->data)->id == objectId) {
			comp.object->object = (ActiveObjectInstance *) it->data;
			break;
		}
	}

	comp.onRender = NULL;
	comp.onCalc = calcObjectComponentPosition;

	listPush(map->menu->components, &comp);
}

/**
 * Action loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processAction(GameInstance *this, Map *map, char buff[255]) {
	Action action;
	sscanf(buff, "N %d %d", &action.id, (int*) &action.type);

	if (action.type == ACTION_TELEPORT) {
		float coord[2];
		sscanf(buff, "N %*d %*d %f %f", &coord[X], &coord[Y]);
		action.value = newGenericValue(coord, sizeof(float) * 2);
	} else if (action.type == ACTION_DAMAGE || action.type == ACTION_ADD_SCORE) {
		int count;
		sscanf(buff, "N %*d %*d %d", &count);
		action.value = newGenericValue(&count, sizeof(int));
	} else if (action.type == ACTION_SET_DOBJ) {
		float data[12];
		sscanf(buff, "N %*d %*d %f %f %f %f %f %f %f %f %f %f %f %f",
				&data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6],
				&data[7], &data[8], &data[9], &data[10], &data[11]);
		action.value = newGenericValue(data, sizeof(float) * 12);
	} else if (action.type == ACTION_SET_AOBJ) {
		float data[11];
		sscanf(buff, "N %*d %*d %f %f %f %f %f %f %f %f %f %f %f",
				&data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6],
				&data[7], &data[8], &data[9], &data[10]);
		action.value = newGenericValue(data, sizeof(float) * 11);
	} else if (action.type == ACTION_SET_ITEM) {
		GLint itemId;
		sscanf(buff, "N %*d %*d %d", &itemId);
		action.value = newGenericValue(&itemId, sizeof(GLint));
	} else if (action.type == ACTION_SET_LIGHT) {
		float data[12];
		sscanf(buff, "N %*d %*d %f %f %f %f %f %f %f %f %f %f %f %f",
				&data[0], &data[1], &data[2], &data[3], &data[4], &data[5], &data[6],
				&data[7], &data[8], &data[9], &data[10], &data[11]);
		action.value = newGenericValue(data, sizeof(float) * 12);
	} else if (action.type == ACTION_OBJECT_PSX) {
		float data[4];
		sscanf(buff, "N %*d %*d %f %f %f %f", &data[0], &data[1], &data[2], &data[3]);
		action.value = newGenericValue(data, sizeof(float) * 4);
	} else if (action.type == ACTION_WIN || action.type == ACTION_LOSE) {
		action.value = NULL;
	}

	listPush(map->actions, &action);
}

/**
 * Region loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processRegion(GameInstance *this, Map *map, char buff[255]) {
	Region region;
	sscanf(buff, "R %f %f %f %f %d %d %d %d", &region.xMin, &region.yMin, &region.xMax, &region.yMax,
			&region.actionId, &region.maxUse, &region.itemReq, (int*) &region.notSneek);

	listPush(map->regions, &region);
}

/**
 * Message loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processMessage(GameInstance *this, Map *map, char buff[255]) {
	Message msg;
	unsigned int r, g, b;
	sscanf(buff, "M %f %f %f %02x%02x%02x %f %d %s",
			&msg.position.x, &msg.position.y, &msg.position.z, &r, &g, &b,
			&msg.color.a, (int*) &msg.size, msg.message);
	msg.color.r = (float) r / 255;
	msg.color.g = (float) g / 255;
	msg.color.b = (float) b / 255;

	int i = 0;
	while (msg.message[i] != '\0') {
		if (msg.message[i] == '_')
			msg.message[i] = ' ';
		++i;
	}

	listPush(map->messages, &msg);
}

/**
 * Entity loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processEntity(GameInstance *this, Map *map, char buff[255]) {
	Entity e;
	int objectId;
	sscanf(buff, "E %d %d %d %f %f %f %d %f %f", &e.id, &objectId, &e.lightId,
			&e.spellSpeed, &e.damage, &e.hp, &e.score, &e.fi0, &e.radius);

	Iterator it;
	foreach (it, map->objects->activeInstances->first) {
		if (((ActiveObjectInstance *) it->data)->id == objectId) {
			e.obj = it->data;
			break;
		}
	}

	foreach (it, this->referencePoints->first) {
		if (((ReferencePoint *) it->data)->id == ENTITY_FLOATING_REFERENCEPOINT_ID) {
			e.obj->reference = it->data;
			break;
		}
	}

	listPush(map->entities, &e);
}

/**
 * PhysicsArea loader processor
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param map The loading map
 * @param buff The current line
 */
static void processPhysics(GameInstance *this, Map *map, char buff[255]) {
	PhysicsArea pa;
	sscanf(buff, "P %d %f %f %d", &pa.id, &pa.x, &pa.y, (int *) &pa.enabled);

	listPush(map->physics, &pa);
}

/**
 * Loads a map or menu
 *
 * @see fileformats.md -> Map and Menu table
 *
 * @param this Actual GameInstance instance
 * @param path Map or menu file path
 */
Map* loadMap(GameInstance *this, char path[]) {
	Map *map = new(Map);
	initMapDefaults(map);
	initLists(map);
	initObjects(map);
	initMenu(map);

	this->state = MENU;
	FILE *file;
	char buff[255];
	DEBUG("Map", "Loading map: %s", path);

	file = fopen(path, "r");
	if (file == NULL) {
		ERROR("Failed to load map: %s", path);
		return NULL;
	}

	while (fgets(buff, 255, file)) {
		switch (buff[X]) {
			case '$': processMeta(this, map, buff); break;
			case 'X': processTexture(this, map, buff); break;
			case 'Y': processTextureBlock(this, map, buff); break;
			case 'T': processTile(this, map, buff); break;
			case 'S': processStaticLight(this, map, buff); break;
			case 'O': processObject(this, map, buff); break;
			case 'I': processObjectInstance(this, map, buff); break;
			case 'A': processTextComponent(this, map, buff); break;
			case 'B': processObjectComponent(this, map, buff); break;
			case 'N': processAction(this, map, buff); break;
			case 'R': processRegion(this, map, buff); break;
			case 'M': processMessage(this, map, buff); break;
			case 'E': processEntity(this, map, buff); break;
			case 'P': processPhysics(this, map, buff); break;
		}
	}

	if (this->state == INGAME) {
		Action lose;
		lose.id = ACTION_LOSE_ID;
		lose.type = ACTION_LOSE;
		lose.value = NULL;
		listPush(map->actions, &lose);

		map->spells = new(Spells);
		map->spells->spell1.id = -1;
		map->spells->spell1.lastUse = 0;
		map->spells->spell1.reloadTime = 3;
		map->spells->spell1.damage = 3;
		map->spells->spell1.speed = 0;
	} else {
		map->spells = NULL;
	}

	fclose(file);
	setPosition(this->camera->position, 0.0f, 0.0f, 0.0f);
	fixViewport(this);

	DEBUG("Map", "Successfully loaded");

	return map;
}

/**
 * Free the map
 *
 * @param map Map to free
 */
void freeMap(Map *map) {
	DEBUG("Map", "Cleaning map: %s", map->name);
	Iterator it;
	foreach (it, map->actions->first) {
		Action *a = it->data;
		if (a->value != NULL)
			freeGenericValue(a->value);
	}
	listFree(map->actions);
	free(map->actions);

	listFree(map->tiles);
	free(map->tiles);
	listFree(map->lights);
	free(map->lights);
	listFree(map->textures);
	free(map->textures);
	listFree(map->textureBlocks);
	free(map->textureBlocks);
	listFree(map->regions);
	free(map->regions);
	listFree(map->messages);
	free(map->messages);
	listFree(map->physics);
	free(map->physics);
	listFree(map->entities);
	free(map->entities);

	foreach (it, map->objects->staticObjects->first) {
		StaticObject *temp = it->data;
		listFree(temp->parts);
		free(temp->parts);
		listFree(temp->colors);
		free(temp->colors);
	}
	listFree(map->objects->staticObjects);
	free(map->objects->staticObjects);
	listFree(map->objects->staticInstances);
	free(map->objects->staticInstances);

	foreach (it, map->objects->dynamicObjects->first) {
		DynamicObject *temp = it->data;
		listFree(temp->parts);
		free(temp->parts);
		listFree(temp->colors);
		free(temp->colors);
	}
	listFree(map->objects->dynamicObjects);
	free(map->objects->dynamicObjects);
	listFree(map->objects->dynamicInstances);
	free(map->objects->dynamicInstances);

	foreach (it, map->objects->activeObjects->first) {
		ActiveObject *temp = it->data;
		int i;
		for (i = 0; i < temp->size; ++i) {
			listFree(temp->parts[i].parts);
			free(temp->parts[i].parts);
		}
		listFree(temp->parts[0].colors);
		free(temp->parts[0].colors);
		free(temp->parts);
	}
	listFree(map->objects->activeObjects);
	free(map->objects->activeObjects);
	listFree(map->objects->activeInstances);
	free(map->objects->activeInstances);

	free(map->objects);

	foreach (it, map->menu->components->first) {
		Component *temp = it->data;
		if (temp->type == CT_TEXT) {
			free(temp->text->text);
			free(temp->text);
		} else if (temp->type == CT_OBJECT) {
			free(temp->object);
		}
		if (temp->value != NULL)
			freeGenericValue(temp->value);
	}
	listFree(map->menu->components);
	free(map->menu->components);
	free(map->menu);
	if (map->spells != NULL)
		free(map->spells);

	free(map);
	DEBUG("Map", "Cleanup finished");
}

/**
 * Update highscores if necessary
 *
 * @param this Actual GameInstance instance
 * @param deltaT Completed time
 */
void updateHightScore(GameInstance *this, time_t deltaT) {
	LinkedList /*HighScoreValue*/ *lines = newList(HighScoreValue);

	FILE *file = fopen("data/records.dat", "r");
	if (file == NULL) {
		file = fopen("data/records.dat", "w");
		fprintf(file, "%s 0 %d\n", this->map->name, (int) deltaT);
		fprintf(file, "%s 1 %d\n", this->map->name, this->map->score);
		fclose(file);
		listFree(lines);
		free(lines);
		return;
	}

	char temp[100];
	int type, value;
	while (fscanf(file, "%s %d %d", temp, &type, &value) == 3) {
		HighScoreValue hsv;
		strcpy(hsv.name, temp);
		hsv.type = type;
		hsv.value = value;

		if (equals(hsv.name, this->map->name)) {
			if (type == 0 && value > deltaT)
				hsv.value = deltaT;
			else if (type == 1 && value < this->map->score)
				hsv.value = this->map->score;
		}

		listPush(lines, &hsv);
	}
	fclose(file);

	file = fopen("data/records.dat", "w");
	Iterator it;
	foreach (it, lines->first) {
		HighScoreValue *hsv = it->data;
		fprintf(file, "%s %d %d\n", hsv->name, hsv->type, hsv->value);
	}
	fclose(file);

	listFree(lines);
	free(lines);
	DEBUG("Hightscore", "Highscore saved");
}

