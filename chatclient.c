#include <stdio.h>		
#include <fcntl.h>		
#include <stdlib.h>
#include <sys/socket.h>		
#include <netinet/in.h>		
#include <sys/time.h>
#include <string.h>
#define MAXLINE 512		                                        
#define MAX_SOCK 12                                        

int main(int argc, char *argv[]){  
char *endmessage = "exit";                                 
char name[10];                                                 
  struct sockaddr_in server_addr;
  int maxfd;
  int n, pid;
  int s;  fd_set readfd;
  char line[MAXLINE], message[MAXLINE+1];                       
  if(argc != 4){                                
     printf(". %s chu\n", argv[0]);
    exit(0);}
  sprintf(name, "%s", argv[3]);                                  
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){                
     printf("no serch socket.\n");
    exit(0);}
  bzero((char *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]); 
  server_addr.sin_port = htons(atoi(argv[2]));                  
  if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
    printf("fail\n");                        
    exit(0);                                                    
  } else{
      send(s, name, strlen(name), 0);}                         
  maxfd = s + 1;
  FD_ZERO(&readfd);
  while(1){
    FD_SET(0, &readfd);
    FD_SET(s, &readfd);
    if(select(maxfd, &readfd, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0){
      printf("			warning.\n");
      exit(0);}                                                  
    if (FD_ISSET(0, &readfd)){
      if(fgets(message, MAXLINE, stdin)){                        
        sprintf(line, "%s %s", name, message);                    
        if (send(s, line, strlen(line), 0) < 0)                    
          printf("			error.\n");
        if (strstr(message, endmessage) != NULL ){                      
          printf("			bye.\n");
          close(s);
          exit(0);
        }
      }
    } 
    if (FD_ISSET(s, &readfd)) {                                   
      int size;
      if ((size = recv(s, message, MAXLINE, 0)) > 0){
        message[size] = '\0';
        printf("%s", message);                                    
        if(strstr(message, endmessage) != NULL ){                    
          close(s);                                                
          printf("\n");
          exit(0);}
        if(strstr(message, "rename.")!= NULL) {
          close(s);                                              
          printf("\n");
          exit(0);}
        if(strstr(message, "sorry.")!=NULL && strstr(message, name)!=NULL) {
          close(s);                                    
          printf("\n");
          exit(0);}
      }
    }
  }
}
