#include <GL/gl.h>
#include <GL/glx.h>

void glGenFramebuffers(GLsizei, GLuint*);
void glBindFramebuffer(GLenum, GLuint);
void glXSwapIntervalEXT(Display*, GLXDrawable, int);
void glFramebufferTexture2D(GLenum, GLenum, GLenum, GLuint, GLint);
