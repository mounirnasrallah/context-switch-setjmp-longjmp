void mylongjmp(int idx);
int mysetjmp(int idx);
int main(int argc,char* argv[]);
void f();
void g();
void new_proc(void (*fun)(int), int arg);
void commut(int no);

