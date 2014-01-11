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
int aes256(const unsigned char* raw, int len, const unsigned char key[256/8],
	const unsigned char iv[128/8], unsigned char** enc) {
	EVP_CIPHER_CTX evpctx;
	int elen = len + AES_BLOCK_SIZE, flen = 0;
	EVP_CIPHER_CTX_init(&evpctx);
	EVP_EncryptInit_ex(&evpctx, EVP_aes_256_cbc(), NULL, key, iv);
	*enc = malloc(elen);
	memset(*enc, 0, elen);
	EVP_EncryptInit_ex(&evpctx, NULL, NULL, NULL, NULL);
	EVP_EncryptUpdate(&evpctx, *enc, &elen, raw, len);
	EVP_EncryptFinal_ex(&evpctx, *enc + elen, &flen);
	return elen + flen;
}
int aes256d(const unsigned char* raw, int len, const unsigned char key[256/8],
	const unsigned char iv[128/8], unsigned char** dec) {
	EVP_CIPHER_CTX evpctx;
	int dlen = len, flen = 0;
	EVP_CIPHER_CTX_init(&evpctx);
	EVP_DecryptInit_ex(&evpctx, EVP_aes_256_cbc(), NULL, key, iv);
	*dec = malloc(dlen+AES_BLOCK_SIZE);
	memset(*dec, 0, dlen+AES_BLOCK_SIZE);
	EVP_DecryptInit_ex(&evpctx, NULL, NULL, NULL, NULL);
	EVP_DecryptUpdate(&evpctx, *dec, &dlen, raw, len);
	EVP_DecryptFinal_ex(&evpctx, *dec + dlen, &flen);
	return dlen + flen;
}
#ifdef _WIN32
static HANDLE conin;
#endif
size_t input(const char* msg, unsigned char **buf) {
	unsigned char *cur, *end;
	size_t s = 8;
	*buf = cur = malloc(s);
	memset(*buf, 0, s);
	end = *buf + s;
#ifndef _WIN32
	__fpurge(stdin);
#else
	FlushConsoleInputBuffer(conin);
#endif
	fputs(msg, stdout);
	do {
		fread(cur, 1, 1, stdin);
		if(end < cur) {
			s <<= 1;
			end = realloc(*buf, s);
			cur += end - *buf;
			*buf = end;
			end = *buf + s;
		}
	} while(*cur++ != 0x04);
	cur--;
	fputc('\n', stdout);
	return cur - *buf;
}
int main(int argc, char* argv[]) {
	if(argc!=3) {
		fputs("Usage: <executable> [e/d] [encrypted file name]\n", stderr);
		return 1;
	}
	int enc;
	if((argv[1][0] & 0x5F) == 'D') enc = 0;
	else if((argv[1][0] & 0x5F) == 'E') enc = 1;
	else {
		fputs("Usage: <executable> [e/d] [encrypted file name]\n", stderr);
		return 2;
	}
	FILE *f;
	f = fopen(argv[2], enc ? "wb" : "rb");
	if(!f) {
		fputs("Usage: <executable> [e/d] [encrypted file name]\n", stderr);
		return 3;
	}
	unsigned char *raw, *rpass, *riv, pass[256 / 8], iv[128 / 8], *out;
	size_t lraw, lrpass, lriv, lout;
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
	if(enc)
		lraw = input("Please type password to encrypted (no echo):",&raw);
	lrpass = input("Please type first password (no echo):",&rpass);
	lriv = input("Please type second password (no echo):",&riv);
#ifndef _WIN32
	tcsetattr(STDIN_FILENO, TCSANOW, &bak);
#else
	SetConsoleMode(conin, bak);
#endif
	SHA256(rpass, lrpass, pass);
	MD5(riv, lriv, iv);
	if(enc) {
		lout = aes256(raw, lraw, pass, iv, &out);
		fwrite(out, 1, lout, f);
	} else {
		fseek(f, 0, SEEK_END);
		lraw = ftell(f);
		raw = malloc(lraw);
		fseek(f, 0, SEEK_SET);
		fread(raw, 1, lraw, f);
		lout = aes256d(raw, lraw, pass, iv, &out);
		fputs("Your password is\n", stdout);
		fwrite(out, 1, lout, stdout);
		fputs("\n", stdout);
	}
	fclose(f);
	return 0;
}
