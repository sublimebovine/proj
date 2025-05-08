
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

#if __linux
    #include <sys/syscall.h>
    #define gettid() syscall(SYS_gettid)
#elif defined(_WIN32) || defined(_WIN64)
    #include <windows.h>       // Or something like it. 
    #define gettid() GetCurrentThreadId()
#endif

void SIGINT_handler(int signo, siginfo_t *info, void *context){
    struct sigaction sigac;

    //write(STDOUT_FILENO,"hi I",2);
    printf("Received signal %d\n", signo);
    printf("Recipient PID: %d\nRecipient TID: %d\n", getpid(), gettid());   // or syscall(SYS_gettid) for thread-level
    if (info != NULL) {
        printf("Sender PID: %d\n", info->si_pid);
    }
    return NULL;
}



void* proc(void* arg) {
    // Set up signal handling for threads
        sigset_t allow;
        sigemptyset(&allow);

        sigaddset(&allow, SIGINT);
        sigaddset(&allow, SIGCHLD);
        sigaddset(&allow, SIGHUP);
        sigaddset(&allow, SIGTSTP);
        sigaddset(&allow, SIGSEGV);
        sigaddset(&allow, SIGFPE);
        //all other signals are still blocked from main thread
        //blocked signals include SIGHUP, SIGTSTP
        
        pthread_sigmask(SIG_UNBLOCK, &allow, NULL);
    //

    //setup handler
        struct sigaction sigac;
        memset(&sigac, 0, sizeof(sigac));
        sigac.sa_sigaction = SIGINT_handler;
        sigac.sa_flags = SA_SIGINFO | SA_RESTART;

        //not blocking: SIGINT, SIGABRT, SIGILL, SIGCHLD,
        //blocking during handler: SIGSEGV, SIGFPE
        //permenantly blocked: SIGHUP, SIGTSTP

        //set up signal blocking during handing
            sigemptyset(&sigac.sa_mask);
            sigaddset(&sigac.sa_mask, SIGSEGV);
            sigaddset(&sigac.sa_mask, SIGFPE);
        //

        sigaction(SIGINT, &sigac, NULL);
        sigaction(SIGHUP, &sigac, NULL);
        sigaction(SIGTSTP, &sigac, NULL);
        sigaction(SIGCHLD, &sigac, NULL);
    //

    pid_t tid = gettid();
    pid_t pid = getpid();
    int sum = 0;

    for (int i = 0; i <= 10*tid; i++) {
        sum += i;

        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "TID: %d, PID: %d\n", tid, pid);
        write(STDOUT_FILENO, buffer, len);

        sleep(1);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
    // Set up signal blocking for main thread
        sigset_t fullset;
        sigfillset(&fullset);
        pthread_sigmask(SIG_BLOCK, &fullset, NULL);
    //
    
    //signal(SIGINT, SIGINT_handler);

    //make threads
    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;
    
    pthread_create(&thread1, NULL, proc, NULL);
    pthread_create(&thread2, NULL, proc, NULL);
    pthread_create(&thread3, NULL, proc, NULL);
    pthread_create(&thread4, NULL, proc, NULL);

    printf("thread1: %p at memory address: %p\n",(void*)thread1,(void*)&thread1);
    printf("thread2: %p at memory address: %p\n",(void*)thread2,(void*)&thread2);
    printf("thread3: %p at memory address: %p\n",(void*)thread3,(void*)&thread3);
    printf("thread4: %p at memory address: %p\n",(void*)thread4,(void*)&thread4);

    sleep(1);

    printf("Sending SIGINT to thread1: %p\n", (void*)thread1);
    pthread_kill(thread1, SIGINT);
    sleep(1);
    printf("Sending SIGCHLD to thread2: %p\n", (void*)thread2);
    pthread_kill(thread2, SIGCHLD);
    sleep(1);
    printf("Sending SIGHUP to thread3: %p\n", (void*)thread3);
    pthread_kill(thread3, SIGHUP);
    sleep(1);
    printf("Sending SIGTSTP to thread4: %p\n", (void*)thread4);
    pthread_kill(thread4, SIGTSTP);
    sleep(1);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);

    sigset_t emptyset;
    sigemptyset(&emptyset);
    pthread_sigmask(SIG_SETMASK, &emptyset, NULL);

}
