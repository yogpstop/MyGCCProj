#include <sqlite3.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#if _WIN32
#include <windows.h>
#include <wincrypt.h>
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
  char *val = NULL;
  while (SQLITE_ROW == sqlite3_step(stmt)) {
    const char *tmp = (const char*) sqlite3_column_text(stmt, 0);
	if (tmp && strlen(tmp)) {
      val = malloc(strlen(tmp) + 1);
      strcpy(val, tmp);
      break;
    }
    DATA_BLOB w_src, w_dst;
    w_src.cbData = sqlite3_column_bytes(stmt, 1);
    w_src.pbData = (void*) sqlite3_column_blob(stmt, 1);
    if (CryptUnprotectData(&w_src, NULL, NULL, NULL, NULL,
        CRYPTPROTECT_UI_FORBIDDEN, &w_dst)) {
      val = malloc(w_dst.cbData + 1);
      memcpy(val, w_dst.pbData, w_dst.cbData);
      val[w_dst.cbData] = 0;
      LocalFree(w_src.pbData);
      break;
    }
  }
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  return val;
}
