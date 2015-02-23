#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include "thread.h"
#include "gl3.h"

void loadTexture(const char *, GLuint);
void *glmain(pt_glmain *p) {
	static int fbattr[] = {
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, True,
		GLX_SAMPLE_BUFFERS, 1,
		GLX_SAMPLES, 1,
		None
	};
	Display *dsp = XOpenDisplay(NULL);
	int scr = DefaultScreen(dsp);
	Window root = RootWindow(dsp, scr);
	int fbc;
	GLXFBConfig *fb = glXChooseFBConfig(dsp, scr, fbattr, &fbc);
	int best_samples = 0, best_i = -1, i, tmp;
	for (i = 0; i < fbc; i++) {
		glXGetFBConfigAttrib(dsp, fb[i], GLX_SAMPLES, &tmp);
		if (tmp > best_samples) {
			best_samples = tmp;
			best_i = i;
		}
	}
	XVisualInfo *vi = glXGetVisualFromFBConfig(dsp, fb[best_i]);
	XSetWindowAttributes wattr;
	wattr.colormap = XCreateColormap(dsp, root, vi->visual, AllocNone);
	wattr.background_pixmap = 0;
	wattr.border_pixel = 0;
	wattr.event_mask = 0;//TODO
	Window win = XCreateWindow(dsp, root, 0, 0, 1280, 960,
			0, vi->depth, InputOutput, vi->visual,
			CWBackPixel|CWBorderPixel|CWColormap/*|CWEventMask*/, &wattr);
	XMapWindow(dsp, win);
	GLXContext glx = glXCreateNewContext(dsp, fb[best_i],
			GLX_RGBA_TYPE, NULL, True);
	glXMakeCurrent(dsp, win, glx);
	glXSwapIntervalEXT(dsp, win, 0);
	glScaled(1.0/320.0,1.0/240.0,1);
	glRotated(180, 1, 0, 0);
	// 64-512-64, 56-384-40
	// wider than 4:3 => letterbox, narrower than 4:3 => crop
	glTranslated(-256, -184, 0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(1);
	GLuint tex[6];
	glGenTextures(6, tex);
	loadTexture("/media/DATA/ws/osu_skin/hitcircle@2x.png", tex[0]);
	loadTexture("/media/DATA/ws/osu_skin/hitcircleoverlay@2x.png", tex[1]);
	loadTexture("/media/DATA/ws/osu_skin/cursor@2x.png", tex[2]);
	loadTexture("/media/DATA/ws/osu_skin/cursormiddle@2x.png", tex[3]);
	loadTexture("/media/DATA/ws/osu_skin/approachcircle@2x.png", tex[4]);
	loadTexture("/media/DATA/ws/osu_skin/sliderscorepoint@2x.png", tex[5]);
	int n = 0;
	hito *bbp/*, *min*/;
	double cs = 45 * (12 - p->cs) / 12;
	//struct timespec ts = {0, 1000000};
	point *pshow;
	//void *dummy = malloc(1280*960*4);
	while (++n) {
		if (*p->done) break;
		glClear(GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		//min = NULL;
		pthread_mutex_lock(p->mutex);
		for (bbp = p->buf + p->buf_len - 1; bbp >= p->buf; bbp--) {
			if (bbp->alpha < 0) continue;
			glColor4d(bbp->rgb * bbp->combo->r, bbp->rgb * bbp->combo->g, bbp->rgb * bbp->combo->b, bbp->alpha);
			glBindTexture(GL_TEXTURE_2D, tex[0]);
			if (bbp->type & OSU_OBJ_TYPE_SLIDER) {
				pshow = bbp->p + (unsigned int)((bbp->plen - 1) * bbp->cshow);
				glBegin(GL_QUADS);
				glTexCoord2d(0, 0);
				glVertex2d(pshow->x - cs, pshow->y - cs);
				glTexCoord2d(0, 1);
				glVertex2d(pshow->x - cs, pshow->y + cs);
				glTexCoord2d(1, 1);
				glVertex2d(pshow->x + cs, pshow->y + cs);
				glTexCoord2d(1, 0);
				glVertex2d(pshow->x + cs, pshow->y - cs);
				glEnd();
			}
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2d(bbp->p->x - cs, bbp->p->y - cs);
			glTexCoord2d(0, 1); glVertex2d(bbp->p->x - cs, bbp->p->y + cs);
			glTexCoord2d(1, 1); glVertex2d(bbp->p->x + cs, bbp->p->y + cs);
			glTexCoord2d(1, 0); glVertex2d(bbp->p->x + cs, bbp->p->y - cs);
			glEnd();
			if (bbp->acsize > 0) {
				glBindTexture(GL_TEXTURE_2D, tex[4]);
				glBegin(GL_QUADS);
				glTexCoord2d(0, 0); glVertex2d(bbp->p->x - cs * bbp->acsize, bbp->p->y - cs * bbp->acsize);
				glTexCoord2d(0, 1); glVertex2d(bbp->p->x - cs * bbp->acsize, bbp->p->y + cs * bbp->acsize);
				glTexCoord2d(1, 1); glVertex2d(bbp->p->x + cs * bbp->acsize, bbp->p->y + cs * bbp->acsize);
				glTexCoord2d(1, 0); glVertex2d(bbp->p->x + cs * bbp->acsize, bbp->p->y - cs * bbp->acsize);
				glEnd();
			}
			glColor4d(bbp->rgb, bbp->rgb, bbp->rgb, bbp->alpha);
			glBindTexture(GL_TEXTURE_2D, tex[1]);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2d(bbp->p->x - cs, bbp->p->y - cs);
			glTexCoord2d(0, 1); glVertex2d(bbp->p->x - cs, bbp->p->y + cs);
			glTexCoord2d(1, 1); glVertex2d(bbp->p->x + cs, bbp->p->y + cs);
			glTexCoord2d(1, 0); glVertex2d(bbp->p->x + cs, bbp->p->y - cs);
			glEnd();
		}
		/*if (min) {
			glColor4d(1, 1, 1, 1);
			glBindTexture(GL_TEXTURE_2D, tex[2]);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2d(min->p->x - 24, min->p->y - 24);
			glTexCoord2d(0, 1); glVertex2d(min->p->x - 24, min->p->y + 24);
			glTexCoord2d(1, 1); glVertex2d(min->p->x + 24, min->p->y + 24);
			glTexCoord2d(1, 0); glVertex2d(min->p->x + 24, min->p->y - 24);
			glEnd();
			glBindTexture(GL_TEXTURE_2D, tex[3]);
			glBegin(GL_QUADS);
			glTexCoord2d(0, 0); glVertex2d(min->p->x - 10, min->p->y - 10);
			glTexCoord2d(0, 1); glVertex2d(min->p->x - 10, min->p->y + 10);
			glTexCoord2d(1, 1); glVertex2d(min->p->x + 10, min->p->y + 10);
			glTexCoord2d(1, 0); glVertex2d(min->p->x + 10, min->p->y - 10);
			glEnd();
		}*/
		pthread_mutex_unlock(p->mutex);
		glPopMatrix();
		//glReadBuffer(GL_BACK);//TODO capture
		//glReadPixels(0, 0, 1280, 960, GL_RGBA, GL_UNSIGNED_BYTE, dummy);
		glXSwapBuffers(dsp, win);
		//nanosleep(&ts, NULL);
	}
	glDeleteTextures(6, tex);
	glXDestroyContext(dsp, glx);
	XFree(vi);
	XDestroyWindow(dsp, win);
	XFreeColormap(dsp, wattr.colormap);
	XCloseDisplay(dsp);
	return NULL;
}
