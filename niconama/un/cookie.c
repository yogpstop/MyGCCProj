#include <sqlite3.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
char *user_session;
int exec_callback(void *arg, int argc, char **argv, char **column) {
  user_session = malloc(strlen(argv[0])+1);
  strcpy(user_session,argv[0]);
  return SQLITE_OK;
}

void getSession(){
  const char *select = "SELECT value FROM cookies WHERE name = 'user_session' AND host_key LIKE '%nicovideo.jp';";
  char cookie[128];
  strcpy(cookie, getenv("HOME"));
  strcat(cookie,"/.config/google-chrome/Default/Cookies");

  char *errmsg;
  sqlite3 *db;

  if (sqlite3_open_v2(cookie, &db, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
    return;

  sqlite3_exec(db, select, exec_callback, NULL, &errmsg);
  sqlite3_free(errmsg);
  sqlite3_close(db);
}