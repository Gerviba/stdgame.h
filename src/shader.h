/**
 * @file shader.h
 * @author Gerviba (Szabo Gergely)
 * @brief Shader file loader and compiler (header)
 *
 * @par Definition:
 * 		shader.c
 */

#ifndef SHADER_H_
#define SHADER_H_

GLuint shaderAttachFromFile(GLuint program, GLenum type, const char *filePath);

#endif /* SHADER_H_ */
