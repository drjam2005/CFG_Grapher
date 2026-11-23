#pragma once
#ifndef PLUG_H
#define PLUG_H

#include <stdbool.h>

typedef struct Plug {
	void* handle;

	void (*plug_init)(const char*, const char*);
	
	void (*plug_update)(void);
	void (*plug_render)(void);

	void* (*plug_pre_reload)(void);
	void (*plug_post_reload)(void*);
} Plug;

bool plug_reload(Plug* plug);

#ifndef RELOAD
void plug_init(const char* variablePath, const char* rulePath);
void plug_update(void);
void plug_render(void);
void* plug_pre_reload(void);
void plug_post_reload(void*);
#endif

#endif
