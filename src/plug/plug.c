#include "../../include/plug.h"
#include "../../include/structs.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	Variable* variables;
	size_t variableCount;

	Rule* rules;
	size_t ruleCount;

	struct {
		Node* root;
		size_t level;
	} Tree;

} PlugData;

static PlugData* data = NULL;

static void free_data(PlugData* grammar) {
	if(grammar == NULL) return;

	for(size_t i = 0; i < grammar->variableCount; ++i)
		free((void*)grammar->variables[i]);
	for(size_t i = 0; i < grammar->ruleCount; ++i) {
		free((void*)grammar->rules[i].var);
		free((void*)grammar->rules[i].result);
	}
	free(grammar->variables);
	free(grammar->rules);
	free(grammar);
}

static char** load_lines(const char* path, int* count) {
	char* content = LoadFileText(path);
	if(content == NULL) return NULL;

	char** lines = LoadTextLines(content, count);
	UnloadFileText(content);
	return lines;
}

static char* skip_space(char* text) {
	while(isspace((unsigned char)*text)) ++text;
	return text;
}

static bool initialize_vars(PlugData* grammar, const char* path) {
	int count = 0;
	char** lines = load_lines(path, &count);
	if(lines == NULL) return false;

	grammar->variables = calloc(count, sizeof(*grammar->variables));
	bool loaded = count == 0 || grammar->variables != NULL;
	for(int i = 0; loaded && i < count; ++i) {
		char* start = skip_space(lines[i]);
		char* end = start + strlen(start);
		while(end > start && isspace((unsigned char)end[-1])) --end;
		if(end == start) continue;
		if(end - start >= 2 && *start == '"' && end[-1] == '"') {
			++start;
			--end;
		}
		char* variable = strndup(start, end - start);
		loaded = variable != NULL;
		if(loaded) grammar->variables[grammar->variableCount++] = variable;
	}
	UnloadTextLines(lines, count);


	return loaded;
}

static bool initialize_rules(PlugData* grammar, const char* path) {
	int count = 0;
	char** lines = load_lines(path, &count);
	if(lines == NULL) return false;

	grammar->rules = calloc(count, sizeof(*grammar->rules));
	bool loaded = count == 0 || grammar->rules != NULL;
	for(int i = 0; loaded && i < count; ++i) {
		char* var = skip_space(lines[i]);
		if(*var == '\0') continue;

		char* var_end = *var == '"' ? strchr(var + 1, '"') : NULL;
		char* arrow = var_end ? skip_space(var_end + 1) : NULL;
		char* result = arrow && strncmp(arrow, "->", 2) == 0 ? skip_space(arrow + 2) : NULL;
		char* result_end = result && *result == '"' ? strchr(result + 1, '"') : NULL;
		if(result_end == NULL) {
			loaded = false;
			break;
		}

		Rule* rule = &grammar->rules[grammar->ruleCount++];
		rule->var = strndup(var + 1, var_end - var - 1);
		rule->result = strndup(result + 1, result_end - result - 1);
		loaded = rule->var != NULL && rule->result != NULL;
	}
	UnloadTextLines(lines, count);
	return loaded;
}

void plug_init(const char* variablePath, const char* rulePath) {
	PlugData* next = calloc(1, sizeof(*next));
	if(next == NULL) return;

	if(!initialize_vars(next, variablePath) || !initialize_rules(next, rulePath)) {
		TraceLog(LOG_WARNING, "Could not load grammar from %s and %s", variablePath, rulePath);
		free_data(next);
		return;
	}

	free_data(data);
	data = next;
	Node* node = malloc(sizeof(Node));
	node->var = strdup(data->variables[0]);
	node->nodeCount = 0;
	node->nodes = NULL;
	node->type = VARIABLE;
	data->Tree.root = node;
}

void plug_update(void) {
	if(IsFileDropped()) {
		FilePathList list = LoadDroppedFiles();
		if(list.count == 2) plug_init(list.paths[0], list.paths[1]);
		UnloadDroppedFiles(list);
	}
}

void drawNode(Node* node, Vector2 pos) {
	if(CheckCollisionPointCircle(GetMousePosition(), pos, 25.0f))
		DrawCircleLinesEx(pos, 25.0f, 4.0f, WHITE);
	else
		DrawCircleLinesEx(pos, 25.0f, 1.0f, WHITE);
	Vector2 textDims = MeasureTextEx(GetFontDefault(), node->ter, 6.0f, 3.0f);
	DrawText(node->ter, pos.x - textDims.x/2.0f, pos.y - textDims.y/2.0f, 6.0f, WHITE);
}

void plug_render(void) {
	ClearBackground(BLACK);
	if(data == NULL) return;

	Vector2 center = {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};

	drawNode(data->Tree.root, center);

//	for(size_t i = 0; i < data->ruleCount; ++i) {
//		const char* var = TextFormat("%s -> ", data->rules[i].var);
//		DrawText(var, 20, 40 + 25*(int)i, 20, WHITE);
//		int x = MeasureText(var, 20);
//		const char* res = data->rules[i].result;
//
//		for(int j = 0; j < strlen(res); ++j) {
//			int charLen = MeasureText(TextFormat("%c", res[j]), 20);
//			DrawText(TextFormat("%c", res[j]), 20 + x, 40 + 25*(int)i, 20, ('A' <= res[j] && res[j] <= 'Z') ? GREEN : WHITE);
//			x += charLen + 2;
//		}
//		//DrawText(res, 20 + x, 40 + 25*(int)i, 20, WHITE);
//		for(int j = 0; j < data->variableCount; ++j) {
//			int index = TextFindIndex(res, data->variables[j]);
//			if(index == -1)
//				continue;
//
//		}
//	}

}

void* plug_pre_reload(void) {
	return data;
}

void plug_post_reload(void* givenData) {
	data = (PlugData*)givenData;
}
