extern void getalertdata();
void *getliveinfo(void*);

extern void getSession();

int main(int argc, char *argv[]) {
  getSession();
  if(argc==2)
    getliveinfo(argv[1]);
  else
    getalertdata();
  return 0;
}