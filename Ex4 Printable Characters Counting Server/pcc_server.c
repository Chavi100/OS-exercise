#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <signal.h>

int toExit=0;
int inaccept=0;
unsigned long statistics[95]={0};
void sighandler(int sig)
{ 
	toExit=1;
	if(inaccept && toExit)
  	{
  		for(int i=0;i<95;i++){
    		printf("char ’%c’ : %lu times\n",i+32 ,statistics[i]);
    	}
   		exit(0);
  	} 
}


int main(int argc, char *argv[])
{
  signal(SIGINT,sighandler);
  int nget = -1;
  int listenfd = -1;
  int connfd = -1;
  unsigned long printable=0;


  struct sockaddr_in serv_addr;
  socklen_t addrsize = sizeof(struct sockaddr_in );

  char char_sent;
  listenfd = socket( AF_INET, SOCK_STREAM, 0 );
  memset( &serv_addr, 0, addrsize );

  serv_addr.sin_family = AF_INET;
  // INADDR_ANY = any local machine address
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[1]));
  
  int flag = 1;  
   if (-1 == setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag))) {  
       perror("setsockopt fail");  
   }  

  if( 0 != bind( listenfd,
                 (struct sockaddr*) &serv_addr,
                 addrsize ) )
  {
    printf("\n Error : Bind Failed. %s \n", strerror(errno));
   
  }

  if( 0 != listen( listenfd, 10 ) )
  {
    printf("\n Error : Listen Failed. %s \n", strerror(errno));
   
  }

  while( 1 )
  {
  	if(!inaccept && toExit)
  	{
  		for(int i=0;i<95;i++){
    		printf("char ’%c’ : %lu times\n",i+32 ,statistics[i]);
    	}
   		exit(0);
  	}
    
    inaccept=1;
    connfd = accept( listenfd, NULL, &addrsize); 
	inaccept=0;
	if( connfd < 0 )
		{
		printf("\n Error : Accept Failed. %s \n", strerror(errno));
		
		}
	 
 unsigned long lenghthOfFile;
    int g=read(connfd, &lenghthOfFile, sizeof(unsigned long));
    if (g<=0){
    printf("\n error:reading failed.%s \n", strerror(errno));
    }                
    lenghthOfFile=ntohl(lenghthOfFile);
    unsigned long notread = lenghthOfFile;
    // keep looping until nothing left to write
    while( notread > 0 )
    {
      // notread = how much we have left to write
      // nget = how much we've written in last write() call */
      nget = read(connfd,&char_sent,sizeof(char));
      if (g<=0){
	  	printf("\n error:reading failed.%s \n", strerror(errno));	
      }
      notread -= nget;
     
      if((char_sent>=32) && (char_sent<=126))
      {
      printable++;
      statistics[((int)char_sent)-32]++;
      }
    }
    printable=htonl(printable);
   int f=write(connfd,&printable,sizeof(unsigned long));
   if (f<=0){
   	printf("\n error:write failed.%s \n", strerror(errno));
   }
	printable=0;
    // close socket
    close(connfd);

  }
}


