#include "jsmn.h"
#include <stdio.h>
#include <malloc.h>

void print(const char * const buf, const jsmntok_t * const tokens, int * const l, const int t) {
	int s, j, b;
	if (tokens[*l].type == JSMN_OBJECT) {
		for(s = tokens[*l].size, j = 0, b = 0; j < s; j++) {
			(*l)++;
			if (b)
				print(buf, tokens, l, t + 2);
			else
				print(buf, tokens, l, t);
			b = ~b;
		}
	} else if (tokens[*l].type == JSMN_ARRAY) {
		for(s = tokens[*l].size, j = 0; j < s; j++) {
			(*l)++;
			for (b = 0; b < t; b++)
				fputc(' ', stdout);
			printf("array[%d]\n", j);
			print(buf, tokens, l, t + 2);
		}
	} else {
		for (j = 0; j < t; j++)
			fputc(' ', stdout);
		for (j = tokens[*l].start; j < tokens[*l].end; j++)
			fputc(*(buf+j), stdout);
		fputc('\n', stdout);
	}
}

int main(int argc, char *argv[]) {
	jsmn_parser p;
	jsmntok_t tokens[256] = {};
	char *buf;
	FILE *fp;
	int l;
	if ((fp = fopen(argv[1], "rb")) == NULL)
		return -1;
	fseek(fp, 0, SEEK_END);
	if ((buf = malloc(l = ftell(fp))) == NULL)
		return -1;
	fseek(fp, 0, SEEK_SET);
	fread(buf, 1, l, fp);
	fclose(fp);
	jsmn_init(&p);
	if (jsmn_parse(&p, buf, tokens, 256) != JSMN_SUCCESS)
		return -1;
	l = 0;
	print(buf, tokens, &l, 0);
	return 0;
}
