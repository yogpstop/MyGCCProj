#include "xml_struct.h"

extern void getalertdata();

extern void getSession();
extern struct getplayerstatus* getplayerstatus(char*);
extern void freegetplayerstatus(struct getplayerstatus*);
extern void printcomment(struct getplayerstatus*);

extern void create();

int main(int argc, char *argv[]) {
	create();
	/*getSession();
	if(argc==2) {
		struct getplayerstatus* gps;
		gps = getplayerstatus(argv[1]);
		printcomment(gps);
		freegetplayerstatus(gps);
	} else
		getalertdata();*/
	return 0;
}