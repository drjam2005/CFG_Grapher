#pragma once
#ifndef STRUCTS_H
#define STRUCTS_H

#include "../vendor/raylib/include/raylib.h"
#include <stddef.h>

typedef const char* Variable;
typedef const char* Terminal;
typedef enum {
	VARIABLE,
	TERMINAL,
	BOTH,
	HOWBRUH
} NODE_TYPE;

typedef struct {
	Variable var;
	const char* result;
} Rule;

typedef struct Node {
	NODE_TYPE type;
	union {
		Variable var;
		Terminal ter;
	};
	struct Node* nodes;
	size_t nodeCount;
} Node;

typedef struct {
	Variable var;
	Vector2 position;
} VariableGraphic;

#endif
