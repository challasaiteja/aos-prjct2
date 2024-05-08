#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <fstream>
#include <regex>
#include <bits/stdc++.h>
using namespace std;
   
#define IPADDR "121.0.0.1"

int portno;
string vecclk = "0,0,0,";
int ports[3]= {8000, 8001, 8002};//fixed port numbers
  
void* client(void* cli) { 
   	struct sockaddr_in addr1;
	int clk[3];
	int xi=0;
   	int addrlen1, sockfd1, c;
   	struct ip_mreq mreq1;
   	char message[50];

	struct sockaddr_in addr2;
   	int addrlen2, sockfd2, cnt2;
   	struct ip_mreq mreq2;

   	sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);
	sockfd2 = socket(AF_INET, SOCK_DGRAM, 0);
   	if (sockfd1 < 0) {
     		printf("unable to create socket");
     		exit(1);
   	}

	int a = 0;
	int b = 0;

	for (int i =0; i < 3; i++){
		if(ports[i] != portno){
			if(a==0)
				a=ports[i];
			else
				b=ports[i];
			
		}
	}
	//https://pubs.opengroup.org/onlinepubs/000095399/functions/setsockopt.html
	int v = 1;
	setsockopt ((sockfd1), SOL_SOCKET, SO_REUSEPORT, &v, sizeof(v));
	setsockopt ((sockfd1), SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
   
	bzero((char *)&addr1, sizeof(addr1));//https://man7.org/linux/man-pages/man3/bzero.3.html
   	addr1.sin_family = AF_INET;
   	addr1.sin_addr.s_addr = htonl(INADDR_ANY);
   	addr1.sin_port = htons(a);
   	addrlen1 = sizeof(addr1);
	cout<<"Client portnumber "<<a<<endl;

	bzero((char *)&addr2, sizeof(addr2));
   	addr2.sin_family = AF_INET;
   	addr2.sin_addr.s_addr = htonl(INADDR_ANY);
   	addr2.sin_port = htons(b);
   	addrlen2 = sizeof(addr2);
	cout<<"Server portnumber "<<b<<endl;
    
      	addr1.sin_addr.s_addr = inet_addr(IPADDR);
	int pos;
	string del = ",";
	while ((pos = vecclk.find(del)) != std::string::npos) {
    		clk[xi] = stoi(vecclk.substr(0, pos));
		xi++;
    		vecclk.erase(0, pos + del.length());
		
	}
   	int po=portno%10;
	string m1,m2,m3,m4;
	m1 = to_string(portno);

	int mc;
	cout<<"How many multicasts you need";
	cin>>mc;
      	while (mc) {
  		memset(message, 0, sizeof message);
		clk[po]++;
		m2=to_string(clk[0]);
		m3=to_string(clk[1]);
		m4=to_string(clk[2]);
		strcat(message, m1.c_str());//adding every clock value
		strcat(message, del.c_str());
		strcat(message, m2.c_str());
		strcat(message, del.c_str());
		strcat(message, m3.c_str());
		strcat(message, del.c_str());
		strcat(message, m4.c_str());
		strcat(message, del.c_str());
		printf("sending: %s\n", message);
		cout<<"Connected on portNumber : " <<portno<<endl;
		c = sendto(sockfd1, message, sizeof(message), 0,(struct sockaddr *) &addr1, addrlen1);
		if (c < 0) {
 	    		perror("Unable to send");
	    		exit(1);
		}
	sleep(5);
	mc--;
      	}
	pthread_exit(NULL); 
   
}

void* server(void* serv) { 

	struct sockaddr_in addr;
   	int sockfd, cnt,xi,xj;
	int nClock[3];
	int clk[3];
	string clck;
	string msg="";
	string rcv_port;
	socklen_t addrlen;
   	struct ip_mreq mreq;
   	char message[50];
	string del = ",";

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   	if (sockfd < 0) {
     		printf("Unable to create socket");
     		exit(1);
   	}

	int v = 1;
	setsockopt ((sockfd), SOL_SOCKET, SO_REUSEPORT, &v, sizeof(v));
	setsockopt ((sockfd), SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));

   	bzero((char *)&addr, sizeof(addr));
   	addr.sin_family = AF_INET;
   	addr.sin_addr.s_addr = htonl(INADDR_ANY);
   	addr.sin_port = htons(portno);
   	addrlen = sizeof(addr);
    	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {        
         	printf("Binding failed");
	 	exit(1);
      	}
      	mreq.imr_multiaddr.s_addr = inet_addr(IPADDR);         
      	mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
      	
	xi=0;
	int pos;
	xj=0;
	while ((pos = vecclk.find(del)) != std::string::npos) {
    		clk[xj] = stoi(vecclk.substr(0, pos));
		
		xj++;
    		vecclk.erase(0, pos + del.length());		
		}
	
	int por;
	int po; 
	string vec;     
      	while (1) {
 	 	cnt = recvfrom(sockfd, message, sizeof(message), 0, (struct sockaddr *) &addr, &addrlen);
		msg=(message);
		rcv_port=msg.substr(0, msg.find(del));
		msg.erase(0, (msg.find(del)) + del.length());
		vec=msg;
		while ((pos = msg.find(del)) != std::string::npos) {
    		nClock[xi] = stoi(msg.substr(0, pos));
		
		xi++;
    		msg.erase(0, pos + del.length());
		}
		xi=0;
		
		por=stoi(rcv_port);
		po=por%10;
		
		clck="["+to_string(clk[0])+","+to_string(clk[1])+","+to_string(clk[2])+"]";
		cout<<"Local Clock: "<<clck<<endl;
		if(nClock[po]-clk[po]==1){
			cout<<"Message Delivered"<<endl;
			clk[po]=nClock[po];
		}
		else
			cout<<"Message Buffered"<<endl;
	 	if (cnt < 0) {
	    		perror("recvfrom");
	    		exit(1);
	 	} else if (cnt == 0) {
 	    		break;
	 	}
	printf("%s: message = \"%s\"\n", inet_ntoa(addr.sin_addr), message);
	cout<<"Message received is :"<<message<<endl;
	cout<<"Connected to portNumber : " <<portno<<endl;
      	}
   
	pthread_exit(NULL);
    //return 0; 
}  

int main(int argc, char* argv[]){
	
	int x = 3;
	int y = atoi(argv[1]);
	string p = "800"+to_string(y);
	portno = std::stoi(p);
	pthread_t s1,c1;
	
	if (atoi(argv[2]) == 1){
		pthread_create(&s1,0, server, NULL);
		pthread_join(s1,NULL);
	}else if (atoi(argv[2]) == 2){
		pthread_create(&c1,0, client, NULL);
		pthread_join(c1,NULL);
	}else{
		cout<<"Enter process id in argv[0] and in argv[1]: 1->server or 2->client."<<endl;
	}
	return 0;

}
