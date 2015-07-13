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

#include "main.h"

//TODO: allocation failed

static int df_cmp(const void *f1, const void *f2) {
	const data_format *d1 = f1; const data_format *d2 = f2;
	int r = strcmp(d1->n, d2->n);
	if (r) return r;
	if (d1->f != d2->f) return d1->f - d2->f;
	if (d1->t != d2->t) return d1->t - d2->t;
	return 0;
}
static int cpp_cmp(const void *f1, const void *f2) {
	return strcmp(*((const char**)f1), *((const char**)f2));
}
static ssize_t bspos(const void *key, const void *base,
		const size_t len, const size_t size,
		int (*compar)(const void*, const void*)) {
	size_t min = 0, max = len, idx; int diff;
	while (min < max) {
		idx = (min + max) >> 1;
		diff = compar(key, base + idx * size);
		if (diff < 0) max = idx;
		else if (diff > 0) min = idx + 1;
		else return -1;
	}
	if (max < len && compar(key, base + max * size) > 0) return max + 1;
	return max;
}

static int visit(char *add, char ***visited, size_t *visited_size) {
	ssize_t pos = bspos(&add, *visited, *visited_size, sizeof(char*), cpp_cmp);
	if (pos < 0) return 1;
	char **new = malloc(++*visited_size * sizeof(data_format));
	memcpy(new, *visited, pos * sizeof(char*));
	new[pos] = malloc((strlen(add) + 1) * sizeof(char));
	strcpy(new[pos], add);
	memcpy(new + pos + 1, *visited + pos, (*visited_size - pos - 1) * sizeof(char*));
	free(*visited);
	*visited = new;
	return 0;
}
static void clear_visited(char **visited, size_t visited_size) {
	size_t i;
	if (visited) {
		for (i = 0; i < visited_size; i++) {
			if (visited[i]) free(visited[i]);
		}
		free(visited);
	}
}

static void listing_do(char *, data_format**, size_t*, char***, size_t*);
static void list_add(data_format **list, size_t *list_size, char *n, size_t f, size_t t) {
	data_format key = {n, f, t};
	ssize_t pos = bspos(&key, *list, *list_size, sizeof(data_format), df_cmp);
	if (pos < 0) return;
	data_format *new = malloc(++*list_size * sizeof(data_format));
	memcpy(new, *list, pos * sizeof(data_format));
	new[pos].n = malloc((strlen(n) + 1) * sizeof(char));
	strcpy(new[pos].n, n);
	new[pos].f = f;
	new[pos].t = t;
	memcpy(new + pos + 1, *list + pos, (*list_size - pos - 1) * sizeof(data_format));
	free(*list);
	*list = new;
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
static void list_read(int fd, char *n, data_format **list, size_t *list_size, char ***visited, size_t *visit_size) {
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
		listing_do(buf, list, list_size, visited, visit_size);
		free(buf);
	}
	if (chdir(old)) {
		free(old);
		return;
	}
	free(old);
}
static void dir_read(char *n, data_format **list, size_t *list_size, char ***visited, size_t *visit_size) {
	if (visit(n, visited, visit_size)) return;
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
				listing_do(c->d_name, list, list_size, visited, visit_size);
		}
		closedir(d);
	}
	if (chdir(old)) {
		free(old);
		return;
	}
	free(old);
}
static void listing_do(char *o, data_format **list, size_t *list_size, char ***visited, size_t *visit_size) {
	size_t from = 0, to = SIZE_MAX;
	char *c1 = strchr(o, ';'), *c2;
	char *n;
	if (c1 && (c2 = strchr(c1 + 1, ';'))) {
		// cueSheet;dataFile;trackNum
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
	struct stat st;
	int fd;
#ifdef _WIN32
	if (!stat(n, &st)) {
#else
	if ((fd = open(n, O_RDONLY)) != -1) {
		if (!fstat(fd, &st)) {
#endif
			if (S_ISDIR(st.st_mode)) dir_read(n, list, list_size, visited, visit_size);
			else {
#ifdef _WIN32
			if ((fd = open(n, O_RDONLY)) != -1) {
#endif
				uint32_t fourcc;
				if (read(fd, &fourcc, 4) == 4) {
					if (fourcc == 0x46464952 || fourcc == 0x43614C66 || fourcc == 0x01564C46) list_add(list, list_size, n, from, to);
					else if (fourcc == 0x5453494C) list_read(fd, n, list, list_size, visited, visit_size);
				}
#ifdef _WIN32
				close(fd);
#endif
			}
		}
#ifndef _WIN32
		close(fd);
#endif
	}
	free(n);
}
void list_full_remove(data_format *list) {
	size_t i;
	if (list) {
		for (i = 0; list[i].n; i++) {
			if (list[i].n) free(list[i].n);
		}
		free(list);
	}
	list = NULL;
}
void list_shuffle(data_format *list) {
#ifdef _WIN32
	srand(time(NULL));
#else
	srandom(time(NULL));
#endif
	size_t i, r;
	data_format t;
	for (i = 1; list[i].n; i++) {
#ifdef _WIN32
		r = rand() % i;
#else
		r = random() % i;
#endif
		t = list[i];
		list[i] = list[r];
		list[r] = t;
	}
}
data_format *listing(char **n) {
	data_format *list = NULL;
	size_t list_size = 0;
	char **visited = NULL;
	size_t visit_size = 0;
	while (*n) {
		listing_do(*n++, &list, &list_size, &visited, &visit_size);
	}
	clear_visited(visited, visit_size);
	if (!list) return NULL;
	list = realloc(list, (list_size + 1) * sizeof(data_format));
	list[list_size].n = NULL;
	return list;
}
