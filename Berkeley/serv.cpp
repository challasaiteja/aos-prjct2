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

using namespace std;

char buf[256];
int DLC;

int lamp_clock = 0, total = 0, avg = 0, c = 0, mc=0;

int Berkeley(long newsockfd)
{
	bzero(buf,256);

	stringstream s1, s2, s3;
	s1 << lamp_clock;
	string tmpstr1 = s1.str(); //int to string
	strcpy(buf,tmpstr1.c_str()); // string to const char *
	
	DLC = write((long)newsockfd,buf,strlen(buf));	// Sending logical clock value of server to the connected local machine.
	if (DLC < 0){
	       printf("Unable to write to socket");
	       return -1;
	}
	

	bzero(buf,256);
	read((long)newsockfd,buf,255); //Retrieving the time difference of the time daemon and the connected machines
	cout << "Time Difference of Machine '" << newsockfd <<" is "<< buf << endl;
	
	s2 << buf;
	string tmpstr2 = s2.str();
	
	int d = atoi(tmpstr2.c_str());	//converting Time Daemon's clock from char array to integer value

	total = total + d;			//Adding all time differences

	sleep(5);

	avg = total/(c+1);			//Taking average of the total time differences

	int adjusted_clock = avg - d;		//Calculating the average time adjustment for each clock

	bzero(buf,256);
	
	s3 << adjusted_clock;
	string tmpstr3 = s3.str();
	strcpy(buf,tmpstr3.c_str());		//Converting time adjustment value from integer to const char *
	DLC = write((long)newsockfd,buf,strlen(buf));	//Sending specific time adjustment to corresponding local machine
	
	if (DLC < 0){
	       printf("Unable to write to socket\n");
	       return -1;
	}
	return 0;
}


void *MultiConnect(void *newsockfd) //To handle the each client request a thread function is created
{
	if((long)newsockfd < 0){
	       printf("Unable to connect to new machine\n");
	       exit(0);
	} 
	
	cout << "I'm connected to a local machine "<<long(newsockfd) << endl;
	c++;

	while(c != mc)
	{
		continue;
	}

	Berkeley((long)newsockfd);
	
	close((long)newsockfd);
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	long sockfd, newsockfd[10];
	socklen_t len;
	
	struct sockaddr_in serv_addr, cli_addr;

	pthread_t threads[10];	//threads for handling client requests
	
	//https://en.cppreference.com/w/cpp/numeric/random/rand
	srand(time(0));	// Initiating the random function with current time as input
	lamp_clock = (rand()%50);	//Random numbers from 0 to 50
	
	if(argc != 2) {
                printf("Please provide port number in the command line argument\n");
                exit(0);
        }

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd < 0){
	       printf("Unableto create socket\n");
	       return -1;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	
	//https://pubs.opengroup.org/onlinepubs/000095399/functions/setsockopt.html
	int u=1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&u, sizeof(u)) < 0){ //Reusing the Socket address
		printf("Reusing the socket address failed");
		return -1;
	}

	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&u, sizeof(u)) < 0){ //Reusing the port number
	       printf("Reusing the portnumber failed");
	       return -1;
	}
	

	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		printf("Unable to bind");
		exit(0);
	}

	listen(sockfd,10);
	len = sizeof(cli_addr);

	cout << "How many machines need to connect?? ";
	cin >> mc;

	cout << "Logical timestamp of the server/timedaemon: " << lamp_clock << endl;

	int index = 0;

	for(int i=0; i < mc; i++) //creating threads fo each client
	{
		newsockfd[index] = accept(sockfd,(struct sockaddr *) &cli_addr,&len);

		pthread_create(&threads[i], NULL, MultiConnect, (void *)newsockfd[index]);
		
		index=(index+1)%100;
	}
	
	for(int i=0; i<mc; i++)
	{
		int j = pthread_join(threads[i], NULL);
		if(j)
		{
			printf("Thread joining failed :\n");
			cout << "Error: " << j << endl;
			exit(-1);
		}
	
	}

	cout << "Adjusted logical clock value: " << avg << endl;
	lamp_clock = lamp_clock + avg;

	cout << "Final logical clock value is " << lamp_clock << endl;

	close(sockfd);
	return 0; 
	pthread_exit(NULL);
}
