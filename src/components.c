#include "includes.h"

GenericType* newGenericValue(void *value, size_t size) {
	GenericType *gt = malloc(sizeof(GenericType));
	gt->valueLength = size;
	gt->value = malloc(gt->valueLength);
	int i;
	for (i = 0; i < gt->valueLength; ++i)
		*(char *)(gt->value + i) = *(char *)(value + i);
	return gt;
}

GenericType* newGenericIntValue(GLint intValue) {
	GenericType *gt = malloc(sizeof(GenericType));
	gt->valueLength = sizeof(GLint);
	gt->value = malloc(gt->valueLength);
	void *value = &intValue;
	int i;
	for (i = 0; i < gt->valueLength; ++i)
		*(char *)(gt->value + i) = *(char *)(value + i);
	return gt;
}

GenericType* newGenericFloatValue(GLfloat floatValue) {
	GenericType *gt = malloc(sizeof(GenericType));
	gt->valueLength = sizeof(GLfloat);
	gt->value = malloc(gt->valueLength);
	void *value = &floatValue;
	int i;
	for (i = 0; i < gt->valueLength; ++i)
		*(char *)(gt->value + i) = *(char *)(value + i);
	return gt;
}

void setGenericValue(GenericType *value, void *newValue, size_t newSize) {
	free(value->value);
	value->valueLength = newSize;
	value->value = malloc(value->valueLength);
	int i;
	for (i = 0; i < value->valueLength; ++i)
		*(char *)(value->value + i) = *(char *)(newValue + i);
}

void setGenericIntValue(GenericType *value, GLint newIntValue) {
	free(value->value);
	value->valueLength = sizeof(GLint);
	value->value = malloc(value->valueLength);
	void *newValue = &newIntValue;
	int i;
	for (i = 0; i < value->valueLength; ++i)
		*(char *)(value->value + i) = *(char *)(newValue + i);
}

void setGenericFloatValue(GenericType *value, GLfloat newFloatValue) {
	free(value->value);
	value->valueLength = sizeof(GLfloat);
	value->value = malloc(value->valueLength);
	void *newValue = &newFloatValue;
	int i;
	for (i = 0; i < value->valueLength; ++i)
		*(char *)(value->value + i) = *(char *)(newValue + i);
}

GLint getGenericIntValue(GenericType *value) {
	return *(GLint *)(*value).value;
}

GLfloat getGenericFloatValue(GenericType *value) {
	return *(GLfloat *)(*value).value;
}

void freeGenericValue(GenericType *value) {
	free(value->value);
	free(value);
}
