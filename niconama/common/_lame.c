int main() {
	lame_global_flags *gf;

	gf = lame_init()
	lame_set_*(gf, value);
	lame_init_params(gf);

	lame_encode_buffer(gf, short[] l, short[] r, int samples per channel, char* encodedbuf, int bufsize);
	lame_encode_flush(gf, char*, int);

	lame_close(gf);
}
