#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdarg.h>

#include <pthread.h>
#include <sys/types.h>

#if __linux
#include <sys/syscall.h>
#elif defined(_WIN32) || defined(_WIN64)
    #include <windows.h>       // Or something like it. 
    #define gettid() GetCurrentThreadId()
#endif

void SIGINT_handler(){
    write(STDOUT_FILENO,"hi",2);
}

#include <unistd.h>
#include <sys/syscall.h>

void* proc(void* arg) {
    sigset_t threadBlock;

    // sigemptyset(&threadBlock);
    // sigaddset(&threadBlock, SIGTSTP);
    // sigaddset(&threadBlock, SIGHUP);
    // pthread_sigmask(SIG_UNBLOCK, &threadBlock, NULL);
    pid_t tid = gettid();
    pid_t pid = getpid();
    int sum = 0;

    for (int i = 0; i <= 10*tid; i++) {
        sum += i * tid;

        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "TID: %d, PID: %d\n", tid, pid);
        write(STDOUT_FILENO, buffer, len);

        sleep(1);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, SIGINT_handler);

    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;
    
    pthread_create(&thread1, NULL, proc, NULL);
    pthread_create(&thread2, NULL, proc, NULL);
    pthread_create(&thread3, NULL, proc, NULL);
    pthread_create(&thread4, NULL, proc, NULL);


    // sigset_t fullset;
    // sigfillset(&fullset);
    // pthread_sigmask(SIG_BLOCK, &fullset, NULL);


    printf("thread1: %p at memory address: %p\n",(void*)thread1,(void*)&thread1);
    printf("thread1: %p at memory address: %p\n",(void*)thread2,(void*)&thread2);
    printf("thread1: %p at memory address: %p\n",(void*)thread3,(void*)&thread3);
    printf("thread1: %p at memory address: %p\n",(void*)thread4,(void*)&thread4);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);

    // sigset_t emptyset;
    // sigemptyset(&emptyset);
    // pthread_sigmask(SIG_SETMASK, &emptyset, NULL);

}
