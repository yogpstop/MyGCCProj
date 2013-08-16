#define VIDEO_DEVICE "/dev/video0"
#define FRAME_WIDTH  1920
#define FRAME_HEIGHT 1080
#define FPS 30
#if 0
#define OGLC
#endif

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#ifdef OGLC
	#include <GL/gl.h>
	#include <malloc.h>
#else
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
#endif

#define FRAME_NSEC ((long)(1000000000/FPS))
#define ERROR_ONSEC (1000000000-FRAME_NSEC*FPS)

#ifdef OGLC
	static __u8*buffer;
#else
	static Display* d;
#endif
static int fdwr = 0;

void onExit(int id) {
	close(fdwr);
	#ifdef OGLC
		free(buffer);
	#else
		XCloseDisplay(d);
	#endif
}

int main() {
	struct v4l2_capability vid_caps;
	struct v4l2_format vid_format;
	struct timespec nBefore;
	size_t frames = 0;

	fdwr = open(VIDEO_DEVICE, O_RDWR);

	ioctl(fdwr, VIDIOC_QUERYCAP, &vid_caps);

	memset(&vid_format, 0, sizeof(vid_format));

	ioctl(fdwr, VIDIOC_G_FMT, &vid_format);

	vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vid_format.fmt.pix.width = FRAME_WIDTH;
	vid_format.fmt.pix.height = FRAME_HEIGHT;
	vid_format.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR32;
	vid_format.fmt.pix.sizeimage = FRAME_WIDTH * FRAME_HEIGHT * 4;
	vid_format.fmt.pix.field = V4L2_FIELD_NONE;
	vid_format.fmt.pix.bytesperline = FRAME_WIDTH;
	vid_format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

	ioctl(fdwr, VIDIOC_S_FMT, &vid_format);

	#ifdef OGLC
		buffer=malloc(vid_format.fmt.pix.sizeimage);
	#else
		d = XOpenDisplay(NULL);
		Window root = DefaultRootWindow(d);
		XImage *img;
	#endif

	signal(0,onExit);

	clock_gettime(CLOCK_MONOTONIC,&nBefore);

	while(1) {
		#ifdef OGLC
			glReadBuffer(GL_FRONT);
			glReadPixels(0,0,FRAME_WIDTH,FRAME_HEIGHT,GL_BGRA,GL_UNSIGNED_BYTE,buffer);
			write(fdwr, buffer, vid_format.fmt.pix.sizeimage);
		#else
			img = XGetImage(d, root, 0, 0, FRAME_WIDTH, FRAME_HEIGHT, AllPlanes, ZPixmap);
			if(img != NULL) {
			write(fdwr, img->data, vid_format.fmt.pix.sizeimage);
			XDestroyImage(img);
		#endif
		frames++;
		nBefore.tv_nsec += FRAME_NSEC;
		if(!(frames%FPS))
			nBefore.tv_nsec += ERROR_ONSEC;
		if(nBefore.tv_nsec >= 1000000000) {
			nBefore.tv_nsec -= 1000000000;
			nBefore.tv_sec += 1;
		}
		clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&nBefore,NULL);
		#ifndef OGLC
			}
		#endif
	}
	return 0;
}