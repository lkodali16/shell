#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "functions.h"
#include <fcntl.h>
#include <signal.h>

int w,flag;

//installing signal handler for SIGCHLD to cleanup zombie processes
void signal_handler(int signum)
{
	int status;
	w=waitpid(-1,&status,WUNTRACED);
	if(w==-1)
	flag=0;
	if(w>0)
	flag=1;
	//printf("signal %d received \t from %d\n",signum,w);
}

// If only one command is given as input
int one_sub_command(struct Command *command)
{
	if(strcmp(command->sub_commands[0].argv[0] , "cd")==0)	// to check if the command is cd
	{
		char file_dir[200];
			file_dir[0]='\0';
			int i=1;
					
			if(command->sub_commands[0].argv[1])			//to direct the path to HOME if input is cd or cd ~
			{if((strcmp(command->sub_commands[0].argv[1],"~")==0))
			command->sub_commands[0].argv[1]=strdup(getenv("HOME"));}
			else{command->sub_commands[0].argv[1]=strdup(getenv("HOME"));}
			while(command->sub_commands[0].argv[i]){
			
				strcat(file_dir,command->sub_commands[0].argv[i]);
				strcat(file_dir," ");
				i++;
			}
			file_dir[strlen(file_dir)-1]='\0';
		
				
		if((chdir(file_dir))<0)					//to change the file directory path to given input path
		{
			if(strlen(file_dir)==0)
			return;
			fprintf(stderr,"%s:No such file directory found\n",file_dir);
			fflush(stdout);
		}
	}	
	
	else{				//if the command is not cd, then execute the command by spawing a child process
	int ret = fork();
	
	if(ret==0)	//child process
	{
		if(command->stdin_redirect) 	//check input redirect
		{
			int in=open(command->stdin_redirect,O_RDONLY);
			if(in<0)
			{
				fprintf(stderr,"%s: File not found\n",command->stdin_redirect);
				exit(1);
			}
			close(0);
			dup(in);
			close(in);
			execvp(command->sub_commands[0].argv[0],command->sub_commands[0].argv);
			fprintf(stderr,"%s :command not found\n",command->sub_commands[0].argv[0]);
			exit(1);
		}
		else if(command->stdout_redirect)	//check output redirect
		{
			int out=open(command->stdout_redirect,O_WRONLY|O_CREAT,0666);
			if(out<0)
			{
				fprintf(stderr,"%s: File not found\n",command->stdout_redirect);
				exit(1);
			}
			close(1);
			dup(out);
			close(out);
			execvp(command->sub_commands[0].argv[0],command->sub_commands[0].argv);
			fprintf(stderr,"%s :command not found\n",command->sub_commands[0].argv[0]);
			exit(1);
		}
		else
		{
			execvp(command->sub_commands[0].argv[0],command->sub_commands[0].argv);
			fprintf(stderr,"%s :command not found\n",command->sub_commands[0].argv[0]);
			exit(1);
		}
		
	}
	
	else		//parent process
	{
		int *status,w;
		if(command->background ==1)		//To check for background symbol
		{
			printf("[%d]\n",ret);
                
		}
		else{
                w=waitpid(ret,status,WUNTRACED);
			}
	}
}
return 0;
}	//one_sub_command






//executing each command of *(myargs+i)
int exec_subcom(struct Command *command)
{
	int i,j,ret1=1;
	int num_sub_commands=command->num_sub_commands;
	int ret[num_sub_commands];
	int *fds[num_sub_commands-1];
	
	for(i=0;(i<num_sub_commands)&(ret1!=0);i++)  //creating N-1 pipes and N child processes
	{
		if(i!=(num_sub_commands-1))
		{
			fds[i]=malloc(2*sizeof(int));
			pipe(fds[i]);					//creating pipes (N-1) 
		}
		ret[i]=fork();						//creating a child process for each subcommand
		ret1=ret[i];
	}
	
	
	if(ret[i-1]==0)	//child process
	{
		int child_no=i;
		if(i==1)						//------------first child or first subcommand-------------------
		{	
			if(command->stdin_redirect)	//if stdin is present redirect the input of first subcommand from given "<input" file
			{
				int in=open(command->stdin_redirect,O_RDONLY);
				if(in<0)
				{
					fprintf(stderr,"%s: File not found\n",command->stdin_redirect);
					exit(1);
				}
				close(0);
				dup(in);
				close(in);
				close(3);
				close(1);
				dup2(4,1);
				execvp(command->sub_commands[0].argv[0],command->sub_commands[0].argv);
				fprintf(stderr,"%s :command not found\n",command->sub_commands[0].argv[0]);
				exit(1);
			}
			else	
			{
				close(3);
				close(1);
				dup(4);
			
				execvp(command->sub_commands[0].argv[0],command->sub_commands[0].argv);
				fprintf(stderr,"%s :command not found\n",command->sub_commands[0].argv[0]);
				exit(1);
			}		
						
		}
		else if(i==num_sub_commands)	//last child or last subcommand---------------------------------
		{
			close(2*i);
			//close(0);
			dup2(2*i-1, 0);
			for(j=3;j<(2*i-1);j++)
			{
				close(j);		//closing unwanted fds
			}
			
			
			if(command->stdout_redirect) // redirecting the last command output to given ">output" file
			{
				int out=open(command->stdout_redirect,O_WRONLY|O_CREAT,0666);
				if(out<0)
				{
					fprintf(stderr,"%s: File not found\n",command->stdout_redirect);
					exit(1);
				}
				close(1);
				dup(out);
				close(out);
				execvp(command->sub_commands[i-1].argv[0],command->sub_commands[i-1].argv);
				fprintf(stderr,"%s :command not found\n",command->sub_commands[i-1].argv[0]);
				exit(1);
			}
			else
			{
				execvp(command->sub_commands[i-1].argv[0],command->sub_commands[i-1].argv);
				fprintf(stderr,"%s :command not found\n",command->sub_commands[i-1].argv[0]);
				exit(1);
			}
			
		}
		else							//middle subcommands
		{
			close(0);
			close(1);
			dup(2*i-1);
			dup(2*i+2);
			for(j=3;j<(2*i+2);j++)
			{
				if(j!=(2*i-1)){ close(j); }
			}
			
			execvp(command->sub_commands[i-1].argv[0],command->sub_commands[i-1].argv);
			fprintf(stderr,"%s :command not found\n",command->sub_commands[i-1].argv[0]);
			exit(1);			
		}
	}						//parent process
	else
	{
		for(j=3;j<=(2*num_sub_commands);j++)	//closing unwanted file descriptors
		{
			close(j);
		}
		int *status,w;
		if(command->background ==1)		//To check the background symbol
		{
			printf("[%d]\n",ret[i-1]);
                
		}
		else{
				w=waitpid(ret1,status,WUNTRACED);
			
			}
	}
return 0;	
}
 //exec_subcom function
