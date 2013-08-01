#define IN_ATTR_NAME 0x1
#define IN_ATTR_VAL_EMP 0x2
#define IN_ATTR_VAL_APOS 0x4
#define IN_ATTR_VAL_QUOT 0x8
#define IN_TAG_NAME 0x10
#define IN_ATTR_VAL (IN_ATTR_VAL_EMP|IN_ATTR_VAL_APOS|IN_ATTR_VAL_QUOT)
#define IN_ATTR (IN_ATTR_VAL|IN_ATTR_NAME)
#define IN_TAG (IN_TAG_NAME|IN_ATTR_NAME|IN_ATTR_VAL)
#define OUT_QUOTE (IN_ATTR_NAME|IN_ATTR_VAL_EMP|IN_TAG_NAME)

#define IN_ENDTAG 0x100
#define IN_DECL 0x200

struct xml {
	char el_n[256];
	char el_v[512];
	char at_n[64];
	char at_v[128];
	char pv;
	int fl;
	void(*tag)(struct xml*);
	void(*attr)(struct xml*);
};

void next(char, struct xml*);