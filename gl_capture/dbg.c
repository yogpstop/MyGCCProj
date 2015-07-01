#include <stdio.h>
#include <windows.h>
#include "dbg.h"
#define DBG_PERF_MAX 32
typedef struct {
	LARGE_INTEGER *data;
	int row;
	int max;
	int cur;
} DBG_PERF_STR;
static DBG_PERF_STR str[DBG_PERF_MAX] = {};
void DBG_PERF_INIT(const int uid, const int row, const int max) {
	if (uid >= DBG_PERF_MAX || str[uid].data) return;
	str[uid].row = row;
	str[uid].max = max;
	int size = sizeof(LARGE_INTEGER) * row * max;
	str[uid].data = malloc(size);
	ZeroMemory(str[uid].data, size);
}
void DBG_PERF(const int uid, const int cp) {
	if (uid >= DBG_PERF_MAX || !str[uid].data || str[uid].max <= str[uid].cur || cp >= str[uid].row) return;
	QueryPerformanceCounter(str[uid].data + str[uid].cur * str[uid].row + cp);
}
void DBG_PERF_INC(const int uid) {
	if (uid >= DBG_PERF_MAX || !str[uid].data) return;
	str[uid].cur++;
}
void DBG_PERF_FLUSH() {
	int i;
	char fn[128];
	for (i = 0; i < DBG_PERF_MAX; i++) {
		if (!str[i].data) continue;
		sprintf(fn, "V:\\dbg_perf_%02d.dat", i);
		FILE *f = fopen(fn, "wb");
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		fwrite(&li, 1, sizeof(LARGE_INTEGER), f);
		fwrite(&str[i].row, 1, 1, f);
		int size = sizeof(LARGE_INTEGER) * str[i].row * str[i].max;
		fwrite(&size, 1, 4, f);
		fwrite(str[i].data, 1, size, f);
		fclose(f);
		free(str[i].data);
	}
}
