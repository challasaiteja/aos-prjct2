/* Taking the reference https://cis.temple.edu/~giorgio/old/cis307s97/readings/pbuffer.html */

#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

#define REQ 1
#define REL 2
#define FIN 3

using namespace std;

int main(int argc, char *argv[])
{

	long sockfd, l ;
	struct sockaddr_in serv_addr;
	struct hostent *ser;//https://man7.org/linux/man-pages/man3/gethostbyname.3.html
	int req;
	char buf[256];
	
	if(argc != 2) {
		printf("Please enter the port number in the command line argument");
		exit(0);
	}
	

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd < 0){
		printf("Unable to create a socket");
		return -1;
	}

	ser = gethostbyname("localhost");
	
	if(ser == NULL) {
		printf("Unknown host\n");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));////https://man7.org/linux/man-pages/man3/bzero.3.html
	serv_addr.sin_family = AF_INET;
	bcopy((char *)ser->h_addr, (char *)&serv_addr.sin_addr.s_addr, ser->h_length);
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
	       printf("Unable to connect");
	       return -1;
	}
	
	int a;
	cout<<"How many times will you need to update the counter?";
	cin>>a;

	while(a)
	{
		req=REQ;
		l = write(sockfd,&req,sizeof(int));// Sending a request to access the file
			if (l < 0){
			       printf("Unable to write to socket");
			       return -1;
			}

		cout << "Getting access to Shared File!" << endl; 
		
		l = read(sockfd,&req,sizeof(int));// Getting response from server
		
		if (l < 0){
		       printf("Unable to read from socket");
		       return -1;
		}

		cout << "Successfully accessed" << endl;
	
		ifstream shared_file("Values.txt");
	
		string msg;
		int c;

		if(shared_file.is_open())
		{
			getline(shared_file, msg);//retrieving data from file
			istringstream s(msg);
			s >> c;
			shared_file.close();
		}
		else cout << "No such file found"; 
	
		sleep(3);
	
		cout << "Initial value: " << c << endl;
		c++;

		ofstream my_shared_file("Values.txt");
		if(my_shared_file.is_open())
		{
			my_shared_file << c;//Incrementing the values in the file and updating it.
			my_shared_file.close();
		}
		else cout << "No file found";
	
		ifstream shared_file1("Values.txt");
	
		if(shared_file1.is_open())//To retrieve the updated values in the file.
		{
			getline(shared_file1, msg);	
			istringstream s1(msg);
			s1 >> c;
			shared_file1.close();
		}
		else cout << "No file found"; 
		cout << "Updated value of Counter: " << c << endl;
		req = REL;
		l = write(sockfd,&req,sizeof(int));

		if (l < 0) {
			printf("Unable to write to socket");
			return -1;
		}

		cout << "Released File Access!\n" << endl;

		a--;
	}

	req = FIN;
	l = write(sockfd,&req,sizeof(int));
	cout << "Successfully accessed values in the file and Mutual exclusion was implemented." << endl;
	if (l < 0) {
	       printf("Unable to write to socket");
	       return -1;
	}


	close(sockfd);
	return 0;
}
