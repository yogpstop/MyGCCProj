#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

void read_cue(char *fn, size_t track, size_t *from, size_t *to) {
	int fd = open(fn, O_RDONLY);
	if (fd == -1) return;
	struct stat st;
	if (fstat(fd, &st)) {
		close(fd);
		return;
	}
	char *data = malloc(st.st_size);
	if (!data) {
		close(fd);
		return;
	}
	if (st.st_size > read(fd, data, st.st_size)) {
		close(fd);
		free(data);
		return;
	}
	close(fd);
	char tr[9], *cpos;
	sprintf(tr, "TRACK %02"PFZ"u", track);
	cpos = strstr(data, tr);
	if (!cpos) {
		free(data);
		return;
	}
	cpos += 8;
	cpos = strstr(cpos, "INDEX 01");
	if (!cpos) {
		free(data);
		return;
	}
	cpos += 9;
	int buf = (cpos[0] - '0') * 10 * 60 * 75;
	buf += (cpos[1] - '0') * 60 * 75;
	buf += (cpos[3] - '0') * 10 * 75;
	buf += (cpos[4] - '0') * 75;
	buf += (cpos[6] - '0') * 10;
	buf += (cpos[7] - '0');
	*from = buf * 588 * 4;
	cpos += 8;
	cpos = strstr(cpos, "INDEX");
	if (!cpos) {
		free(data);
		return;
	}
	cpos += 9;
	buf = (cpos[0] - '0') * 10 * 60 * 75;
	buf += (cpos[1] - '0') * 60 * 75;
	buf += (cpos[3] - '0') * 10 * 75;
	buf += (cpos[4] - '0') * 75;
	buf += (cpos[6] - '0') * 10;
	buf += (cpos[7] - '0');
	*to = buf * 588 * 4;
	free(data);
}
