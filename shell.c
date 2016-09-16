#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"
#include "functions.c"
#include "exec.c"

int main()
{      
	signal(SIGCHLD,signal_handler); 
	while(1)
	{
		char s[200], buf[200];
		getcwd(buf,200);		//to get the current working directory
		struct Command *com;
		com=malloc(sizeof(struct Command));
		if(flag==1)
		{
			printf("[%d]  finished\n",w);
			flag=0;
		}
		printf("%s$ ",buf);
		fgets(s,sizeof s,stdin);
		s[strlen(s)-1]='\0';
		if(s[0]!='\0')
		{
			ReadCommand(s,com);
			ReadRedirectsAndBackground(com);
			//PrintCommand(com);
			if(com->num_sub_commands == 1)
			{
				one_sub_command(com); 		// to execute one subcommand
			}
			else{
				exec_subcom(com);
			}
			
			free(com);
		}
	}
  return 0;
}

