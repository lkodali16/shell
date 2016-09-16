#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"


//populating the command data structure with subcommands and also argv fields
void ReadCommand(char *line, struct Command *command)
{
  char *temp;
  temp= strtok(line,"|");
  int i;
  for(i=0;(i<MAX_SUB_COMMANDS)&(temp!=NULL);i++)
    {
        command->sub_commands[i].line=temp;
		temp=strtok(NULL,"|");
    }
    command->num_sub_commands=i;
    int k=0;
    for(k=0;k<i;k++)
    	{
		ReadArgs(command->sub_commands[k].line,command->sub_commands[k].argv,MAX_ARGS);
	}
	
}

//To populate the redirect and background fields
void ReadRedirectsAndBackground(struct Command *command)
{
	command->background=0;
	command->stdin_redirect=NULL;
	command->stdout_redirect=NULL;
	char *temp,*file_name;
	temp=strdup(command->sub_commands[(command->num_sub_commands)-1].line);
	int length=strlen(temp),i=0,j,k;
		
	file_name=malloc(length);	
	file_name[0]='\0';			//initialising an empty array which can grow upto size:length
	for(i=length-1;i>=0;i--)
	{
		if(temp[i]=='&')
		{
			command->background=1;
			temp[i]='\0';
		}
		if(temp[i]=='<')
		{
			for(j=i+1,k=0;temp[j]!='\0';k++,j++)
			{
				file_name[k]=temp[j];
				
			}
			file_name[k]='\0';
			if((file_name[0]==' ')||(file_name[strlen(file_name)-1]==' '))
			file_name=strtok(file_name," ");		// to remove spave from starting of the file name
						
			command->stdin_redirect=strdup(file_name);
			temp[i]='\0';
		}
		
		else if(temp[i]=='>')
		{
			for(j=i+1,k=0;temp[j]!='\0';k++,j++)
			{
				file_name[k]=temp[j];
				
			}
			file_name[k]='\0';
			
			file_name[k]='\0';
			
			if((file_name[0]==' ')||(file_name[strlen(file_name)-1]==' ')){			// to remove space from starting of the file name
				
				file_name=strtok(file_name," ");	
			}
			
			
			command->stdout_redirect=strdup(file_name);
			temp[i]='\0';
		}
		
	}
	command->sub_commands[(command->num_sub_commands)-1].line = temp;
	ReadArgs(command->sub_commands[(command->num_sub_commands)-1].line, command->sub_commands[(command->num_sub_commands)-1].argv, MAX_ARGS);
}

//To print the command given
void PrintCommand(struct Command *command)
{
	int i;
	for(i=0;i<command->num_sub_commands;i++)
	{
		printf(" sub_command[%d] = %s\n",i,command->sub_commands[i].line);
		print_args(command->sub_commands[i].argv);
	}
	printf("stdin_redirect :%s\n",command->stdin_redirect);
	printf("stdout_redirect :%s\n",command->stdout_redirect);
	if(command->background==1)
	{
	printf("background :yes\n");
	}
	else
	{
		printf("background :no\n");
	}
	
}


//processing the subcommands
void ReadArgs(char *in, char **argv, int size)
{
  char *temp,*temp_in;
  temp_in=strdup(in);
  temp= strtok(temp_in," ");
  int j;
  for(j=0;(j<size-1)&(temp!=NULL);j++)
    {
      argv[j]= strdup(temp);
      temp=strtok(NULL," ");
    }
  argv[j]=NULL;
}

//to print the all arguments of each subcommand
void print_args(char** argv)
{
  int i;
  for(i=0;argv[i]!=NULL;i++)
    {  
      printf("   argv[%d] = '%s'\n",i, *(argv+i));
    }
}
