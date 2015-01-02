#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <GL/gl.h>

void loadTexture(const char *fn, GLuint tex) {
	unsigned int w, h;
	int b, c;
	FILE *fp = fopen(fn, "rb");
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
			NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, fp);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &w, &h, &b, &c, NULL, NULL, NULL);
	png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);
	png_bytep ret = malloc(h * png_get_rowbytes(png_ptr, info_ptr));
	png_bytepp ptr = malloc(h * sizeof(void*));
	int i;
	for (i = 0; i < h; i++)
		ptr[i] = ret + i * png_get_rowbytes(png_ptr, info_ptr);
	png_read_image(png_ptr, ptr);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	free(ptr);
	fclose(fp);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ret);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	free(ret);
}
