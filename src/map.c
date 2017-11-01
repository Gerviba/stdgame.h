#include <stdio.h>
#include <string.h>
#include "includes.h"

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
	Map *map = malloc(sizeof(Map));
	map->tiles = newLinkedListPointer(sizeof(Tile));
	map->lights = newLinkedListPointer(sizeof(Light));
	map->textures = newLinkedListPointer(sizeof(Texture));
	map->textureBlocks = newLinkedListPointer(sizeof(TextureBlock));

	map->objects = malloc(sizeof(ObjectInfo));
	map->objects->staticObjects = newLinkedListPointer(sizeof(StaticObject));
	map->objects->staticInstances = newLinkedListPointer(sizeof(StaticObjectInstance));
	map->objects->dynamicObjects = newLinkedListPointer(sizeof(DynamicObject));
	map->objects->dynamicInstances = newLinkedListPointer(sizeof(DynamicObjectInstance));
	map->objects->activeObjects = newLinkedListPointer(sizeof(ActiveObject));
	map->objects->activeInstances = newLinkedListPointer(sizeof(ActiveObjectInstance));

	FILE *file;
	char buff[255];
	printf("[Map] Loading map: %s\n", path);

	file = fopen(path, "r");
	while (fgets(buff, 255, file)) {
		switch (buff[0]) {
			case '$': { // Meta
				char type[32];
				sscanf(buff, "$ %s", type);
				if (strcmp(type, "NAME") == 0) {
					sscanf(buff, "$ %*s %s", map->name);
				} else if (strcmp(type, "AUTHOR") == 0) {
					sscanf(buff, "$ %*s %s", map->author);
				} else if (strcmp(type, "DESCRIPTION") == 0) {
					sscanf(buff, "$ %*s %s", map->description);
					char *p;
					for (p = map->description; *p != '\0'; ++p)
						if (*p == '_')
							*p = ' ';
//				} else if (strcmp(type, "BACKGROUND") == 0) {
//					unsigned int r, g, b;
//					sscanf(buff, "$ %*s %02x%02x%02x", &r, &g, &b);
//					map->background[0] = (float)r / 255;
//					map->background[1] = (float)g / 255;
//					map->background[2] = (float)b / 255;
				} else if (strcmp(type, "AMBIENT") == 0) {
					sscanf(buff, "$ %*s %f", &map->ambient[0]);
					map->ambient[1] = map->ambient[0];
					map->ambient[2] = map->ambient[0];
				} else if (strcmp(type, "SPAWN") == 0) {
					sscanf(buff, "$ %*s %f %f %f", &map->spawn[0], &map->spawn[1], &map->spawn[2]);
					printf("%f %f %f\n", map->spawn[0], map->spawn[1], map->spawn[2]);
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
				sscanf(buff, "S %f %f %f %02x%02x%02x", &light.x, &light.y, &light.strength, &r, &g, &b);
				light.color[0] = (float)r / 255;
				light.color[1] = (float)g / 255;
				light.color[2] = (float)b / 255;

				listPush(map->lights, &light);
				break;
			}
			case 'O': {
				break;
			}
			case 'I': {
				char type[32];
				sscanf(buff, "I %*d %s", type);
				int visible;

				if (strcmp(buff, "STATIC") == 0) {
					StaticObjectInstance soi;
					int objectId;
					sscanf(buff, "I %d %d STATIC %f %f %f %f %f %f %f %f %f %d", &soi.id, objectId,
							&soi.position[0], &soi.position[1], &soi.position[2],
							&soi.rotation[0], &soi.rotation[1], &soi.rotation[2],
							&soi.scale[0], &soi.scale[1], &soi.scale[2], &visible);
					soi.visible = visible ? GL_TRUE : GL_FALSE;

					ListElement *it;
					for (it = map->objects->staticInstances; it != NULL; it = it->next) {
						if (((StaticObjectInstance *)it->data)->id == objectId) {
							soi.object = ((StaticObjectInstance *) it->data)->object;
							break;
						}
					}

					listPush(map->objects->staticInstances, &soi);

				} else if (strcmp(buff, "DYNAMIC") == 0) {
					DynamicObjectInstance doi;
					int objectId, referencePoint;
					sscanf(buff, "I %d %d STATIC %f %f %f %f %f %f %f %f %f %d %d", &doi.id,
							&doi.position[0], &doi.position[1], &doi.position[2],
							&doi.rotation[0], &doi.rotation[1], &doi.rotation[2],
							&doi.scale[0], &doi.scale[1], &doi.scale[2], &visible, &referencePoint);
					doi.visible = visible ? GL_TRUE : GL_FALSE;

					ListElement *it;
					for (it = map->objects->dynamicInstances; it != NULL; it = it->next) {
						if (((DynamicObjectInstance *)it->data)->id == objectId) {
							doi.object = ((DynamicObjectInstance *) it->data)->object;
							break;
						}
					}

					doi.reference = NULL; //TODO: Implement reference points

					listPush(map->objects->dynamicInstances, &doi);
				}



				break;
			}
//			case 'C': {
//				char type[32];
//				sscanf(buff, "C %*d %s", type);
//
//				if (strcmp(type, "SPAWN") == 0) {
//					sscanf("C %*d SPAWN %f %f", &map->spawn[0], &map->spawn[1]);
//					map->spawn[2] = 4;
//					printf("Map loaded %f %f\n", map->spawn[0], map->spawn[1]);
//				}
//				break;
//			}
		}
	}

	fclose(file);

//	ListElement *it;
//	for (it = map->textures->first; it != NULL; it = it->next) {
//		int f = ((Texture *)it->data)->textureId;
//		printf("%d => %d\n", ((Texture *)it->data)->id, f);
//	}

	return map;
}


