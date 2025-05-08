
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
    //write(STDOUT_FILENO,"hi I",2);
    printf("Received signal %d\n", signo);
    printf("\tRecipient PID: %d\n\tRecipient TID: %ld\n", getpid(), gettid());
    if (info != NULL) {
        printf("\tSender PID: %d\n", info->si_pid);
    }
}

void printPending(sigset_t pending) {
    for(int sig = 1; sig < NSIG; sig++) {
        if (sigismember(&pending, sig)) {
            printf("[Thread %ld] Signal %d is pending\n", gettid(), sig);
        }
    }
}

void makeThreads(pthread_t* threads, int num_threads) {
    for (int i = 0; i < num_threads; i++) {
        if (i < num_threads / 2)
            pthread_create(threads[i], NULL, proc, NULL);
        else
            pthread_create(threads[i], NULL, proc2, NULL);
    }
}


void* proc(void* arg) {
    sigset_t pending;
    sigpending(&pending);
    printf("created thread: %ld\n", gettid());
    sleep(10);
    // Set up signal handling for threads
        sigset_t mask_set;

        sigfillset(&mask_set);
        
        sigdelset(&mask_set, SIGINT);
        sigdelset(&mask_set, SIGABRT);
        sigdelset(&mask_set, SIGTSTP);
        sigdelset(&mask_set, SIGILL);

        pthread_sigmask(SIG_SETMASK, &mask_set, NULL);
    //

    pid_t tid = gettid();
    pid_t pid = getpid();
    int sum = 0;

    for (int i = 0; i <= 10; i++) {
        sum += i * tid;

        printPending(pending);
        // int len = snprintf(buffer, sizeof(buffer), "TID: %d, PID: %d\n", tid, pid);
        // write(STDOUT_FILENO, buffer, len);

        sleep(1);
    }
    
}

void* proc2(void* arg) {
    sigset_t pending;
    sigpending(&pending);
    printf("created thread: %ld\n", gettid());
    sleep(10);

    // Set up signal handling for threads
        sigset_t mask_set;

        sigfillset(&mask_set);

        sigdelset(&mask_set, SIGCHLD);
        sigdelset(&mask_set, SIGHUP);
        sigdelset(&mask_set, SIGSEGV);
        sigdelset(&mask_set, SIGFPE);
        //all other signals are still blocked from main thread
        //blocked signals include SIGHUP, SIGTSTP
        
        pthread_sigmask(SIG_SETMASK, &mask_set, NULL);
    //

   


    pid_t tid = gettid();
    pid_t pid = getpid();
    int sum = 0;

    for (int i = 0; i <= 10; i++) {
        sum += i * tid;
        //see peding
        printPending(pending);
        // char buffer[64];
        // int len = snprintf(buffer, sizeof(buffer), "TID: %d, PID: %d\n", tid, pid);
        // write(STDOUT_FILENO, buffer, len);

        sleep(1);
    }


}

pthread_t main_thread;
pid_t mainTid;

int main(int argc, char *argv[]) {
    //setup pending
        sigset_t pending;
        sigpending(&pending);
    //block SIGINT, SIGQUIT, SIGTSTP
        sigset_t mask_set;
        
        sigaddset(&mask_set, SIGINT);
        sigaddset(&mask_set, SIGQUIT);
        sigaddset(&mask_set, SIGTSTP);

        pthread_sigmask(SIG_SETMASK, &mask_set, NULL);
    //ignore all signals
        // struct sigaction sigac;
        // memset(&sigac, 0, sizeof(sigac));
        // sigac.sa_handler = SIG_IGN;
        // sigemptyset(&sigac.sa_mask);
        // sigac.sa_flags = 0;

        // Loop through valid signals (1 to NSIG-1)
        // for (int sig = 1; sig < NSIG; sig++) {
        //     if (sig == SIGKILL || sig == SIGSTOP) {
        //         continue;
        //     }

        //     if (sigaction(sig, &sigac, NULL) == -1) {
        //         fprintf(stderr, "Could not ignore signal %d\n", sig);
        //     }
        // }
    
    //send signals to self
        // int signals[] = {
        //     SIGINT, 
        //     //SIGABRT,
        //     SIGILL,
        //     SIGCHLD,
        //     //SIGSEGV,
        //     SIGFPE,
        //     //SIGHUP,
        //     SIGTSTP
        // };
        
        // int num_signals = sizeof(signals) / sizeof(signals[0]);
        
        // for (int i = 0; i < num_signals; i++) {
        //     for (int j = 0; j < 3; j++) {
        //         kill(getpid(), signals[i]);
        //     }
        // }



    main_thread = pthread_self();
    mainTid = gettid();
    printf("Main thread ID: %d, Pid: %d\n", mainTid, getpid());
    
    sleep(10);
    printf("Pending for main:\n");
    printPending(pending);
    //make threads
        static pthread_t threads[4];
        makeThreads(threads, 4);
    //
    
    //signal(SIGINT, SIGINT_handler);
    //setup handler
        struct sigaction sigac;
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

    //unblock SIGINT, SIGQUIT, SIGTSTP
        sigemptyset(&mask_set);
        pthread_sigmask(SIG_SETMASK, &mask_set, NULL);

    sleep(120);

    // for(int i = 0; i < 4; i++) {
    //     printf("Sending SIGINT to thread%d: %p\n",i, (void*)threads[i]);
    //     pthread_kill(threads[i], SIGINT);
    //     sleep(1);
    //     printf("Sending SIGCHLD to thread%d: %p\n",i, (void*)threads[i]);
    //     pthread_kill(threads[i], SIGCHLD);
    //     sleep(1);
    //     printf("Sending SIGHUP to thread%d: %p\n",i,  (void*)threads[i]);
    //     pthread_kill(threads[i], SIGHUP);
    //     sleep(1);
    //     printf("Sending SIGTSTP to thread%d: %p\n", i,(void*)threads[i]);
    //     pthread_kill(threads[i], SIGTSTP);
    //     sleep(1);
    //     printf("Sending SIGSEGV to thread%d: %p\n",i, (void*)threads[i]);
    //     pthread_kill(threads[i], SIGSEGV);
    //     sleep(1);
    //     printf("Sending SIGFPE to thread%d: %p\n",i, (void*)threads[i]);
    //     pthread_kill(threads[i], SIGFPE);
    //     sleep(1);
    // }

    sleep(1);

    for(int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    //Restore default signal handlers
        sigac.sa_handler = SIG_DFL;
        sigemptyset(&sigac.sa_mask);
        sigac.sa_flags = 0;
    //

    sleep(10);

    return 0;
}
