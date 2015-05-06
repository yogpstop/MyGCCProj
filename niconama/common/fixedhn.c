#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fixedhn.h"
#include "xml.h"

static int cmp_com(const void *m1, const void *m2) {
	return strcmp(((hn_com*)m1)->com, ((hn_com*)m2)->com);
}
static int cmp_id(const void *m1, const void *m2) {
	return strcmp(((hn_user*)m1)->id, ((hn_user*)m2)->id);
}
char *hn_find(fixedhn *obj, char *com, char *id) {
	hn_com hck; hck.com = com;
	hn_com *hc = bsearch(&hck, obj->c, obj->l, sizeof(hn_com), cmp_com);
	if (!hc) return id;
	hn_user huk; huk.id = id;
	hn_user *hu = bsearch(&huk, hc->u, hc->l, sizeof(hn_user), cmp_id);
	return hu ? hu->name : id;
}
void hn_add(fixedhn *obj, char *com, char *id, char *name, char *tim) {
	hn_com hck; hck.com = com;
	hn_com *hc = bsearch(&hck, obj->c, obj->l, sizeof(hn_com), cmp_com);
	if (!hc) {
		hck.com = NULL; // Need qsort
		obj->c = realloc(obj->c, ++obj->l * sizeof(hn_com));
		memset(obj->c + obj->l - 1, 0, sizeof(hn_com));
		hc = obj->c + obj->l - 1;
		hc->com = malloc(strlen(com) + 1);
		strcpy(hc->com, com);
	}
	hn_user huk; huk.id = id;
	hn_user *hu = bsearch(&huk, hc->u, hc->l, sizeof(hn_user), cmp_id);
	if (hu) return;
	hc->u = realloc(hc->u, ++hc->l * sizeof(hn_user));
	memset(hc->u + hc->l - 1, 0, sizeof(hn_user));
	hu = hc->u + hc->l - 1;
	hu->id = malloc(strlen(id) + 1);
	strcpy(hu->id, id);
	hu->name = malloc(strlen(name) + 1);
	strcpy(hu->name, name);
	if (tim) {
		hu->time = malloc(strlen(tim) + 1);
		strcpy(hu->time, tim);
	} else {
		hu->time = malloc(21);
		sprintf(hu->time, "%"
#if _WIN32
		"I64"
#else
		"ll"
#endif
		"u", (unsigned long long) time(NULL));
	}
	qsort(hc->u, hc->l, sizeof(hn_user), cmp_id);
	if(!hck.com) qsort(obj->c, obj->l, sizeof(hn_com), cmp_com);
}
fixedhn *hn_init() {
	fixedhn *r = malloc(sizeof(fixedhn));
	memset(r, 0, sizeof(fixedhn));
	return r;
}
void hn_uninit(fixedhn *r) {
	int i, j;
	for (i = 0; i < r->l; i++) {
		for(j = 0; j < r->c[i].l; j++) {
			free(r->c[i].u[j].id);
			free(r->c[i].u[j].name);
			free(r->c[i].u[j].time);
		}
		free(r->c[i].u);
		free(r->c[i].com);
	}
	free(r->c);
	free(r);
}
static void xml_cb_a(void *user, char *el_n, char *at_n, char *at_v) {
	if (strcmp(el_n, "UserSetting/user")) return;
	void **u = user;
	if (!strcmp(at_n, "community")) {
		if(u[1]) return;
		u[1] = malloc(strlen(at_v) + 1);
		strcpy(u[1], at_v);
	} else if (!strcmp(at_n, "name")) {
		if(u[2]) return;
		u[2] = malloc(strlen(at_v) + 1);
		strcpy(u[2], at_v);
	} else if (!strcmp(at_n, "time")) {
		if(u[3]) return;
		u[3] = malloc(strlen(at_v) + 1);
		strcpy(u[3], at_v);
	}
}
static void xml_cb_e(void *user, char *el_n, char *el_v) {
	if (strcmp(el_n, "UserSetting/user")) return;
	void **u = user;
	if (u[1] && u[2]) hn_add(u[0], u[1], el_v, u[2], u[3]);
	if (u[1]) { free(u[1]); u[1] = NULL; }
	if (u[2]) { free(u[2]); u[2] = NULL; }
	if (u[3]) { free(u[3]); u[3] = NULL; }
}
void hn_load(fixedhn *r, char *fn) {
	void **u = malloc(4 * sizeof(void*));
	u[0] = r; u[1] = NULL; u[2] = NULL; u[3] = NULL;
	struct xml data;
	memset(&data, 0, sizeof(struct xml));
	data.tag = xml_cb_e;
	data.attr = xml_cb_a;
	data.user = u;
	FILE *f = fopen(fn, "rb");
	char tmp;
	while (fread(&tmp, 1, 1, f))
		xml_next(tmp, &data);
	fclose(f);
}

void hn_save(fixedhn *hn, char *fn) {
	FILE *f = fopen(fn, "wb");
	fputs("<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n", f);
	fputs("<UserSetting xmlns:"
		"NCV=\"http://posite-c.jp/niconamacommentviewer/common/\" xmlns=\""
		"http://posite-c.jp/niconamacommentviewer/usersetting/\">\r\n", f);
	int i, j;
	for (i = 0; i < hn->l; i++) {
		for(j = 0; j < hn->c[i].l; j++)
			fprintf(f, "\t<user community=\"%s\" name=\"%s\" "
					"time=\"%s\">%s</user>\r\n", hn->c[i].com,
					hn->c[i].u[j].name, hn->c[i].u[j].time, hn->c[i].u[j].id);
	}
	fputs("</UserSetting>\r\n", f);
	fclose(f);
}

int main(int argc, char **argv) {
	fixedhn *hn = hn_init();
	hn_load(hn, argv[1]);
	int i, j;
	for (i = 0; i < hn->l; i++) {
		printf("%s\n", hn->c[i].com);
		for(j = 0; j < hn->c[i].l; j++)
			printf("\t%s %s %s\n", hn->c[i].u[j].id,
					hn->c[i].u[j].name, hn->c[i].u[j].time);
	}
	hn_save(hn, argv[2]);
	hn_uninit(hn);
	return 0;
}
