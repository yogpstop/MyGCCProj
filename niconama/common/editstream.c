#include "main.h"
#include "xml.h"
#include "cookie.h"
#include <pcre.h>

//TODO input{file,reset,button,image,submit} button

static pcre *reg_form = NULL, *reg_select = NULL, *reg_option = NULL,
		*reg_name = NULL, *reg_value = NULL, *reg_type = NULL, *reg_any = NULL;

typedef struct {
	char *v;
	int l;
} form_vl_set;

typedef struct {
	form_vl_set n;
} form_text_t;

typedef struct {
	form_vl_set n;
	form_vl_set v;
	int f;
} form_bool_t;

typedef struct {
	form_vl_set n;
	form_vl_set *v;
	int vl;
	int f;
} form_select_t;

typedef struct {
	char *internal_buffer;
	form_text_t *t;
	int tl;
	form_bool_t *b;
	int bl;
	form_select_t *s;
	int sl;
} form_t;

static int get_i(void **v, int *vl, int size) {
	void *p, *x; int i = 0;
	while (i < *vl) {
		p = *v + i * size; x = p + size;
		while (p < x) {
			if (*(unsigned char*)p != 0) break;
			p++;
		}
		if (p == x) return i;
		i++;
	}
	if (!*vl) *vl = 8; else *vl <<= 1;
	*v = realloc(*v, *vl * size);
	memset(*v + i * size, 0, (*vl - i) * size);
	return i;
}

static int form_is(const char *a, size_t al, const char *b) {
	size_t bl = strlen(b), i = 0;
	al -= bl;
	while (i <= al) if (!memcmp(a + i++, b, bl)) return 1;
	return 0;
}

static int find_select(form_select_t **v, int *vl, form_vl_set *n) {
	int vi = 0;
	while (vi < *vl && (*v)[vi].n.v) {
		if (n->l == (*v)[vi].n.l && !memcmp(n->v, (*v)[vi].n.v, n->l))
			return vi;
		vi++;
	}
	vi = get_i((void**)v, vl, sizeof(form_select_t));
	(*v)[vi].n.l = n->l; (*v)[vi].n.v = n->v;
	return vi;
}

static void get(char *str, int len,
		form_vl_set *name, form_vl_set *value, form_vl_set *type) {
	if (!reg_name || !reg_value || !reg_type) {
		const char *err = NULL; int erroff = 0;
		if (!reg_name)
			reg_name = pcre_compile("name=\"(.*?)\"",
					PCRE_CASELESS | PCRE_DOTALL, &err, &erroff, NULL);
		if (!reg_value)
			reg_value = pcre_compile("value=\"(.*?)\"",
					PCRE_CASELESS | PCRE_DOTALL, &err, &erroff, NULL);
		if (!reg_type)
			reg_type = pcre_compile("type=\"(.*?)\"",
					PCRE_CASELESS | PCRE_DOTALL, &err, &erroff, NULL);
		if (err || erroff || !reg_name || !reg_value || !reg_type) return;
	}
	int ov[6];
	if (name && 0 < pcre_exec(reg_name, NULL, str, len, 0, 0, ov, 6)) {
		name->v = str + ov[2]; name->l = ov[3] - ov[2]; }
	if (value && 0 < pcre_exec(reg_value, NULL, str, len, 0, 0, ov, 6)) {
		value->v = str + ov[2]; value->l = ov[3] - ov[2]; }
	if (type && 0 < pcre_exec(reg_type, NULL, str, len, 0, 0, ov, 6)) {
		type->v = str + ov[2]; type->l = ov[3] - ov[2]; }
}

static void get_form(char **str, int *len) {
	if (!reg_form) {
		const char *err = NULL; int erroff = 0;
		reg_form = pcre_compile("<form action=\"editstream\".*?>(.+?)</form>",
				PCRE_CASELESS | PCRE_DOTALL, &err, &erroff, NULL);
		if (err || erroff || !reg_form) return;
	}
	int ov[6];
	if (0 > pcre_exec(reg_form, NULL, *str, *len, 0, 0, ov, 6)) return;
	*str += ov[2];
	*len = ov[3] - ov[2];
}

static void get_select(char *str, int len, form_select_t **v, int *vl) {
	if (!reg_select || !reg_option) {
		const char *err = NULL; int erroff = 0;
		if (!reg_select)
			reg_select = pcre_compile("<select (.+?)>(.+?)</select>",
					PCRE_CASELESS | PCRE_DOTALL, &err, &erroff, NULL);
		if (!reg_option)
			reg_option = pcre_compile("<option (.+?)>",
					PCRE_CASELESS | PCRE_DOTALL, &err, &erroff, NULL);
		if (err || erroff || !reg_select || !reg_option) return;
	}
	int ov[9];
	int vi, voff = 0;
	while (0 < pcre_exec(reg_select, NULL, str, len, voff, 0, ov, 9)) {
		vi = get_i((void**)v, vl, sizeof(form_select_t));
		get(str + ov[2], ov[3] - ov[2], &(*v)[vi].n, NULL, NULL);
		int oi, ooff = 0;
		ov[7] = ov[4]; ov[8] = ov[5];
		while (0 < pcre_exec(reg_option, NULL, str + ov[7], ov[8] - ov[7],
				ooff, 0, ov, 6)) {
			oi = get_i((void**)&(*v)[vi].v, &(*v)[vi].vl, sizeof(form_vl_set));
			if (form_is(str + ov[7] + ov[2], ov[3] - ov[2], "selected"))
				(*v)[vi].f = oi;
			get(str + ov[7] + ov[2], ov[3] - ov[2],
					NULL, (*v)[vi].v + oi, NULL);
			ooff = ov[1];
		}
		voff = ov[8] + 9;
	}
}

static form_t *get_main(char *str, int len) {
    form_t *f = malloc(sizeof(form_t));
	memset(f, 0, sizeof(form_t));
	get_form(&str, &len);
	get_select(str, len, &f->s, &f->sl);
	if (!reg_any) {
		const char *err = NULL; int erroff = 0;
		reg_any = pcre_compile("<(input|button|textarea) (.+?)>",
				PCRE_CASELESS | PCRE_DOTALL, &err, &erroff, NULL);
		if (err || erroff || !reg_any) return NULL;
	}
	int ov[9];
	int vi, voff = 0, ei;
	form_vl_set fvs[3];
	while (0 < pcre_exec(reg_any, NULL, str, len, voff, 0, ov, 9)) {
		memset(fvs, 0, sizeof(form_vl_set) * 3);
		get(str + ov[4], ov[5] - ov[4], fvs, fvs + 1, fvs + 2);
		if (fvs[0].v) {
		if (ov[3] - ov[2] == 5 &&
				fvs[2].l == 5 && !memcmp(fvs[2].v, "radio", 5)) {
			vi = find_select(&f->s, &f->sl, fvs);
			ei = get_i((void**)&f->s[vi].v, &f->s[vi].vl, sizeof(form_vl_set));
			if (form_is(str + ov[4], ov[5] - ov[4], "checked"))
				f->s[vi].f = ei;
			f->s[vi].v[ei].v = fvs[1].v;
			f->s[vi].v[ei].l = fvs[1].l;
		} else if (ov[3] - ov[2] == 8 || (ov[3] - ov[2] == 5 &&
				((fvs[2].l == 4 && !memcmp(fvs[2].v, "text", 4)) ||
					(fvs[2].l == 8 && !memcmp(fvs[2].v, "password", 8))))) {
			vi = get_i((void**)&f->t, &f->tl, sizeof(form_text_t));
			f->t[vi].n.v = fvs[0].v;
			f->t[vi].n.l = fvs[0].l;
		} else if (ov[3] - ov[2] == 5 &&
				((fvs[2].l == 6 && !memcmp(fvs[2].v, "hidden", 6)) ||
					(fvs[2].l == 8 && !memcmp(fvs[2].v, "checkbox", 8)))) {
			vi = get_i((void**)&f->b, &f->bl, sizeof(form_bool_t));
			if ((fvs[2].l == 6 && !memcmp(fvs[2].v, "hidden", 6)) ||
					form_is(str + ov[4], ov[5] - ov[4], "checked"))
				f->b[vi].f = 1;
			f->b[vi].n.v = fvs[0].v;
			f->b[vi].n.l = fvs[0].l;
			f->b[vi].v.v = fvs[1].v;
			f->b[vi].v.l = fvs[1].l;
		} }
		voff = ov[1];
	}
	return f;
}

static void freeeditstream(form_t *v) {
	int ri = 0;
	while (ri < v->sl && v->s[ri].v) {
		free(v->s[ri].v); ri++;
	}
	free(v->s);
	free(v->b);
	free(v->t);
	free(v->internal_buffer);
	free(v);
}

static form_t *geteditstream(const char *p, const char *b) {
  int sock = create_socket("live.nicovideo.jp", "http", 0);
  send(sock, p && b ? "POST" : "GET", p && b ? 4 : 3, 0);
  send(sock, " /editstream HTTP/1.1\r\n"
      "Host: live.nicovideo.jp\r\nCookie: user_session=", 69, 0);
  char *session = getSession();
  send(sock, session, strlen(session), 0);
  if (p && b) {
    send(sock, "\r\nContent-Type: multipart/form-data; boundary=", 46, 0);
    send(sock, b, strlen(b), 0);
    send(sock, "\r\nContent-Length: ", 18, 0);
    char tmp[8];
    sprintf(tmp, "%"
#if _WIN32
	"I"
#else
	"z"
#endif
	"u", strlen(p));
    send(sock, tmp, strlen(tmp), 0);
  }
  send(sock, "\r\n\r\n", 4, 0);
  if (p && b) send(sock, p, strlen(p), 0);
  size_t bufl = 1024 * 128, bufi = 0;
  void *buf = malloc(bufl);
  int http = 0;
  while (2 > http) {
    if (1 > recv(sock, buf, 1, 0))
      break;
    else if (*(char*)buf == '\n')
      http++;
    else if (*(char*)buf != '\r')
      http = 0;
  }
  while (0 < (http = recv(sock, buf + bufi, bufl - bufi, 0))) {
    bufi += http;
    if (bufi >= bufl) buf = realloc(buf, bufl <<= 1);
  }
  form_t *ret = get_main(buf, bufi);
  CLOSESOCKET(sock);
  ret->internal_buffer = buf;
  return ret;
}

form_t *printeditstream(const char *p, const char *b) {
	form_t *ret = geteditstream(p, b);
	int ri = 0;
	while (ri < ret->sl && ret->s[ri].n.v) {
		printf("\n%d Name: ",ret->s[ri].f);
		fwrite(ret->s[ri].n.v, 1, ret->s[ri].n.l, stdout);
		int oi = 0;
		while (oi < ret->s[ri].vl && ret->s[ri].v[oi].v) {
			fputs("\n\t", stdout);
			fwrite(ret->s[ri].v[oi].v, 1, ret->s[ri].v[oi].l, stdout);
			oi++;
		}
		ri++;
	}
	ri = 0;
	while (ri < ret->bl && ret->b[ri].n.v) {
		printf("\n%d Name: ",ret->b[ri].f);
		fwrite(ret->b[ri].n.v, 1, ret->b[ri].n.l, stdout);
		fputs("\n\t",stdout);
		fwrite(ret->b[ri].v.v, 1, ret->b[ri].v.l, stdout);
		ri++;
	}
	ri = 0;
	while (ri < ret->tl && ret->t[ri].n.v) {
		fputs("\nName: ",stdout);
		fwrite(ret->t[ri].n.v, 1, ret->t[ri].n.l, stdout);
		ri++;
	}
	return ret;
}

static void reloccat(char **p, int *l, const char *s, int sl) {
	int pl = *p ? strlen(*p) : 0;
	while (*l <= sl + pl) { int ll = *l; if(!*l) *l = 1024; else *l <<= 1;
			*p = realloc(*p, *l); memset(*p + ll, 0, *l - ll); }
	strncpy(*p + pl, s, sl);
}
static void mpfd_add(char **p, int *l, const char *b,
		form_vl_set *n, form_vl_set *v) {
	reloccat(p, l, "--", 2);
	reloccat(p, l, b, strlen(b));
	reloccat(p, l, "\r\nContent-Disposition: form-data; name=\"", 40);
	reloccat(p, l, n->v, n->l);
	reloccat(p, l, "\"\r\n\r\n", 5);
	reloccat(p, l, v->v, v->l);
	reloccat(p, l, "\r\n", 2);
}
static void mpfd_done(char **p, int *l, const char *b) {
	reloccat(p, l, "--", 2);
	reloccat(p, l, b, strlen(b));
	reloccat(p, l, "--", 2);
	reloccat(p, l, "\r\n", 2);
}

int main() {
	WS2U
	form_t *ret = printeditstream(NULL, NULL);
	char *p = NULL;
	int l = 0;
	char *b = "---This_is_test_boundary---";
	int ri = 0;
	form_vl_set test = {"test", 4};
	while (ri < ret->sl && ret->s[ri].n.v) {
		mpfd_add(&p, &l, b, &ret->s[ri].n, ret->s[ri].v + ret->s[ri].f);
		ri++;
	}
	ri = 0;
	while (ri < ret->bl && ret->b[ri].n.v) {
		if (ret->b[ri].f)
		mpfd_add(&p, &l, b, &ret->b[ri].n, &ret->b[ri].v);
		ri++;
	}
	ri = 0;
	while (ri < ret->tl && ret->t[ri].n.v) {
		mpfd_add(&p, &l, b, &ret->t[ri].n, &test);
		ri++;
	}
	mpfd_done(&p, &l, b);
	form_t *ret2 = printeditstream(p, b);
    freeeditstream(ret);
    freeeditstream(ret2);
	WS2U
	return 0;
}
