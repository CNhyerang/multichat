#include <stdio.h>		
#include <fcntl.h>		
#include <stdlib.h>
#include <signal.h>		
#include <sys/socket.h>		
#include <sys/file.h>
#include <netinet/in.h>		
#include <string.h>		
#include <arpa/inet.h>
#define MAXLINE  512							
#define MAX_SOCK 12								
 
int maxnumber(int);								 
int namecheck();								
int maxfd;									    
int clientnumber = 0;						    
int clientsocket[MAX_SOCK];					  
void clientclose(int);						
void serverclose(int);						
char clientmessage[MAXLINE];					 
char clientname[MAX_SOCK][10];					 
char *endmessage = "exit";					


int main(int argc, char *argv[]){
  char my_msg[MAXLINE], message[MAXLINE];     
  char *start = "	welcome hye rang talk.\n";									
  int i, j, n;
  int s, clientfd, clilen;  fd_set  readfd;
  struct sockaddr_in  client_addr, server_addr;  
  if(argc != 2){                                
    printf("	%s port num\n", argv[0]); 
    exit(0);} 
  if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){                                                
    printf("	You can not open a socket.");   
    exit(0);}

  bzero((char *)&server_addr, sizeof(server_addr));    
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(atoi(argv[1]));
  if (bind(s,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
    printf("You can not bind an IP address.\n");
    exit(0);}
  printf("opene a chat room. \n");                  
  listen(s, 5);
  maxfd = s + 1;

  while(1){
    sprintf(clientname[clientnumber], "%s", "[Anyone]");
    message[0] = '\0';
    clientmessage[0] = '\0';                             
    FD_ZERO(&readfd);
    FD_SET(s, &readfd);
    FD_SET(0, &readfd);
    for(i=0; i<clientnumber; i++)  FD_SET(clientsocket[i], &readfd);
    maxfd = maxnumber(s) + 1;       			           
    if (select(maxfd, &readfd, (fd_set *)0, (fd_set *)0,(struct timeval *)0) < 0){
      printf("	Room will be closed as excess personnel		. \n");
      exit(0);} 
    if(FD_ISSET(s, &readfd)){                              
      clilen = sizeof(client_addr);
      clientfd = accept(s, (struct sockaddr *)&client_addr, &clilen);
      if(clientfd == -1){
        printf("			Access error\n");
        exit(0);}
      clientsocket[clientnumber] = clientfd;                    
      n = recv(clientfd, clientmessage, MAXLINE, 0);            
      clientmessage[n] = '\0';
      sprintf(clientname[clientnumber], "%s", clientmessage);
      getsockname(clientfd, (struct sockaddr *)&client_addr, &clilen);	
      if(namecheck() == 1) {                               
        printf("		%d¹øÂ°·Î %s ´ÔÀÌ ÀÔÀåÇÏ¼Ì½À´Ï´Ù.\n", clientnumber+1, clientname[clientnumber]);
                                                          
        printf("	%s  %s ip rnum %d portnum.\n", clientname[clientnumber],
                                        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
					                                      
        sprintf(message, "		%s hi \n", clientname[clientnumber]);
        clientnumber++;
        for(j=0; j<clientnumber-1; j++)                        
          send(clientsocket[j], message, strlen(message), 0);  
        send(clientfd, start, strlen(start), 0);           
       } 
    }
    if(FD_ISSET(0, &readfd)){                              
      if(fgets(my_msg, MAXLINE, stdin)){
        if (strstr(my_msg, endmessage) != NULL)		       
          serverclose(s);					                
        sprintf(message, "¹æÀå : %s", my_msg);             
        if(strstr(message, "/°­Åð")!=NULL) {              
	  for(i=0; i<clientnumber; i++){
	    if(strstr(message, clientname[i])!=NULL){           	 
	      sprintf(message, "			%s ´ÔÀ» °­ÅðÇÏ°Ú½À´Ï´Ù. \n", clientname[i]);
	      for(j=0; j<clientnumber; j++) {                    	   
	        send(clientsocket[j], message, strlen(message), 0);}
	      clientclose(i);                                  
	    }
	  }
	} else if(strstr(message, "/sdsd") !=NULL) {            	     
	        for(i=0; i<clientnumber; i++)
		if(strstr(message, clientname[i])!=NULL)               
		  send(clientsocket[i], message, strlen(message), 0);  
	       } else {                                          
		   for(i=0; i<clientnumber; i++){
		     if(send(clientsocket[i], message, strlen(message), 0) < 0)
		       	printf("			fail.");}
                 }
      }
    }
    for(i = 0; i < clientnumber; i++){
      if(FD_ISSET(clientsocket[i], &readfd)){                    
          if((n = recv(clientsocket[i], clientmessage, MAXLINE, 0))  <= 0){
          clientclose(i);
          continue;}
        clientmessage[n] = '\0';
\     if(strstr(clientmessage, endmessage) != NULL) {             
          clientclose(i);                                         
          continue;}
        if(strstr(clientmessage, "/sdsd") !=NULL){                      
          for(j=0; j<clientnumber; j++)
            if(strstr(clientmessage, clientname[j])!=NULL && i!=j)  
              send(clientsocket[j], clientmessage, n, 0);          
        } else{                                                      
            for (j = 0; j < clientnumber; j++){
              if (i!=j)                                            
                send(clientsocket[j], clientmessage, n, 0);
            }
          }
        printf("%s", clientmessage);                               
      }
    }
  }
}
\
int namecheck(){                                                  

  char *msg = "	rename.\n";
  int i;
  for(i=0; i<clientnumber; i++){
    if(strstr(clientmessage, clientname[i])!=NULL){                
      send(clientsocket[clientnumber], msg, strlen(msg), 0);        
      close(clientsocket[clientnumber]);                            
      return(0);		
     }
   }
  return(1);
}
void clientclose(int i){                                          
  int j;
  char msg[MAXLINE];
  sprintf(msg, "		%s exit.\n", clientname[i]);
  close(clientsocket[i]);                                        
  if(i != clientnumber-1) {
    clientsocket[i] = clientsocket[clientnumber-1];
    sprintf(clientname[i], "%s", clientname[clientnumber-1]);
    clientname[clientnumber-1][0] = '\0'; }                        
  clientnumber--;                                                  
  printf("	%s user%d number\n", msg, clientnumber);     
  for(j=0; j<clientnumber; j++)
    send(clientsocket[j], msg, strlen(msg), 0);}                   
int maxnumber(int k){                                             
  int max = k;    
  int r;
  for (r=0; r < clientnumber; r++){
    if (clientsocket[r] > max ) max = clientsocket[r];}
  return max;}
void serverclose(int i){
  char *msg = "	bye user	.";
  int j;
  for(j=0; j<clientnumber; j++)  {
    send(clientsocket[j], msg, strlen(msg), 0);                     
    close(clientsocket[j]);}                                     
  close(i);                                                      
  printf("	bye	.\n");
  exit(0);
}
