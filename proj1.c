#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdarg.h>
#define MSGSIZE 16
#include <stdio.h> //all includes her
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
void* handler1(void* arg) {
    int sum=0;
    for(int i=0;i!=11+1;i++){
    pid_t tid = syscall(SYS_gettid);
    sum=sum+i*tid;
    sleep(1);
    }
    return NULL;
}
void* handler2(void* arg) {
    int sum=0;
    for(int i=0;i!=11+1;i++){
    pid_t tid = syscall(SYS_gettid);
    sum=sum+i*tid;
    sleep(1);
    }
    return NULL;
}
void* handler3(void* arg) {
    int sum=0;
    for(int i=0;i!=11+1;i++){
    pid_t tid = syscall(SYS_gettid);
    sum=sum+i*tid;
    sleep(1);
    }
    return NULL;
}
void* handler4(void* arg) {
    int sum=0;
    for(int i=0;i!=11+1;i++){
    pid_t tid = syscall(SYS_gettid);
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
pthread_create(&thread1, NULL, handler1, NULL);
pthread_create(&thread2, NULL, handler2, NULL);
pthread_create(&thread3, NULL, handler3, NULL);
pthread_create(&thread4, NULL, handler4, NULL);
// Creating a new thread

}