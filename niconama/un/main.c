extern void getalertdata();
void *getliveinfo(void*);

extern char *user_session;

int main(int argc, char *argv[]) {
  user_session = argv[1];
  if(argc==3)
    getliveinfo(argv[2]);
  else
    getalertdata();
  return 0;
}