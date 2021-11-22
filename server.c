#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>

//display error
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
               do { perror(msg); exit(EXIT_FAILURE); } while (0)
				   
int day, port;
char *QOTD;
const char *goToQOTD;
pthread_mutex_t quoteLock=PTHREAD_MUTEX_INITIALIZER;
pthread_t checkForNewDayThread, connectionHandlerThread;

//This funciton for it to open and read the file that need
int BasedLine_no(const char* file){
	
    int count = 0;
    int currChar; //Current Char that read
    FILE* CFTR = fopen(file,"read_file.txt"); //Name of the file // CFTR mean Current File That Read
	
    if(CFTR==NULL){
        perror("Error opening the quotes file");
        exit(EXIT_FAILURE);
    }
    while(true){
        currChar=fgetc(CFTR);
        switch (currChar){
            case '\n':{
                count++;
                break;
            }
            case EOF:{
                fclose(CFTR);
                return count;
            }
            default:
                continue;
        }
    }
}

//to generate random line based on file and send to buffer address
char* quote_read(const char* filePath){
	
   int quotes_no=BasedLine_no(filePath);
   int lineNumberOfQOTD=rand()%quotes_no;
   int lineCounter=0;
   char* lineptr=NULL; //will contain the address of the buffer containing the quote
   size_t n=0; //lineptr=NULL and n=0 makes it automatically right amount of memory
   
   FILE* CFTR=fopen(filePath, "read_file.txt");
    if(CFTR==NULL){
        perror("Error opening the read file");
        exit(0);
    }
   while(lineCounter<lineNumberOfQOTD){ //seek based on the client input
       if(fgetc(CFTR)=='\n') lineCounter++;
   }
    getline(&lineptr, &n, CFTR); //stores the line read to pass for client
    fclose(CFTR);
    return lineptr;
}

//To make sure connection can connected
void * connection(void* port_ptr){
	
    struct sockaddr_in address;
    int server_CFTR, new_socket, opt = 1, addrlen = sizeof(address), port=*((int*) port_ptr);

    free(port_ptr);

    //creating socket file descriptor
    if ((server_CFTR = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //assign ip address and port to 1717
    if (setsockopt(server_CFTR, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))==-1)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(1717);

    //bind the socket to the port 1717
    if (bind(server_CFTR, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_CFTR, 100) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %i\n", port);

    while(1){ //connection with clients loop
        if ((new_socket = accept(server_CFTR, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0){
            perror("accept");
            exit(EXIT_FAILURE);
        }
        pthread_mutex_lock(&quoteLock);
        send(new_socket, QOTD, strlen(QOTD), 0);
        pthread_mutex_unlock(&quoteLock);
        close(new_socket);
    }
}


// main code to pass all the information and connection
int main(int argc, char const *argv[]){
	
    int thread1, thread2, join;
    int* port=malloc(sizeof(int));

    if(port==NULL){
	perror("Couldn't allocate memory to store listening port");
	exit(0);
    }

    time_t now; // `time_t` is an arithmetic time type
    time(&now); // Obtain current time
	
    switch(argc){ //Read settings from terminal or use defaults
        case 1:{ //No input from user, use defaults
            *port=1717;
            goToQOTD=strdup("read_file.txt");
            break;
        }
        case 3:{ //file and port specified by user
            *port=atoi(argv[2]);
            goToQOTD=argv[1];
            break;
        }
        default:{
            fprintf(stderr,"Bad arguments\n");
            exit(EXIT_FAILURE);
        }

    }

	printf("Username is %s", getlogin());
	printf("\n");
	printf("Today is %s", ctime(&now)); // Convert to local time format and print to stdout
	printf("\n");
	printf("The quotes file contains %i quotes\n", BasedLine_no(goToQOTD));
	
    QOTD = quote_read(goToQOTD); //No need to acquire lock here since no thread is even started
    connection(port);
	
    
	
    return 0;
}
