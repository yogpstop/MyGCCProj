typedef struct {
	char *id;
	char *name;
	char *time;
} hn_user;
typedef struct {
	char *com;
	int l;
	hn_user *u;
} hn_com;
typedef struct {
	int l;
	hn_com *c;
} fixedhn;
char *hn_find(fixedhn*, char*, char*);
void hn_add(fixedhn*, char*, char*, char*, char*);
fixedhn *hn_init();
void hn_uninit(fixedhn*);
void hn_load(fixedhn*, char*);
void hn_save(fixedhn*, char*);
