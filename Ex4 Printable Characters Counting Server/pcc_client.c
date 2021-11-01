#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/stat.h>


int main(int argc, char *argv[])
{
  int totalsent = -1;
  int nsent = -1;
  int  sockfd  = -1;
  unsigned long printablechars=0;


  struct sockaddr_in serv_addr; // where we Want to get to

  
  if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Error : Could not create socket \n");
    exit(1);
  }

 

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(argv[2])); // Note: htons for endiannes
 serv_addr.sin_addr.s_addr = inet_addr(argv[1]); 

  printf("Client: connecting...\n");
  // connect socket to the target address
   if( connect(sockfd,(struct sockaddr*) &serv_addr,
									sizeof(serv_addr)) < 0)
		{
		printf("\n Error : Connect Failed. %s \n", strerror(errno));
		exit(1);
		} 
 
  struct stat sb;
  if (stat(argv[3], &sb) == -1) {
               perror("stat");
               exit(EXIT_FAILURE);
           }
    
    unsigned long n=htonl((long)sb.st_size);
    totalsent = 0;
    int notwritten=sizeof(int);
    while(totalsent<4){
    nsent = write(sockfd, &n ,sizeof(unsigned long));
    if(nsent<=0){
    	printf("\n error:write failed.%s \n", strerror(errno));
    	exit(1);             
    }
    totalsent  += nsent;
    notwritten -= nsent;
    }
    
    
    totalsent = 0;
    notwritten = sb.st_size;
    
   FILE * myfile=fopen(argv[3],"r");
   char toSend;
    // keep looping until nothing left to write
 while( notwritten > 0 )
    {
      // notwritten = how much we have left to write
      // nsent = how much we've written in last write() call */
     toSend= getc(myfile);
     nsent = write(sockfd,
                   &toSend,sizeof(char));
     if(nsent<=0){
    	printf("\n error:write failed.%s \n", strerror(errno));             
    	exit(1);
    }
      
      totalsent  += nsent;
      notwritten -= nsent;
    }

  
  unsigned long *p = & printablechars;
   int bytes_read = read(sockfd,
                      p,
                      sizeof( unsigned long));
    if( bytes_read <= 0 )
     {
     printf("\n error:reading failed.%s \n", strerror(errno));
     exit(1);
     }
   
    
  printablechars=ntohl(printablechars);
  printf("# of printable characters: %lu\n" ,printablechars);
  close(sockfd); 
  return 0;
}
