#ifndef SHADER_H_
#define SHADER_H_

/**
 * Load and attach shader program
 * \param program identifier
 * \param type Vertex or Fragment shader
 * \param filePath The shader file's path
 */
GLuint shaderAttachFromFile(GLuint program, GLenum type, const char *filePath);

#endif /* SHADER_H_ */
