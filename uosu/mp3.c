#include <mpg123.h>

void *mp3(char *afn, size_t *pm) {
	mpg123_init();
	mpg123_handle *mh = mpg123_new(NULL, NULL);
	mpg123_open(mh, afn);
	mpg123_format_none(mh);
	mpg123_format(mh, 44100, MPG123_STEREO, MPG123_ENC_SIGNED_16);
	mpg123_scan(mh);
	size_t l = mpg123_length(mh) * 2 * 16 / 8, d;
	void *m = malloc(l), *p = m;
	//mpg123_seek(mh, atoi(argv[2]) * 44100 / 1000, SEEK_CUR);
	while (MPG123_DONE != mpg123_read(mh, p, l, &d)) {
		l -= d; p += d; }
	*pm = p + d - m;
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
	return m;
}
