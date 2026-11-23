#include "../../include/plug.h"
#ifdef RELOAD
#	include <dlfcn.h>
#	include <stdio.h>
#endif

#ifdef RELOAD
#define OPN(plug, lib) \
	if(plug->handle != NULL) \
		dlclose(plug->handle); \
	plug->handle = dlopen(lib, RTLD_NOW); \
	if(plug->handle == NULL) { \
		fprintf(stderr, "dlopen: %s\n", dlerror());	 \
		return false; \
	}

#define SYM(plug, sym) \
	plug->sym = dlsym(plug->handle, #sym); \
	if(plug->sym == NULL) { \
		fprintf(stderr, "dlsym: %s\n", dlerror());	 \
		return false; \
	}
#else
#define OPN(plug, lib) (void)0
#define SYM(plug, sym) plug->sym = sym;
#endif

bool plug_reload(Plug* plug) {
	OPN(plug, "./build/libplug.so");
	SYM(plug, plug_init);
	SYM(plug, plug_update);
	SYM(plug, plug_render);
	SYM(plug, plug_pre_reload);
	SYM(plug, plug_post_reload);
	return true;
}
