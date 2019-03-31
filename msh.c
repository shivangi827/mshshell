//Shivangi Vyas
//Uta Id: 101570499
//Assignment #1 : msh shell
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 355	// The maximum command-line size

#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports ten arguments

 char* getcommand(char *token[MAX_NUM_ARGUMENTS],char* commands[1000],int printcommandcounter); //gets command and tokenizes it
 void sigHandler(int signum);			//Catches and takes care of crtl+c and crtl+z signals
 
/////////////////////////////////////////////////////////////////   Get Command   /////////////////////////////////////////////////////////////////////////////////////
 char* getcommand(char *token[MAX_NUM_ARGUMENTS],char* commands[1000],int printcommandcounter) 
{
/*
*/
	char * cmd_str = (char*) malloc(MAX_COMMAND_SIZE);
    printf ("msh> ");		// Print out the msh prompt
    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
		if(cmd_str[0]=='!')
	{
		int commandinput;
		cmd_str=strtok(cmd_str,"!");
		{
			if (cmd_str[1]!='\n' )			//checks if the !n is a two digit number
		{
		 commandinput =10*(((int)cmd_str[0])-48) + ((int)cmd_str[1])-48; 	//Converts !n to int
		}
		else
		{
		 commandinput = ((int)cmd_str[0])-48 ;			////Converts !n to int
		}
		}
		if (printcommandcounter>=50)					//checks for istory commands
		{
			commandinput+=(printcommandcounter-50);		//prints out the last nth command
		}
		if (commandinput<16 && commandinput >0)			//checks if n is between 1-15
		{
			if(commandinput>=printcommandcounter )		//checks if n > commands entered so far
			{	
				int i;
				printf("Command not in history...\n\n");
				for (i=0;i<printcommandcounter;i++)
			  {
				  printf("%d: %s\n",i,commands[i]);
			  }
			  cmd_str="\n";
			}
			else
			{
			cmd_str=commands[commandinput];			
			}
		}
		else
		{
			printf("Number should be between 1-15\n");
			cmd_str="\n";
		}
		}
	
	 /* Parse input */
    int   token_count = 0;                                                                            
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                                                        
    char *working_str  = strdup( cmd_str );                
	// we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;
	// Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
		    
    }
	if (cmd_str[0]=='\n')
	{
		return cmd_str;						// if it just new line by itself then returns it
	}
	else
	{
	return strtok(cmd_str,"\n");			//removes newline from commands
	}
    free( working_root );
 }
 //////////////////////////////////////////////////////////////////   SigHAndler   //////////////////////////////////////////////////////////////////////////////////////////
 void sigHandler(int signum)			//Catches and takes care of crtl+c and crtl+z signals
 {
	 int status;
     switch(signum){
		case SIGCHLD:
		waitpid(-1, &status, WNOHANG);
		break;
		case SIGTSTP:
		waitpid(-1, &status, WNOHANG);
		break;
		case SIGINT:
		waitpid(-1, &status, WNOHANG);
		break;		 
     }
}
////////////////////////////////////////////////////////////////////////   MAIN   //////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	system("clear");
	char *token[MAX_NUM_ARGUMENTS];
	pid_t listpids[15];
	signal(SIGINT, SIG_DFL);
	int listpidcounter=0;
	int commandnum=0;
	int printcommandcounter=0;
	struct sigaction act;
	char* commands[1000];
	int a;
	pid_t child_pid;
  while( 1 )
  {
	  if(listpidcounter>15)				//rewrites all pids with newones
	  {
		  listpidcounter=0;  
	  }
	  commands[commandnum]= getcommand(token,commands,printcommandcounter);	//gets tokenized command
	if(strcmp(commands[commandnum],"\n")!=0)		//increments only if there is input
	 {
		 commandnum++;
		 printcommandcounter++;
	 }
	  if(*token==NULL)
	  {
		  continue;
	  }
	  if((strcmp(*token,"quit")==0) || (strcmp(*token,"exit")==0))
	  {
		 exit( EXIT_SUCCESS );
	  }
	  if (strcmp(token[0], "cd") == 0)
	  {
		  if (chdir(token[1])<0)			// location not found
		  {
			  perror(token[1]);
			  continue;
		  }
		  else
		  {
			  chdir(token[1]);
			  continue;
		  }
	  }
	  if((strcmp(*token,"listpids")==0))
	  {	
			int printcounter;
		  for(printcounter=0; printcounter<listpidcounter;printcounter++)
		  {
			  printf("%d: %d\n",printcounter,listpids[printcounter]);
		  }
		  continue;
	  }
	  if((strcmp(*token,"history")==0))
	  {
		  int i,j;
		  int a=0;
		  if (printcommandcounter>=50)		//prints the last 50 commands
		  {
			  for(j=printcommandcounter-50;j<commandnum;j++,a++)
			  {
				  printf("%d: %s\n",a,commands[j]);
			  }
		  }
		  else								//prints the first n commands
		  {
		  for (i=0;i<printcommandcounter;i++)
		  {
			  printf("%d: %s\n",i,commands[i]);
		  }
		  }
		  continue;
	  }
	  if((strcmp(*token,"bg")==0))
	  {
		  kill(child_pid,SIGCONT);		//restarts suspended process
		  continue;
	  }
	    child_pid =fork();
		listpids[listpidcounter]=child_pid;
		listpidcounter++;
		
	   if( child_pid == -1 )
  {
    // When fork() returns -1, an error happened.
    perror("fork failed: ");
    exit( EXIT_FAILURE );
  }
  else if ( child_pid == 0 )
  {
            signal(SIGINT, SIG_DFL);
            execvp(token[0], token);
            if (execvp( token[0],  token) < 0)
            {
                printf("%s: command not found\n\n",  token[0]);
                _exit(1);
            }
    	  exit( EXIT_SUCCESS );
	     
    fflush(NULL);
   }
  else 
  {
    // When fork() returns a positive number, we are in the parent
    // process and the return value is the PID of the newly created
    // child process.
	//Zero out the sigaction struct 
  memset (&act, '\0', sizeof(act));
	//Set the handler to use the function handle_signal()
   act.sa_handler = &sigHandler;
	//Install the handler and check the return value.
  if (sigaction(SIGTSTP, &act, NULL) < 0)
  {
    perror ("sigaction: ");
    return 1;
  }
  if (sigaction(SIGINT, &act, NULL) < 0)
  {
    perror ("sigaction: ");
    return 1;
  }
    int status;
    waitpid(child_pid, &status, 0 );
	//printf("ok");
    fflush( NULL );
  } 
  }
  	return 0;
}