#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> //all includes her
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#if __linux
#include <sys/syscall.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>       // Or something like it. 
#endif
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void* proc(void* arg) {
    int sum=0;
    for(int i=0;i!=11;i++){
    pid_t tid = getpid();
    sum=sum+i*tid;
    sleep(1);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
pthread_t thread1;
pthread_t thread2;
pthread_t thread3;
pthread_t thread4;
pthread_create(&thread1, NULL, proc, NULL);
pthread_create(&thread2, NULL, proc, NULL);
pthread_create(&thread3, NULL, proc, NULL);
pthread_create(&thread4, NULL, proc, NULL);
// Creating a new thread

}