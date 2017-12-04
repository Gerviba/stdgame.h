/**
 * @file shader.c
 * @author Gerviba (Szabo Gergely)
 * @brief Shader file loader and compiler
 *
 * @par Header:
 * 		shader.h
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl.h>
#include <glext.h>
#include "stdgame.h"

/**
 * Load shader from file
 *
 * @param filePath The shader file's path
 */
static char* shaderLoadSource(const char *filePath) {
	const size_t blockSize = 512;
	FILE *file;
	char buff[blockSize];
	char *source = NULL;
	size_t temp, sourceLength = 0;

	file = fopen(filePath, "r");
	if (!file) {
		ERROR("Unable to open %s for reading", filePath);
		return NULL;
	}

	while ((temp = fread(buff, 1, blockSize, file)) > 0) {
		char *newSource = malloc(sourceLength + temp + 1);
		if (!newSource) {
			ERROR("Malloc failed at shaderLoadSource()");
			if (source)
				free(source);
			return NULL;
		}

		if (source) {
			memcpy(newSource, source, sourceLength);
			free(source);
		}
		memcpy(newSource + sourceLength, buff, temp);

		source = newSource;
		sourceLength += temp;
	}

	fclose(file);
	if (source)
		source[sourceLength] = '\0';

	return source;
}

/**
 * Compile shader program
 *
 * @param type Vertex or Fragment shader
 * @param filePath The shader file's path
 */
static GLuint shaderCompileFromFile(GLenum type, const char *filePath) {
	char *source;
	GLuint shader;
	GLint length, result;

	source = shaderLoadSource(filePath);
	if (!source)
		return 0;

	shader = glCreateShader(type);
	length = strlen(source);
	glShaderSource(shader, 1, (const char **) &source, &length);
	glCompileShader(shader);
	free(source);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		char *log;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		log = malloc(length);
		glGetShaderInfoLog(shader, length, &result, log);

		ERROR("Unable to compile shader program %s: %s", filePath, log);
		free(log);

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}


/**
 * Load and attach shader program
 *
 * @param program identifier
 * @param type Vertex or Fragment shader
 * @param filePath The shader file's path
 */
GLuint shaderAttachFromFile(GLuint program, GLenum type, const char *filePath) {
	GLuint shader = shaderCompileFromFile(type, filePath);
	if (shader != 0) {
		glAttachShader(program, shader);
		glDeleteShader(shader);
	}
	return shader;
}
