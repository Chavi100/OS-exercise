#include <sys/wait.h>  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>   
#include <sys/types.h>
#include <signal.h> 
//handels a termenaited child proccess to pervent zombie;
void sigchld_handler(int sigchld)
{
	int s;
	wait(&s);
}
//in prepare the shell applies to get relevant signals 
int prepare(void)
{  
	struct sigaction act;
	memset(& act,'\0',sizeof(act));
	act.sa_handler=SIG_IGN;
	sigaction(SIGINT,&act,NULL);
	
	return 0;
}
//here we handel prccesses sent to the shell
int process_arglist(int count, char **arglist)
{
	int status;
	//a background proccess
	if (!strcmp(arglist[count-1],"&"))
	{
		arglist[count-1]=NULL;
		pid_t pid=fork();
		if(pid ==-1)
		{
			fprintf(stderr,"fork failed\n");
			exit(1);
		}
		if (pid==0)
			{	//we get a singal to pervent zombie
				signal(SIGCHLD,sigchld_handler);
				execvp(arglist[0],arglist);
				fprintf(stderr,"command not found\n");
					exit(1);
			}
	}	
	else 
	{	//a proccess with pipe
		int ispipe=-1;
		for (int i=0;i<count;i++)
			{
				if (!strcmp(arglist[i],"|"))
				{
				ispipe=i;
				arglist[i]=NULL;
				break;
				}
			}
		if (ispipe!=-1)
		{
			int pipe_fd[2]; 
 			int pid1,pid2;
 			int p=pipe(pipe_fd);
 			if (p==-1)
 			{printf("eror pipeing");}
 			pid1=fork();
 			if(pid1 ==-1)
			{
				fprintf(stderr,"fork failed\n");
				exit(1);
			}
 			if(pid1==0)
 			{	signal(SIGINT,SIG_DFL);
 				close(1);
 				dup2(pipe_fd[1],1);
 				execvp(arglist[0],arglist);
 				fprintf(stderr,"command not found\n");
				exit(1);
			}
			
 			else
 			{
 				close (pipe_fd[1]);
 				pid2=fork();
 				if(pid2 ==-1)
				{
					fprintf(stderr,"fork failed\n");
					exit(1);
				}
 				if(pid2==0)
 				{	signal(SIGINT,SIG_DFL);
 					close(0);
 					dup2(pipe_fd[0],0);
 					execvp(arglist[ispipe+1],arglist);
 					fprintf(stderr,"command not found\n");
					exit(1);
 				}
 				wait(&status);
 				wait(&status);
 				
			}
 		}
 			 
	 			
		else
		 {	//a regular proccess
		 	pid_t pid=fork();
			if(pid ==-1)
			{
				fprintf(stderr,"fork failed\n");
				exit(1);
			}
			 if (pid==0)
				{	signal(SIGINT,SIG_DFL);
					execvp(arglist[0],arglist);
					fprintf(stderr,"command not found\n");
					exit(1);
				}
			wait(& status);
		}
	}
	return 1;
}

int finalize(void)
{
	return 0;
}

