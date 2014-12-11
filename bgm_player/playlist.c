#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "list.h"

//TODO: allocation failed

static char **visited = NULL;
static size_t visited_size = 0;
static int visit(char *n) {
	size_t i;
	if (visited) {
		for (i = 0; i < visited_size; i++) {
			if (visited[i] && !strcmp(visited[i], n)) return 1;
		}
	}
	visited = realloc(visited, ++visited_size * sizeof(char*));
	char **to = &(visited[visited_size - 1]);
	*to = malloc((strlen(n) + 1) * sizeof(char));
	strcpy(*to, n);
	return 0;
}
static void clear_visited() {
	size_t i;
	if (visited) {
		for (i = 0; i < visited_size; i++) {
			if (visited[i]) free(visited[i]);
		}
		free(visited);
	}
	visited = NULL;
}

data_format *list = NULL;
size_t list_size = 0;
static void listing_do(char *);
static void list_add(char *n, size_t f, size_t t) {
	list = realloc(list, ++list_size * sizeof(data_format));
	list[list_size - 1].n = malloc((strlen(n) + 1) * sizeof(char));
	strcpy(list[list_size - 1].n, n);
	list[list_size - 1].f = f;
	list[list_size - 1].t = t;
}
static char *list_read_do(int input) {
	char *cur, *buf, *end;
	size_t s = 16;
	buf = cur = malloc(s);
	memset(buf, 0, s);
	end = buf + s;
	do {
		if (read(input, cur, 1) != 1) *cur = 0x0A;
		if (end < cur) {
			s <<= 1;
			end = realloc(buf, s);
			cur += end - buf;
			buf = end;
			end = buf + s;
		}
	} while (*cur++ != 0x0A);
	cur--;
	*cur = 0;
	return buf;
}
static inline char *dirname(char *n) {
	char *b = strrchr(n, '/');
	if (!b)
		return NULL;
	if (!(b - n))
		b++;
	char *new = malloc(1 + b - n);
	if (!new)
		return NULL;
	memcpy(new, n, b - n);
	n[b - n] = 0;
	return n;
}
static void list_read(int fd, char *n) {
	char *old = getcwd(NULL, 0);
	char *new = dirname(n);
	if (!new) {
		free(old);
		return;
	}
	if (chdir(new)) {
		free(new);
		free(old);
		return;
	}
	free(new);
	char _;
	if (1 > read(fd, &_, 1)) {
		free(old);
		return;
	}
	char *buf;
	while (1) {
		buf = list_read_do(fd);
		if (!strlen(buf)) {
			free(buf);
			break;
		}
		listing_do(buf);
		free(buf);
	}
	if (chdir(old)) {
		free(old);
		return;
	}
	free(old);
}
static void dir_read(char *n) {
	if (visit(n)) return;
	char *old = getcwd(NULL, 0);
	if (chdir(n)) {
		free(old);
		return;
	}
	DIR *d = opendir(".");
	if (d) {
		struct dirent *c;
		while ((c = readdir(d))) {
			if (strcmp(".", c->d_name) && strcmp("..", c->d_name))
				listing_do(c->d_name);
		}
		closedir(d);
	}
	if (chdir(old)) {
		free(old);
		return;
	}
	free(old);
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
static char* abspath(char *_) {
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
void read_cue(char *, size_t, size_t *, size_t *);
static void listing_do(char *o) {
	size_t from = 0, to = SIZE_MAX;
	char *c1 = strchr(o, ':'), *c2;
	char *n;
	if (c1 && (c2 = strchr(c1 + 1, ':'))) {
		char *b = malloc(1 + c1 - o);
		strncpy(b, o, c1 - o);
		b[c1 - o] = 0;
		read_cue(b, atoi(c2 + 1), &from, &to);
		free(b);
		b = malloc(c2 - c1);
		strncpy(b, c1 + 1, c2 - c1 - 1);
		b[c2 - c1 - 1] = 0;
		n = abspath(b);
		free(b);
	} else {
		n = abspath(o);
	}
	if (!n)
		return;
	int fd = open(n, O_RDONLY);
	if (fd != -1) {
		struct stat st;
		if (!fstat(fd, &st)) {
			if (S_ISDIR(st.st_mode)) dir_read(n);
			else {
				uint32_t fourcc;
				if (read(fd, &fourcc, 4) == 4) {
					if (fourcc == 0x46464952) list_add(n, from, to);
					else if (fourcc == 0x43614C66) list_add(n, from, to);
					else if (fourcc == 0x5453494C) list_read(fd, n);
				}
			}
		}
		close(fd);
	}
	free(n);
}
static int mycmp(const void *a, const void *b) {
	const data_format *aa = (const data_format *)a;
	const data_format *bb = (const data_format *)b;
	int buf;
	buf = strcmp(aa->n, bb->n);
	if (buf) return buf;
	buf = aa->f - bb->f;
	if (buf) return buf;
	buf = aa->t - bb->t;
	return buf;
}
static void list_remove(size_t pos) {
	if (list[pos].n) free(list[pos].n);
	if (pos + 1 < list_size) {
		memmove(list + pos, list + pos + 1,
			(list_size - pos - 1) * sizeof(data_format));
	}
	list = realloc(list, --list_size * sizeof(data_format));
}
void list_full_remove() {
	size_t i;
	if (list) {
		for (i = 0; i < list_size; i++) {
			if (list[i].n) free(list[i].n);
		}
		free(list);
	}
	list = NULL;
}
static void list_swap(size_t p1, size_t p2) {
	data_format t = list[p1];
	list[p1] = list[p2];
	list[p2] = t;
}
void list_shuffle() {
	srandom(time(NULL));
	size_t i, r;
	for (i = 0; i < list_size; i++) {
		r = random() % list_size;
		list_swap(i, r);
	}
}
int listing(char **n) {
	while (*n) {
		listing_do(*n++);
	}
	clear_visited();
	if (!list_size || !list) return 1;
	qsort(list, list_size, sizeof(data_format), mycmp);
	size_t cur = 1;
	while (cur < list_size) {
		if (!strcmp(list[cur - 1].n, list[cur].n)
				&& list[cur - 1].f == list[cur].f
				&& list[cur - 1].t == list[cur].t) {
			list_remove(cur);
		} else {
			cur++;
		}
	}
	return 0;
}
