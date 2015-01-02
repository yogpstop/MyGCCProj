#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>

char *dirname(char *n) {
	char *b = strrchr(n, '/');
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
char* abspath(char *_) {
	char *a = malloc(strlen(_) + 1);
	strcpy(a, _);
	size_t rl = PATH_MAX;
	char *r = malloc(rl);
	r[0] = 0;
	if (a[0] != '/') {
		char *c = getcwd(NULL, 0);
		sstrcat(&r, &rl, c);
		free(c);
	}
	char *p, *s = a;
	struct stat st;
	while(1) {
		p = s;
		if (!p) break;
		s = strchr(p, '/');
		if (s) *s++ = 0;
		if(!strcmp(p, ".") || !strcmp(p, ""))
			continue;
		if(!strcmp(p, "..")) {
			char *l = strrchr(r, '/');
			if (l) *l = 0;
			continue;
		}
		sstrcat(&r, &rl, "/");
		sstrcat(&r, &rl, p);
		int ret = lstat(r, &st);
		if (ret || !S_ISLNK(st.st_mode)) continue;
		char *new = malloc(strlen(s) + st.st_size + 2);
		st.st_size = readlink(r, new, strlen(s) + st.st_size + 2);
		new[st.st_size] = '/';
		strcpy(new + st.st_size + 1, s);
		free(a);
		a = s = new;
		if (a[0] == '/') r[0] = 0;
		else *strrchr(r, '/') = 0;
	}
	free(a);
	return r;
}
