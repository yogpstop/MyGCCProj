#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef _WIN32
#define DIR_SEP '\\'
#define DIR_SEPS "\\"
#else
#define DIR_SEP '/'
#define DIR_SEPS "/"
#include <limits.h>
#include <sys/stat.h>
#endif

char *dirname(char *n) {
	char *b = strrchr(n, DIR_SEP);
	if (!b) return NULL;
	if (!(b - n)) b++;
	char *new = malloc(1 + b - n);
	if (!new) return NULL;
	memcpy(new, n, b - n);
	new[b - n] = 0;
	return new;
}
static inline void *srealloc(void *p, size_t s) {
	void *t = realloc(p, s);
	if (!t) free(p);
	return t;
}
static inline void sstrcat(char **d, size_t *dl, char *s) {
	if (strlen(*d) + strlen(s) + 1 > *dl)
		if(!(*d = srealloc(*d, *dl *= 2)))
			return;
	strcat(*d, s);
}
//FIXME can I use realpath?
char* abspath(char *rel_arg) {
	char *rel = malloc(strlen(rel_arg) + 1);
	strcpy(rel, rel_arg);
	size_t abs_len;
#if DIR_SEP != '/'
	abs_len = 0;
	while (rel[abs_len]) {
		if (rel[abs_len] == '/') rel[abs_len] = DIR_SEP;
		abs_len++;
	}
#endif
	abs_len = PATH_MAX;
	char *abs = malloc(abs_len);
	abs[0] = 0;
#ifdef _WIN32
	if (!rel[0] || rel[1] != ':' || rel[2] != DIR_SEP) {
#else
	if (rel[0] != DIR_SEP) {
#endif
		char *cwd = getcwd(NULL, 0);
		sstrcat(&abs, &abs_len, cwd);
		free(cwd);
	}
	char *cur, *next = rel;
#ifndef _WIN32
	struct stat sstat;
#endif
	while(1) {
		cur = next;
		if (!cur) break;
		next = strchr(cur, DIR_SEP);
		if (next) *next++ = 0;
		if(!strcmp(cur, ".") || !strcmp(cur, ""))
			continue;
		if(!strcmp(cur, "..")) {
			char *last = strrchr(abs, DIR_SEP);
			if (last) *last = 0;
			continue;
		}
#ifdef _WIN32
		if (abs[0])
#endif
		sstrcat(&abs, &abs_len, DIR_SEPS);
		sstrcat(&abs, &abs_len, cur);
#ifndef _WIN32 //FIXME Win32 readlink
		int ret = lstat(abs, &sstat);
		if (ret || !S_ISLNK(sstat.st_mode)) continue;
		char *new = malloc(strlen(next) + sstat.st_size + 2);
		sstat.st_size = readlink(abs, new, strlen(next) + sstat.st_size + 2);
		new[sstat.st_size] = DIR_SEP;
		strcpy(new + sstat.st_size + 1, next);
		free(rel);
		rel = next = new;
		if (rel[0] == DIR_SEP) abs[0] = 0; // If link target is abstract path
		else *strrchr(abs, DIR_SEP) = 0; // Remove last element
#endif
	}
	free(rel);
	return abs;
}
