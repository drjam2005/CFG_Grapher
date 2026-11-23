#include "../vendor/raylib/include/raylib.h"
#include "../include/plug.h"
#include <stdio.h>

int main(int argc, char** argv) {
	if(argc != 3) {
		fprintf(stderr, "Usage: %s <non-terminals.txt> <rules.txt>\n", argv[0]);
		return 1;
	}

	SetTraceLogLevel(LOG_NONE);
	InitWindow(800, 600, "CFG Grapher");
	SetTargetFPS(60);
	Plug plug = {0};

	plug_reload(&plug);
	plug.plug_init(argv[1], argv[2]);

	while(!WindowShouldClose()) {
		if(IsKeyPressed(KEY_R) && !IsKeyDown(KEY_LEFT_CONTROL)) {
			void* data = plug.plug_pre_reload();
			plug_reload(&plug);
			plug.plug_post_reload(data);
		}
		if(IsKeyPressed(KEY_R) && IsKeyDown(KEY_LEFT_CONTROL)) {
			plug.plug_init(argv[1], argv[2]);
		}

		BeginDrawing();

		plug.plug_update();
		plug.plug_render();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
