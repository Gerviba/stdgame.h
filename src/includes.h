#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include <string.h>

//#define DEBUG 1
//#define DEBUG_MOVEMENT 1
static const float PI = 3.14159265358979323846;

#define X 0
#define Y 1
#define Z 2

#define R 0
#define G 1
#define B 2
#define A 3

#ifdef __linux__
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glu.h>
	#include <GLFW/glfw3.h>
	#include "SOIL/SOIL.h"
#elif defined APPLE
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>
	#include <GLFW/glfw3.h>
	#include "SOIL/SOIL.h"
#else
	#include <gl.h>
	#include <glext.h>
	#include <glu.h>
	#include <glfw3.h>
	#include "SOIL.h"
#endif

#include "linkedlist.h"
#include "font.h"
#include "object.h"
#include "map.h"
#include "components.h"
#include "game.h"

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)
#define sig(a) (a == 0 ? 0 : a > 0 ? 1 : -1)
#define equals(a, b) (strcmp(a, b) == 0)
#define new(x) (malloc(sizeof(x)))

#endif /* __INCLUDES_H__ */
