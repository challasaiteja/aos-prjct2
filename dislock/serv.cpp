/* Taking the reference https://cis.temple.edu/~giorgio/old/cis307s97/readings/pbuffer.html */


#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <queue>
#include <list>

using namespace std;

#define REQ 1
#define REL 2
#define FIN 3
#define OK 4


queue<long> r;	//To maintain waiting requests
list<long> l;	//To maintain list of connected clients
int c = 0;
pthread_mutex_t rLock, cLock;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER; //https://linux.die.net/man/3/pthread_cond_init

void *MultiConnect(void *newsockfd) //thread function for each client request
{
	int req, n;
	long *ptr = (long *)newsockfd;
	long s = *ptr;
	if(s < 0){
	       printf("Unable to accept new client");
	       exit(0);
	}

	cout << "Connected to Client " << s << endl;
	
	while(1)
	{
		
		n = read(s,&req,sizeof(int));	//Receiving the Client's Request
		if (n < 0) {
			printf("Unable to read from socket");
			exit(0);
		}
		
		if(req==REQ)				//Requesting access to shared file
		{
			cout << "Client " << s << " requesting access for the file." << endl;
			pthread_mutex_lock(&rLock);
			r.push(s);				// Adding client requests to queue
			pthread_mutex_unlock(&rLock);
		}
		else if(req==REL)				//Releasing the file access
		{
			pthread_mutex_lock(&cLock);
			if(c == 1)				// if critical section is acquired 
			{
				c = 0;				// Release the critical section
				cout << "Lock released by client: " << s << endl;
				pthread_cond_signal(&cv);	// Signal the conditional variable
			}
			pthread_mutex_unlock(&cLock);
		}
		else if(req==FIN)				//Exiting the loop, whenever client finishes the accessing.
		{
			cout << "Successfully file was accessed by client: " << s << endl;
			break;
		}
	}

	
	close(s);
	pthread_exit(NULL);
}

void *Permission(void *arg) 							// thread function for granting access to Shared File to each client
{
	long permgrant;
	int req, n;
	cout << "Accessing........" << endl;	
	while(1)
	{
		if(l.empty())							// When there are no active clients, exit the thread
		{
			break;
		}
		pthread_mutex_lock(&cLock);
		if(c == 1)							// When there is someone in the critical section
		{				
			pthread_cond_wait(&cv, &cLock);			// Wait on the conditional variable
		}
		pthread_mutex_lock(&rLock);
		if(!r.empty())							// When there are requests pending in the queue
		{
			c = 1;							// Mark the critical section as taken 
			permgrant = r.front();				
			r.pop();						// Remove the request from queue
			req = OK;
			n = send(permgrant,&req,sizeof(int), 0);			// Sending Permission to Access
				if (n < 0){ 
					printf("Unable write to socket");
					exit(0);
				}
			cout << "Permission Granted to client: " << permgrant  << endl; 
		}
		pthread_mutex_unlock(&rLock);
		pthread_mutex_unlock(&cLock);
	}
	
	return 0;	
}


int main(int argc, char *argv[])
{
	int t;
	long sockfd, newsockfd[10];
	socklen_t clilen;
	
	struct sockaddr_in serv_addr, cli_addr;

	pthread_t threads[10];					// threads for handling client requests
	pthread_t access;

	if (argc!= 2) {
		printf("Please provide port number in the command line argument\n");
		return -1;
	}

	if(pthread_mutex_init(&rLock, NULL) != 0)		// Initializing mutex object
	{
		cout << "Unable to initialize Queue mutex" << endl;
	}

	if(pthread_mutex_init(&cLock, NULL) != 0)		// Initializing mutex object
	{
		cout << "Unable to initialize Conditional Variable mutex" << endl;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd < 0) {
		printf("Unable to open socket");
		return -1;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));////https://man7.org/linux/man-pages/man3/bzero.3.html
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	int u = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&u, sizeof(u)) < 0){
		printf("Unable to reuse the address");
	}


	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&u, sizeof(u)) < 0){
	       printf("Unable to reuse the port number");
	}
	
	

	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		printf("Binding failed");
		return -1;
	}

	listen(sockfd,10);
	clilen = sizeof(cli_addr);

	cout << "How many number of clients to connect? ";
	cin >> t;

	int ind = 0;

	for(int i=0; i < t; i++)
	{
		newsockfd[ind] = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);

		l.push_back(newsockfd[ind]);

		pthread_create(&threads[i], NULL, MultiConnect, (void *)&newsockfd[ind]);
		
		ind= ind + 1;
	}
	
	pthread_create(&access, NULL, Permission, NULL);

	for(int i=0; i<t; i++)
	{
		int tj = pthread_join(threads[i], NULL);
		if(tj)
		{
			printf("Error in joining thread :\n");
			cout << "Error: " << tj << endl;
			exit(-1);
		}
	
	}

	pthread_mutex_destroy(&rLock);
	pthread_mutex_destroy(&cLock);

	close(sockfd);
	return 0; 
	pthread_exit(NULL);
}
