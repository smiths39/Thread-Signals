## Description

The program begins using a 'sigset', which is made public throughout the program. Within the main method, the 'sigset' is emptied of all available signals. 
Three signals are added to the 'sigset' (SIGUSR1, SIGUSR2, SIGALRM). 
We block certain signals being used by setting the 'sigmask' for our own personal use.

The File being used within the program is entered as an argument from the command prompt. 
The file is opened and set to 'read'. A 'struct' was used to store a char array of maximum length of 64. 
This is used to pass the word being read in as an argument throughout the program.

Three threads are created

1. printEven: printing an even numbered word
2. printOdd: printing an odd numbered wrd
3. mainThreadHandler: reading in a word from the file and deciding which printing thread to activate

Upon creation, each thread passes the word being read in from the file as an argument. The word being read in is stored within the char array.
 
The 'mainThreadHandler' is activated first by a signal (SIGALRM) being sent. The message read in is declared as a char. 

A 'while' loop is used to perform the read and print actions until the entire file has been read in. 
However, the reading process will not begin until a SIGALRM has been sent from a particular source, 
until then a 'sigwait' is used to prevent the process from continuing. 
An 'if' statement is used to make sure the reading process has performed 1 assignment, otherwise it is confirmed that the end of the file has been reached. 
This is used to prevent a duplication of the last word of the file being printed. 
A second 'if' statement is used to declare if the word read in was even numbered (SIGUSR1 is sent) or odd numbered (SIGUSR2 is sent).

The 'printEven' method reads in the word which was previous stored in the char array. 
A 'while' loop is again used to make sure the end of the file has not been reached. 
A 'sigwait' is used to prevent further action being taken until a signal (eg. SIGUSR1) has been sent. 
Before the word is printed to the screen, the thread sleeps for a random amount of real-time microseconds. 
This is enforced to prevent busy waiting. 
The random amount of time was generated from the 'getRandom' method. 
The thread then prints the message to the screen. 
Once the message has been printed, the thread sends a signal (SIGALRM) back to the 'mainThreadHandler' and the reading process is continued.
The 'printOdd' method has the exact same functionality as the 'printEven' method. 
The reason for this duplication was to clarify if the word read in from the file was even-numbered or odd-numbered.
A SIGUSR2 signal is sent to this thread for activation.

Each thread used within the program has a cancel state assigned to it. 
This is used for signal safety. 
In the beginning of each thread, its cancel state is disabled. This prevents any pre-emption or unexpected shut down of a thread. 
After the file has been read in completely each thread's cancellation state is enabled and they are allowed to be shut down safely.  

In the 'mainThreadHandler', when the file has been read in completely, the 'even' and 'odd' thread's cancellation is called. 
Before each cancellation is called, a print statement is provided to show the Thread is now finished its intended functionality. 
The file used within the program is closed. The 'mainThreadHandlers' cancellation state is enabled and is finally cancelled from the programs main method. 
Finally a join method is called upon the 'mainThreadHandler', this suspends the execution of the calling thread until the thread terminates. 
The program has then ended.





