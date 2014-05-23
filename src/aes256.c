//!/usr/bin/gcc -Wall -Werror -O3 -s -o aes256 aes256.c -lcrypto
//!/usr/bin/gcc -Wall -Werror -O3 -s -o aes256.exe aes256.c -lcrypto -lgdi32
#include <string.h>
#ifndef _WIN32
#include <stdio_ext.h>
#include <termios.h>
#else
#include <windows.h>
#include <wincon.h>
#endif
#include <unistd.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/err.h>
int aes256(const unsigned char* in, int ilen, const unsigned char key[256/8],
	const unsigned char iv[128/8], unsigned char** out, int mode) {
	int mlen = ilen + AES_BLOCK_SIZE - (ilen % AES_BLOCK_SIZE),
		olen = mlen, flen = 0;
	*out = malloc(mlen);
	if(out == NULL)
		return 0;
	memset(*out, 0, mlen);
	EVP_CIPHER_CTX evpctx;
	EVP_CIPHER_CTX_init(&evpctx);
	if(1 != EVP_CipherInit_ex(&evpctx, EVP_aes_256_ofb(), NULL, key, iv, mode))
		goto error;
	if(1 != EVP_CipherInit_ex(&evpctx, NULL, NULL, NULL, NULL, mode))
		goto error;
	if(1 != EVP_CipherUpdate(&evpctx, *out, &olen, in, ilen))
		goto error;
	if(1 != EVP_CipherFinal_ex(&evpctx, *out + olen, &flen))
		goto error;
	EVP_CIPHER_CTX_cleanup(&evpctx);
	return olen + flen;
error:
	EVP_CIPHER_CTX_cleanup(&evpctx);
	memset(*out, 0, mlen);
	free(*out);
	*out = NULL;
	return 0;
}
#ifdef _WIN32
static HANDLE conin;
#endif
size_t input(const char* msg, unsigned char **buf) {
	unsigned char *cur, *end;
	size_t s = AES_BLOCK_SIZE;
	*buf = cur = malloc(s);
	if(!cur) return 0;
	memset(*buf, 0, s);
	end = *buf + s - 1;
#ifndef _WIN32
	__fpurge(stdin);
#else
	FlushConsoleInputBuffer(conin);
#endif
	fputs(msg, stdout);
	do {
		fread(cur, 1, 1, stdin);
		if(end <= cur) {
			s += AES_BLOCK_SIZE;
			end = malloc(s);
			if(!end) {
				memset(*buf, 0, s - AES_BLOCK_SIZE);
				free(*buf);
				*buf = NULL;
				fputc('\n', stdout);
				return 0;
			}
			memset(end, 0, s);
			memcpy(end, *buf, s - AES_BLOCK_SIZE);
			memset(*buf, 0, s - AES_BLOCK_SIZE);
			cur += end - *buf;
			free(*buf);
			*buf = end;
			end = *buf + s - 1;
		}
	} while(*cur++ != 0x04);
	cur--;
	fputc('\n', stdout);
	return cur - *buf;
}
int main(int argc, char* argv[]) {
	FILE *f = NULL;
	unsigned char *raw = NULL, *rpass = NULL, *riv = NULL, *out = NULL,
		pass[256 / 8], iv[128 / 8];
	size_t lraw = 0, lrpass = 0, lriv = 0;
	int lout = 0, enc = 0;
	if(argc!=3) {
		fputs("Usage: <executable> [e/d] [encrypted file name]\n", stderr);
		return 1;
	}
	if((argv[1][0] & 0x5F) == 'D') enc = 0;
	else if((argv[1][0] & 0x5F) == 'E') enc = 1;
	else {
		fputs("Usage: <executable> [e/d] [encrypted file name]\n", stderr);
		return 2;
	}
	f = fopen(argv[2], enc ? "wb" : "rb");
	if(!f) {
		fputs("Usage: <executable> [e/d] [encrypted file name]\n", stderr);
		return 3;
	}
#ifndef _WIN32
	struct termios str, bak;
	tcgetattr(STDIN_FILENO, &str);
	bak = str;
	str.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &str);
#else
	conin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD str, bak;
	GetConsoleMode(conin, &str);
	bak = str;
	str &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
	SetConsoleMode(conin, str);
#endif
	if(enc)
		fputs("WARNING: if your first password and second password is same,\n"
			"    your encrypted data is dangerous.\n", stdout);
	fputs("WARNING: In this console,\n    you must type Ctrl-D key\n"
		"    when you complete type password.\n", stdout);
	if(enc) {
		lraw = input("Please type password to encrypted (no echo):",&raw);
		if (!raw) goto myfree;
	}
	lrpass = input("Please type first password (no echo):",&rpass);
	lriv = input("Please type second password (no echo):",&riv);
	if (!rpass || !riv) goto myfree;
#ifndef _WIN32
	tcsetattr(STDIN_FILENO, TCSANOW, &bak);
#else
	SetConsoleMode(conin, bak);
#endif
	SHA256(rpass, lrpass, pass);
	MD5(riv, lriv, iv);
	if(enc) {
		lout = aes256(raw, lraw, pass, iv, &out, 1);
		if (lout == 0) goto myfree;
		fwrite(out, 1, lout, f);
	} else {
		fseek(f, 0, SEEK_END);
		lraw = ftell(f);
		raw = malloc(lraw);
		if (!raw) goto myfree;
		fseek(f, 0, SEEK_SET);
		fread(raw, 1, lraw, f);
		lout = aes256(raw, lraw, pass, iv, &out, 0);
		if (lout == 0) goto myfree;
		fputs("Your password is\n", stdout);
		fwrite(out, 1, lout, stdout);
		fputs("\n", stdout);
	}
myfree:
	memset(iv, 0, 128 / 8);
	memset(pass, 0, 256 / 8);
	if(riv) {
		memset(riv, 0, lriv);
		free(riv);
	}
	if(rpass) {
		memset(rpass, 0, lrpass);
		free(rpass);
	}
	if(raw) {
		memset(raw, 0, lraw);
		free(raw);
	}
	if(out) {
		memset(out, 0, lout);
		free(out);
	}
	fclose(f);
	return 0;
}
