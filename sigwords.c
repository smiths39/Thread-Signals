#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

typedef struct
{
	char word[64];
} Structure;

/* made public for use throughout program */
sigset_t set;
FILE *pFile;
pthread_t mainThread, evenThread, oddThread;

/* returns a random number for sleeping use */
static double getRandom()
{
	double randomNumber = rand() / 10000;	
	return randomNumber;
}

static void *mainThreadHandler(void *ptr)
{
	/* prevent main thread handler being cancelled */       
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

	int sig;
	char *message = (char *)ptr;
	
	while(!feof(pFile))
	{	
	    /* wait for a signal (SIGALRM) */
	    
		sigwait(&set, &sig);

        /* if fscanf performed 1 assignment */
		if(fscanf(pFile, "%s", message)==1)
		{	
			if(strlen(message) % 2 == 0)		
			    /* send SIGUSR1 signal to print even thread */	
				pthread_kill(evenThread, SIGUSR1);
			else    
			    /* send SIGUSR2 signal to print odd thread */
				pthread_kill(oddThread, SIGUSR2);
		}
		else
		{
		    /* last word has been printed, cancelling other threads */
			printf("Goodbye from Thread 1\n");
			pthread_cancel(evenThread);
			printf("Goodbye from Thread 2\n");
			pthread_cancel(oddThread);						
		}
	}
	
	/* close the file */
	fclose(pFile);	
	
	/* grant permission for main thread handler to be cancelled */   
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);	
	return ((void *)NULL);
}

static void *printEven(void *ptr)
{
    /* prevent thread from being cancelled */
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

	int sig;
	double randomNum = getRandom();	
	char *message = (char *)ptr;
	
	while(!feof(pFile))
	{
	    /* wait for a signal to be sent (SIGUSR1) */
		sigwait(&set, &sig);
		
		usleep(randomNum);
		printf("Thread 1: ");
		printf("%s\n", message);
		    
		/* reactivate main thread handler */
		pthread_kill(mainThread, SIGALRM);
	}
	
	/* grant permission for thread to be cancelled */
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	return ((void *)NULL);
}


static void *printOdd(void *ptr)
{
    /* prevent thread from being cancelled */
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

	int sig;
	double randomNum = getRandom();	
	char *message = (char *)ptr;
	
	while(!feof(pFile))
	{
		/* wait for a signal to be sent (SIGUSR2) */
	    sigwait(&set, &sig);
	
		usleep(randomNum);
		printf("Thread 2: ");
		printf("%s\n", message);
		
		/* reactivate main thread handler */
		pthread_kill(mainThread, SIGALRM);
	}

	/* grant permission for thread to be cancelled */	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	return ((void *)NULL);
}

int main(int argc, char *argv[])
{
    /* empty all signals from set */
	sigemptyset(&set);
	
	/* add signals to used in program */ 
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGUSR2);
	sigaddset(&set, SIGALRM);
	
	/* indicating that the set of signals will be blocked */
	pthread_sigmask(SIG_SETMASK, &set, NULL);
	
	/* open the file passed as a command line parameter */
	pFile = fopen(argv[1], "r");

	Structure structure;

	pthread_create(&mainThread, NULL, mainThreadHandler, (void *)structure.word);	
	pthread_create(&evenThread, NULL, printEven, (void *)structure.word);
	pthread_create(&oddThread, NULL, printOdd, (void *)structure.word);
	
    /* activate the main thread handler */
   	pthread_kill(mainThread, SIGALRM);

    /* cancelling the last thread */
	pthread_cancel(mainThread);

	pthread_join(mainThread, NULL);
	
	return 0;
}	
