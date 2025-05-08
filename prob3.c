
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

#if __linux__
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
    printf("Recipient PID: %d\nRecipient TID: %ld\n", getpid(), gettid());
    if (info != NULL) {
        printf("Sender PID: %d\n", info->si_pid);
    }

    return;
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

    for (int i = 0; i <= 10; i++) {
        sum += i * tid;

        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "TID: %d, PID: %d\n", tid, pid);
        write(STDOUT_FILENO, buffer, len);

        sleep(1);
    }
    
    return;
}

int main(int argc, char *argv[]) {
    // Set up signal blocking for main thread
        sigset_t fullset;
        sigfillset(&fullset);
        pthread_sigmask(SIG_BLOCK, &fullset, NULL);
    //

    pthread_t main_thread = pthread_self();
    pid_t mainTid = gettid();
    printf("Main thread ID: %d, Pid: %d\n", mainTid, getpid());
    
    //signal(SIGINT, SIGINT_handler);

    //make threads
    pthread_t threads[4];
    for(int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, proc, NULL);
        printf("thread%d: %p at memory address: %p\n", i,(void*)threads[i],(void*)&threads[i]);      
    }

    sleep(1);

    for(int i = 0; i < 4; i++) {
        printf("Sending SIGINT to thread%d: %p\n",i, (void*)threads[i]);
        pthread_kill(threads[i], SIGINT);
        sleep(1);
        printf("Sending SIGCHLD to thread%d: %p\n",i, (void*)threads[i]);
        pthread_kill(threads[i], SIGCHLD);
        sleep(1);
        printf("Sending SIGHUP to thread%d: %p\n", (void*)threads[i]);
        pthread_kill(threads[i], SIGHUP);
        sleep(1);
        printf("Sending SIGTSTP to thread%d: %p\n", (void*)threads[i]);
        pthread_kill(threads[i], SIGTSTP);
        sleep(1);
        printf("Sending SIGSEGV to thread%d: %p\n",i, (void*)threads[i]);
        pthread_kill(threads[i], SIGSEGV);
        sleep(1);
        printf("Sending SIGFPE to thread%d: %p\n",i, (void*)threads[i]);
        pthread_kill(threads[i], SIGFPE);
        sleep(1);
    }

    sleep(1);

    for(int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }
    
    sigset_t emptyset;
    sigemptyset(&emptyset);
    pthread_sigmask(SIG_SETMASK, &emptyset, NULL);

    sleep(10);

}
