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
  
#define IPADDR "127.0.0.1"

int portno;
string vecclk= "0,0,0,";
int ports[3]= {8000, 8001, 8002};  
 
void* client(void* cli) { 
   	struct sockaddr_in addr1;
	int clock[3];
	int x=0;
   	int addrlen1, sockfd1, c;
   	struct ip_mreq msgreq1;
   	char msg[50];
	struct sockaddr_in addr2;
   	int addrlen2, sockfd2, c2;
   	struct ip_mreq msgreq2;
   	sockfd1 = socket(AF_INET, SOCK_DGRAM, 0);
	sockfd2 = socket(AF_INET, SOCK_DGRAM, 0);
   	if (sockfd1 < 0) {
     		printf("Unable to create socket");
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
	int u = 1;
	setsockopt ((sockfd1), SOL_SOCKET, SO_REUSEPORT, &u, sizeof(u));
	setsockopt ((sockfd1), SOL_SOCKET, SO_REUSEADDR, &u, sizeof(u));
   
	bzero((char *)&addr1, sizeof(addr1));
   	addr1.sin_family = AF_INET;
   	addr1.sin_addr.s_addr = htonl(INADDR_ANY);
   	addr1.sin_port = htons(a);
   	addrlen1 = sizeof(addr1);
	cout<<"Server portnumber : "<<a<<endl;

	bzero((char *)&addr2, sizeof(addr2));
   	addr2.sin_family = AF_INET;
   	addr2.sin_addr.s_addr = htonl(INADDR_ANY);
   	addr2.sin_port = htons(b);
   	addrlen2 = sizeof(addr2);
	cout<<"Client portnumber : "<<b<<endl;
     
      	addr1.sin_addr.s_addr = inet_addr(IPADDR);
	int pos;
	string del = ",";

	while ((pos = vecclk.find(del)) != std::string::npos) {
    		clock[x] = stoi(vecclk.substr(0, pos));
		x++;
    		vecclk.erase(0, pos + del.length());
		
	}

   	int po=portno%10;
	string m1,m2,m3,m4;
	m1 = to_string(portno);
	int mc;
	cout<<"How many multicasts you need?";
	cin>>mc;


      	//Updating clock value for each send event
	while(mc) {
  		memset(msg, 0, sizeof msg);
		clock[po]++;
		m2=to_string(clock[0]);
		m3=to_string(clock[1]);
		m4=to_string(clock[2]);
		strcat(msg, m1.c_str());//adding every clock value
		strcat(msg, del.c_str());
		strcat(msg, m2.c_str());
		strcat(msg, del.c_str());
		strcat(msg, m3.c_str());
		strcat(msg, del.c_str());
		strcat(msg, m4.c_str());
		strcat(msg, del.c_str());
		printf("sending: %s\n", msg);
		cout<<"Connected to  " <<portno<<endl;

		c = sendto(sockfd1, msg, sizeof(msg), 0,(struct sockaddr *) &addr1, addrlen1);
		
		if (c < 0) {
 	    		printf("Unable to send");
	    		exit(1);
		}
	sleep(5);
	mc--;
      	}
	pthread_exit(NULL); 
    //return 0; 
}

void* server(void* serv) { 

	struct sockaddr_in addr;
   	int sockfd, cnt;
	int xi,xj;
	string msg="";
	string clck;
	int nClk[3];
	int clk[3];
	string rcv_port;
	socklen_t addrlen;
   	struct ip_mreq mreq;
   	char message[50];
	string delimiter = ",";

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   	if (sockfd < 0) {
     		printf("unable to create socket");
     		exit(1);
   	}


	//https://pubs.opengroup.org/onlinepubs/000095399/functions/setsockopt.html
	int v = 1;
	setsockopt ((sockfd), SOL_SOCKET, SO_REUSEPORT, &v, sizeof(v));
	setsockopt ((sockfd), SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));

   	bzero((char *)&addr, sizeof(addr));//https://man7.org/linux/man-pages/man3/bzero.3.html
   	addr.sin_family = AF_INET;
   	addr.sin_addr.s_addr = htonl(INADDR_ANY);
   	addr.sin_port = htons(portno);
   	addrlen = sizeof(addr);
    	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {        
         	printf("Binding Failed");
	 	exit(1);
      	}
      	mreq.imr_multiaddr.s_addr = inet_addr(IPADDR);         
      	mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
      	

	xi=0;
	int pos;
	string vec;
	xj=0;

	while ((pos = vecclk.find(delimiter)) != std::string::npos) {
    		clk[xj] = stoi(vecclk.substr(0, pos));
		xj++;
    		vecclk.erase(0, pos + delimiter.length());		
		} 

	int por;
	int po;     
      	
	while(true) {
 	 	cnt = recvfrom(sockfd, message, sizeof(message), 0, (struct sockaddr *) &addr, &addrlen);
		msg=(message);
		rcv_port=msg.substr(0, msg.find(delimiter));
		msg.erase(0, (msg.find(delimiter)) + delimiter.length());
		vec=msg;
		while ((pos = msg.find(delimiter)) != std::string::npos) {
    		nClk[xi] = stoi(msg.substr(0, pos));
		xi++;
    		msg.erase(0, pos + delimiter.length());
		}
		xi=0;
		por=stoi(rcv_port);
		po=por%10;
		clk[po]=nClk[po];
		clck=("["+to_string(clk[0])+","+to_string(clk[1])+","+to_string(clk[2])+"]");
		cout<<"Local clock: "<<clck<<endl;
	 	if (cnt < 0) {
	    		perror("recvfrom");
	    		exit(1);
	 	} else if (cnt == 0) {
 	    		break;
	 	}
	printf("%s: message = \"%s\"\n", inet_ntoa(addr.sin_addr), message);
	cout<<"Message received is :"<<vecclk<<endl;
	cout<<"Connected to " <<portno<<endl;
      	}
     
	pthread_exit(NULL);
    //return 0; 
}  

int main(int argc, char* argv[]){
	
	int x = 3;
	int p = atoi(argv[1]);
	string pn = "800" +to_string(p);
	portno = std::stoi(pn);
	pthread_t s1,c1;
	if (atoi(argv[2]) == 1){
		pthread_create(&s1,0, server, NULL);
		pthread_join(s1,NULL);
	}else if (atoi(argv[2]) == 2){
		pthread_create(&c1,0, client, NULL);
		pthread_join(c1,NULL);
	}else{
		cout<<"Enter process id in argv[0] and in argv[1]: 1->server or 2->client"<<endl;
	}
	return 0;

}
