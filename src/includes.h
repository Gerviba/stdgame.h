#ifndef __INCLUDES_H__
#define __INCLUDES_H__

//#define DEBUG 1
static const float PI = 3.14159265358979323846;
static const int WINDOW_WIDTH = 1280;
static const int WINDOW_HEIGHT = 720;

#ifdef __linux__
	#include <GL/gl.h>
	#include <GL/glext.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
	#include "SOIL/SOIL.h"
#elif defined APPLE
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
	#include "SOIL/SOIL.h"
#else
	#include <gl.h>
	#include <glext.h>
	#include <glut.h>
	#include <glu.h>
	#include "SOIL.h"
#endif

#include "linkedlist.h"
#include "object.h"
#include "map.h"
#include "game.h"

#endif /* __INCLUDES_H__ */
