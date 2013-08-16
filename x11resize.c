#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
int main(){
	unsigned int child_c, i, j;
	int oc;
	char c[16], **o;
	XWindowChanges v;
	XTextProperty p;
	Display* d = XOpenDisplay(NULL);
	Window *children, droot, parent, root = DefaultRootWindow(d);
	XQueryTree(d, root, &droot, &parent, &children, &child_c);
	v.x=0;
	v.y=0;
	v.width=1280;
	v.height=720;
	for(i=0;i<child_c;i++) {
		if(0 == XGetWMName(d, children[i], &p))
			continue;
		XmbTextPropertyToTextList(d,&p,&o,&oc);
		printf("%d\n", i);
		for(j=0;j<oc;j++){
			printf("\t%s\n", o[j]);
		}
		XFreeStringList(o);
	}
	//XConfigureWindow(d,children[j],CWX|CWY|CWWidth|CWHeight,&v);
	XFree(children);
	XCloseDisplay(d);
	return 0;
}