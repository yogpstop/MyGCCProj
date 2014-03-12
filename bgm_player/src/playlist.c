#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
char *mdir_name (char const *file);
#include "lib/canonicalize.h"
char **list;
size_t list_size;
static void listing_do(char *);
static void list_add(char *name) {
	list = realloc(list, ++list_size * sizeof(char*));
	char **to = &(list[list_size-1]);
	*to = malloc((strlen(name)+1)*sizeof(char));
	strcpy(*to, name);
}
static char *list_read_do(FILE *input) {
	char *cur, *buf, *end;
	size_t s = 16;
	buf = cur = malloc(s);
	memset(buf, 0, s);
	end = buf + s;
	do {
		if(fread(cur, 1, 1, input)<1 && feof(input)) *cur = 0x0A;
		if(end < cur) {
			s <<= 1;
			end = realloc(buf, s);
			cur += end - buf;
			buf = end;
			end = buf + s;
		}
	} while(*cur++ != 0x0A);
	cur--;
	*cur = 0;
	return buf;
}
static void list_read(FILE *f, char *n) {
	char *old = getcwd(NULL, 0);
	char *new = mdir_name(n);
	chdir(new);
	free(new);
	char _;
	fread(&_, 1, 1, f);
	char *buf;
	while (1) {
		buf = list_read_do(f);
		if (strlen(buf) == 0) {
			free(buf);
			break;
		}
		listing_do(buf);
		free(buf);
	}
	chdir(old);
	free(old);
}
static void dir_read(char *n) {
	char *old = getcwd(NULL, 0);
	chdir(n);
	DIR *d = opendir(".");
	struct dirent *c;
	while ((c = readdir(d))!=NULL) {
		if (strcmp(".", c->d_name) && strcmp("..", c->d_name))
			listing_do(c->d_name);
	}
	closedir(d);
	chdir(old);
	free(old);
}
static void listing_do(char *o) {
	char *n = canonicalize_filename_mode(o, CAN_MISSING);
	struct stat st;
	stat(n, &st);
	if(S_ISDIR(st.st_mode)) dir_read(n);
	else {
		FILE *f = fopen(n, "rb");
		uint32_t fourcc;
		if (fread(&fourcc, 4, 1, f) != 1) {
			fclose(f);
			return;
		}
		if(fourcc == 0x46464952) list_add(n);
		else if(fourcc == 0x43614C66) list_add(n);
		else if(fourcc == 0x5453494C) list_read(f, n);
		fclose(f);
	}
	free(n);
}
static int mycmp(const void *a, const void *b) {
	return strcmp(*(char*const*)a,*(char*const*)b);
}
static void list_remove(size_t pos) {
	if(pos + 1 < list_size) {
		memmove(list + pos, list + pos + 1,
			(list_size - pos - 1) * sizeof(char*));
	}
	list = realloc(list, --list_size * sizeof(char*));
}
static void list_swap(size_t p1, size_t p2) {
	char *t = list[p1];
	list[p1] = list[p2];
	list[p2] = t;
}
void list_shuffle() {
	srandom(time(NULL));
	size_t i, r;
	for(i = 0; i < list_size - 1; i++) {
		r = random() % list_size;
		list_swap(i, r);
	}
}
int listing(char **n) {
	while(*n!=NULL) {
		listing_do(*n++);
	}
	if(list_size == 0 || list == NULL) return 1;
	qsort(list, list_size, sizeof(char*), mycmp);
	size_t cur = 1;
	while(cur < list_size) {
		if (!strcmp(list[cur - 1], list[cur])) {
			list_remove(cur);
		} else {
			cur++;
		}
	}
	return 0;
}
