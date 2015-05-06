#include <sqlite3.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#if _WIN32
#include <windows.h>
#include <wincrypt.h>
#else
#include <openssl/evp.h>
#endif
#include "cookie.h"
char *getSession(){
  const char *select = "SELECT value, encrypted_value FROM cookies WHERE name = 'user_session' AND host_key LIKE '%nicovideo.jp';";
  char cookie[128];
#if _WIN32
  strcpy(cookie, getenv("LOCALAPPDATA"));
  strcat(cookie,"\\Google\\Chrome\\User Data\\Default\\Cookies");
#else
  strcpy(cookie, getenv("HOME"));
  strcat(cookie,"/.config/google-chrome/Default/Cookies");
#endif
  sqlite3 *db;
  sqlite3_stmt *stmt;
  if (SQLITE_OK != sqlite3_open_v2(cookie, &db, SQLITE_OPEN_READONLY, 0))
    return NULL;
  sqlite3_prepare_v2(db, select, strlen(select), &stmt, NULL);
  void *val = NULL;
  while (SQLITE_ROW == sqlite3_step(stmt)) {
    const char *tmp = (const char*) sqlite3_column_text(stmt, 0);
	if (tmp && strlen(tmp)) {
      val = malloc(strlen(tmp) + 1);
      strcpy(val, tmp);
      break;
    }
#if _WIN32
    DATA_BLOB w_src, w_dst;
    w_src.cbData = sqlite3_column_bytes(stmt, 1);
    w_src.pbData = (void*) sqlite3_column_blob(stmt, 1);
    if (CryptUnprotectData(&w_src, NULL, NULL, NULL, NULL,
        CRYPTPROTECT_UI_FORBIDDEN, &w_dst)) {
      val = malloc(w_dst.cbData + 1);
      memcpy(val, w_dst.pbData, w_dst.cbData);
      ((char*)val)[w_dst.cbData] = 0;
      LocalFree(w_dst.pbData);
      break;
    }
#else
	const void *src = sqlite3_column_blob(stmt, 1);
	int lenu = sqlite3_column_bytes(stmt, 1), lenf = 0;
	if (lenu < 4 || memcmp(src, "v10", 3)) continue;
	src += 3; lenu -= 3;
	val = malloc(lenu + 1);
	memset(val, 0, lenu + 1);
	void *key = malloc(16);
	void *pw = "peanuts";
	void *salt = "saltysalt";
	void *iv = "                ";
	PKCS5_PBKDF2_HMAC_SHA1(pw, 7, salt, 9, 1, 16, key);
	EVP_CIPHER_CTX ecc;
	EVP_CIPHER_CTX_init(&ecc);
	EVP_DecryptInit_ex(&ecc, EVP_aes_128_cbc(), NULL, key, iv);
	EVP_DecryptUpdate(&ecc, val, &lenu, src, lenu);
	EVP_DecryptFinal_ex(&ecc, val + lenu, &lenf);
	EVP_CIPHER_CTX_cleanup(&ecc);
	free(key);
	break;
#endif
  }
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return val;
}
