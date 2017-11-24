/**
 * @file shader.h
 * @author Gerviba (Szabo Gergely)
 * @brief Shader file loader and compiler
 */

#ifndef SHADER_H_
#define SHADER_H_

/**
 * Load and attach shader program
 * @param program identifier
 * @param type Vertex or Fragment shader
 * @param filePath The shader file's path
 */
GLuint shaderAttachFromFile(GLuint program, GLenum type, const char *filePath);

/**
 * Load shader from file
 * @param filePath The shader file's path
 */
static char* shaderLoadSource(const char *filePath);

/**
 * Compile shader program
 * @param type Vertex or Fragment shader
 * @param filePath The shader file's path
 */
static GLuint shaderCompileFromFile(GLenum type, const char *filePath);

#endif /* SHADER_H_ */
