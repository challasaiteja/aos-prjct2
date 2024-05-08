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
using namespace std;

int main(int argc, char *argv[])
{
	if(argc != 2) {
                printf("Please provide port number in the command line argument\n");
                exit(0);
        }

	long sockfd, DLC;
	struct sockaddr_in serv_addr;
	struct hostent *ser; //https://man7.org/linux/man-pages/man3/gethostbyname.3.html
	int lamp_clock = 0;
	char buf[256];

	//https://en.cppreference.com/w/cpp/numeric/random/rand
	srand(time(0));	// Initiating the random function with current time as input
	lamp_clock = (rand()%50);// Defining the range of random numbers from 0 to 50

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	//creating socket
	if(sockfd < 0){
		printf("Unable to create socket\n");
		return -1;
	}
	
	ser = gethostbyname("localhost"); //localhost name is given in the function, which doesn't perform a lookup.
	
	if(ser == NULL) {
		printf("Unknown host\n");
		return -1;
	}
	

	bzero((char *) &serv_addr, sizeof(serv_addr));//https://man7.org/linux/man-pages/man3/bzero.3.html
	serv_addr.sin_family = AF_INET;
	bcopy((char *)ser->h_addr, (char *)&serv_addr.sin_addr.s_addr, ser->h_length);
	serv_addr.sin_port = htons(atoi(argv[1]));
	
	if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
	       printf("Connection Failed\n");
	       return -1;
	}
	//cout<<"I'm machine: "<<long(sockfd)<<endl;	
	
	cout << "This machine's Logical Clock value: " << lamp_clock << endl; // Printing machine's logical timestamp

	bzero(buf,256);
	DLC = read(sockfd,buf,255); //Getting the logical clock value of Time Daemon.
					
	if (DLC < 0) {
	       printf("Unable to read the value from socket\n");
	       return -1;
	}	       
	
	cout << "Implementing the Berkeley's Algorithm\n"<<"The logical timestamp of Time Daemon is: " << buf << endl;	// Printing Time Daemon's Logical Clock
	
	stringstream s1, s2;
	
	s1 << buf;
	string tempstr = s1.str();
	
	int tmp = atoi(tempstr.c_str()); //Converting char array to integer.
	
	int diff = lamp_clock - tmp;// Calculating time difference of local machine from Time Daemon
	cout << "Time Difference from Daemon's Process: "<< diff << endl;
	
	bzero(buf,256);

	s2 << diff;
	string tmpstr1 = s2.str();
	strcpy(buf,tmpstr1.c_str());

	DLC = write(sockfd,buf,strlen(buf));// Sending this machine's Time Difference to Time Daemon
	if (DLC < 0) {
	       printf("Unable to write to socket\n");
	       return -1;
	}


	bzero(buf,256);
	DLC = read(sockfd,buf,255);	//Getting the average value which is to be adjusted in local machine's logical timestamp.
	cout<<"Adjusted Logical Clock value is:"<<buf<<endl;

	int diff_clock = atoi(buf);
	
	lamp_clock = lamp_clock + diff_clock;

	cout <<  lamp_clock<<" is the final logical clock value of this machine" << endl;

	close(sockfd);	
	return 0;
}
