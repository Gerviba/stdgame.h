#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "includes.h"
#include "components.h"

extern void renderTextComponent(Component*, GameInstance*);
extern void calcTextButton(Component*, GameInstance*, ActiveObjectInstance*);
extern void calcObjectComponentPosition(Component*, GameInstance*, ActiveObjectInstance*);
extern void clickStartButton(Component*, GameInstance*);

void loadTexture(GLuint *textureId, char path[]) {
	glGenTextures(1, textureId);
	glBindTexture(GL_TEXTURE_2D, *textureId);

	char finalPath[255] = "assets/textures/";
	strcat(finalPath, path);
	printf("[Map] Loading texture: %s\n", finalPath);

	int width, height;
	unsigned char* image = SOIL_load_image(finalPath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Map* loadMap(const GameInstance *this, char path[]) {
	Map *map = new(Map);
	map->tiles = newList(Tile);
	map->lights = newList(Light);
	map->textures = newList(Texture);
	map->textureBlocks = newList(TextureBlock);

	map->objects = new(ObjectInfo);
	map->objects->staticObjects = newList(StaticObject);
	map->objects->staticInstances = newList(StaticObjectInstance);
	map->objects->dynamicObjects = newList(DynamicObject);
	map->objects->dynamicInstances = newList(DynamicObjectInstance);
	map->objects->activeObjects = newList(ActiveObject);
	map->objects->activeInstances = newList(ActiveObjectInstance);

	map->menu = NULL;

	FILE *file;
	char buff[255];
	printf("[Map] Loading map: %s\n", path);

	file = fopen(path, "r");
	while (fgets(buff, 255, file)) {
		switch (buff[X]) {
			case '$': { // Meta
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
					sscanf(buff, "$ %*s %f", &map->ambient[R]);
					map->ambient[G] = map->ambient[R];
					map->ambient[B] = map->ambient[R];
				} else if (equals(type, "SPAWN")) {
					sscanf(buff, "$ %*s %f %f %f", &map->spawn[X], &map->spawn[Y], &map->spawn[Z]);
				}
				break;
			}
			case 'X': { // Texture
				Texture texture;
				char path[255];

				sscanf(buff, "X %d %s", &texture.id, path);
				loadTexture(&texture.textureId, path);
				listPush(map->textures, &texture);
				break;
			}
			case 'Y': { // TextureBlock
				TextureBlock block;
				int side[5];

				sscanf(buff, "Y %d %d %d %d %d %d", &block.id, &side[0], &side[1], &side[2], &side[3], &side[4]);

				Texture **pointers[5] = {&block.base, &block.top, &block.right, &block.bottom, &block.left};
				int i;
				for (i = 0; i < 5; ++i) {
					*pointers[i] = NULL;
					if (side[i] < 0)
						continue;

					ListElement *it;
					for (it = map->textures->first; it != NULL; it = it->next) {
						if (((Texture *)it->data)->id == side[i]) {
							*pointers[i] = (Texture *)it->data;
							break;
						}
					}

					if (*pointers[i] == NULL)
						printf("[Map] Texture with id '%d' not found\n", side[i]);
				}

				listPush(map->textureBlocks, &block);
				break;
			}
			case 'T': { // Tile
				Tile tile;
				int texBlockId, tileType;

				sscanf(buff, "T %f %f %d %d", &tile.x, &tile.y, &texBlockId, &tileType);
				tile.type = (TileType) tileType;
				ListElement *it;
				for (it = map->textureBlocks->first; it != NULL; it = it->next) {
					if (((TextureBlock *)it->data)->id == texBlockId) {
						tile.texture = (TextureBlock *)it->data;
						break;
					}
				}

				listPush(map->tiles, &tile);
				break;
			}
			case 'S': { // Static Light
				Light light;
				unsigned int r, g, b;
				sscanf(buff, "S %f %f %f %f %02x%02x%02x", &light.position[X], &light.position[Y], &light.position[Z],
						&light.strength, &r, &g, &b);
				light.color[R] = (float) r / 255;
				light.color[G] = (float) g / 255;
				light.color[B] = (float) b / 255;

				listPush(map->lights, &light);
				break;
			}
			case 'O': { // Object
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

				} else if (equals(type, "DYNAMIC")) {
					char path[255];
					sscanf(buff, "O %d DYNAMIC %s", &id, path);
					char finalPath[255] = "assets/objects/";
					strcat(finalPath, path);

					DynamicObject *dobj = loadDynamicObject(finalPath);
					dobj->id = id;
					listPush(map->objects->dynamicObjects, dobj);

				} else if (equals(type, "ACTIVE")) {
					char path[255];
					sscanf(buff, "O %d ACTIVE %s", &id, path);
					char finalPath[255] = "assets/objects/";
					strcat(finalPath, path);

					ActiveObject *aobj = loadActiveObject(finalPath);
					aobj->id = id;
					listPush(map->objects->activeObjects, aobj);

				} else {
					printf("[Warn] Invalid object type: '%s'\n", type);
				}
				break;
			}
			case 'I': { // Object Instance
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

					ListElement *it;
					for (it = map->objects->staticObjects->first; it != NULL; it = it->next) {
						if (((StaticObject *) it->data)->id == objectId) {
							soi.object = (StaticObject *) it->data;
							break;
						}
					}
					initStraticInstance(this, &soi);
					listPush(map->objects->staticInstances, &soi);

				} else if (equals(type, "DYNAMIC")) {
					DynamicObjectInstance doi;
					int objectId, referencePoint;
					sscanf(buff, "I %d %d DYNAMIC %f %f %f %f %f %f %f %f %f %d %d", &doi.id, &objectId,
							&doi.position[X], &doi.position[Y], &doi.position[Z],
							&doi.rotation[X], &doi.rotation[Y], &doi.rotation[Z],
							&doi.scale[X], &doi.scale[Y], &doi.scale[Z], &visible, &referencePoint);
					doi.visible = visible ? GL_TRUE : GL_FALSE;

					ListElement *it;
					for (it = map->objects->dynamicObjects->first; it != NULL; it = it->next) {
						if (((DynamicObject *) it->data)->id == objectId) {
							doi.object = (DynamicObject *) it->data;
							break;
						}
					}

					doi.reference = NULL; //TODO: Implement reference points
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

					ListElement *it;
					for (it = map->objects->activeObjects->first; it != NULL; it = it->next) {
						if (((ActiveObject *) it->data)->id == objectId) {
							aoi.object = (ActiveObject *) it->data;
							break;
						}
					}

					listPush(map->objects->activeInstances, &aoi);
				}

				break;
			}
			case 'A': { // TextComponent
				if (map->menu == NULL) {
					map->menu = new(Menu);
					map->menu->components = newList(Component);
				}

				Component comp;
				comp.text = new(TextComponent);
				comp.type = CT_TEXT;
				char text[255];
				unsigned int r, g, b;
				sscanf(buff, "A %u %f %f %d %d %d %d %s %02x%02x%02x %f %d", &comp.id, &comp.position[X], &comp.position[Y],
						&comp.position[Z], &comp.relativeX, &comp.relativeY, &comp.text->align, text, &r, &g, &b,
						&comp.text->color[A], &comp.text->fontSize);
				comp.text->color[R] = (float) r / 255;
				comp.text->color[G] = (float) g / 255;
				comp.text->color[B] = (float) b / 255;
				comp.text->text = malloc(sizeof(char) * (strlen(text) + 1));
				strcpy(comp.text->text, text);

				int i, length;
				for (i = 0, length = strlen(comp.text->text); i < length; ++i)
					if (comp.text->text[i] == '_')
						comp.text->text[i] = ' ';

				comp.onRender = renderTextComponent;
				comp.onCalc = calcTextButton;
				comp.onClick = clickStartButton;

				listPush(map->menu->components, &comp);
				break;
			}
			case 'B': { // ObjectComponent
				if (map->menu == NULL) {
					map->menu = new(Menu);
					map->menu->components = newList(Component);
				}

				Component comp;
				comp.type = CT_OBJECT;
				comp.object = new(ObjectComponent);
				int objectId;
				sscanf(buff, "B %d %f %f %f %d %d %d", &comp.id, &comp.position[X], &comp.position[Y],
						&comp.position[Z], &comp.relativeX, &comp.relativeY, &objectId);

				ListElement *it;
				for (it = map->objects->activeInstances->first; it != NULL; it = it->next) {
					if (((ActiveObjectInstance *) it->data)->id == objectId) {
						comp.object->object = (ActiveObjectInstance *) it->data;
						break;
					}
				}

				comp.onRender = NULL;
				comp.onCalc = calcObjectComponentPosition;

				listPush(map->menu->components, &comp);
				break;
			}
		}
	}

	fclose(file);

	printf("[Map] Sucessfully loaded\n");
	return map;
}

void freeMap(Map map) {

}


