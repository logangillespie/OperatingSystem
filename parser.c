#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
typedef struct {
	int size;
	char **items;
} tokenlist;

char *get_input(void);
tokenlist *get_tokens(char *input);
tokenlist *get_paths(char *input);

tokenlist *new_tokenlist(void);
void add_token(tokenlist *tokens, char *item);
void free_tokens(tokenlist *tokens);

bool tokenCheck(char *token);
bool dollarSignCheck(char *token);
char *searchDollar(char *token);
char* returnenv(char *token);
void prompt ();

bool hasTilde(char *token);
void tildeExpansion(char *token);
void externalCommmand(tokenlist * tokenpath, tokenlist * tokens);
void onePipe(char* firstCommand[2], char* secondCommand[2]);
void twoPipes(char* firstCommand[2], char* secondCommand[2], char* thirdCommand[2]);
char* commandPath(char* command);
void exit_command();

int main()
{
	time_t begin = time(NULL);
	time_t biggesttime = time(NULL);
	time_t begincycle = 0;
	time_t currenttime = time(NULL);
	while (1)
	{
		begincycle = time(NULL);
		prompt();
		/* input contains the whole command
		 * tokens contains substrings from input split by spaces
		 */
		tokenlist *tokenpath = get_paths(getenv("PATH"));
		char *input = get_input();
	
		tokenlist *tokens = get_tokens(input);
		externalCommmand(tokenpath, tokens);
		for (int i = 0; i < tokens->size; i++) 
		{
			char *newToken = tokens->items[i]; // grabs individual words from sentence
			if(hasTilde(newToken) == true)
			{
				tildeExpansion(newToken);
			}
		
		}
		if(strcmp(tokens->items[0],"exit") == 0)
		{
			time_t end = time(NULL);
			printf("Shell ran for %ld seconds", (end - begin));
            printf(" and took ");
            printf("%ld", biggesttime); 
            printf(" seconds to execute one command.");
            printf("\n"); 
            exit(0);
		}
		else if(strcmp(tokens->items[0], "cd") == 0)
		{
			char cwd[256];
			if(tokens->size == 2)
			{
				const char* home = getenv("HOME");
				chdir(home);
    			if (getcwd(cwd, sizeof(cwd)) == NULL)
      				perror("getcwd() error");
    			else
  					setenv("PWD", cwd, 1);
		}
		else if(tokens->size == 3)
		{
			const char* directory = tokens->items[1];
				chdir(directory);
    		if (getcwd(cwd, sizeof(cwd)) == NULL)
      			perror("getcwd() error");
    		else
				setenv("PWD", cwd, 1);

		}
		else if(tokens->size == 4)
			printf("%s\n", "Too many arguments");
	}
	time_t endcycle = time(NULL);
	biggesttime = endcycle - begincycle;
	if(biggesttime > currenttime)
	{
		biggesttime = currenttime;
	}
		free(input);
		free_tokens(tokens);
	}
	return 0;
}
tokenlist *new_tokenlist(void)
{
	tokenlist *tokens = (tokenlist *) malloc(sizeof(tokenlist));
	tokens->size = 0;
	tokens->items = (char **) malloc(sizeof(char *));
	tokens->items[0] = NULL; /* make NULL terminated */
	return tokens;
}
void add_token(tokenlist *tokens, char *item)
{
	int i = tokens->size;

	tokens->items = (char **) realloc(tokens->items, (i + 2) * sizeof(char *));
	tokens->items[i] = (char *) malloc(strlen(item) + 1);
	tokens->items[i + 1] = NULL;
	strcpy(tokens->items[i], item);

	tokens->size += 1;
}
char *get_input(void)
{
	char *buffer = NULL;
	int bufsize = 0;
	char line[5];
	while (fgets(line, 5, stdin) != NULL) 
	{
		int addby = 0;
		char *newln = strchr(line, '\n');
		if (newln != NULL)
			addby = newln - line;
		else
			addby = 5 - 1;

		buffer = (char *) realloc(buffer, bufsize + addby);
		memcpy(&buffer[bufsize], line, addby);
		bufsize += addby;

		if (newln != NULL)
			break;
	}

	buffer = (char *) realloc(buffer, bufsize + 1);
	buffer[bufsize] = 0;
	return buffer;
}
tokenlist *get_tokens(char *input)
{
	char *buf = (char *) malloc(strlen(input) + 1);
	strcpy(buf, input);

	tokenlist *tokens = new_tokenlist();

	char *tok = strtok(buf, " ");
	while (tok != NULL) {
		tok = returnenv(tok);
		add_token(tokens, tok); //tok is individual word
		tok = strtok(NULL, " ");
	}
	add_token(tokens, "\0");
	free(buf);
	return tokens;
}

tokenlist *get_paths(char *input)
{
	char *buf = (char *) malloc(strlen(input) + 1);
	strcpy(buf, input);
	tokenlist *tokens = new_tokenlist();
	char *tok = strtok(buf, ":");
	while (tok != NULL) 
	{
		add_token(tokens, tok); //tok is individual word
		tok = strtok(NULL, ":");
	}
	free(buf);
	return tokens;
}

void free_tokens(tokenlist *tokens)
{
	for (int i = 0; i < tokens->size; i++)
	{
		free(tokens->items[i]);
	}
	free(tokens);
}
bool tokenCheck(char *token){
	char *check = getenv(token);
	if(check != NULL)
		return true;
	return false;
}
bool dollarSignCheck(char *token){
		if(token[0] == '$')
			return true;
		return false;
}
char *searchDollar(char* token)
{
	size_t len = strlen(token);
	char* substr;
	for(int i = 0; i<len; i++)
	{
		if(token[i] == '$')
		{
			substr = token + 1;
		}
	}
	return substr;
}
char * returnenv(char * token)
{
	char* hasDollar = token;
	if(dollarSignCheck(token) == true)
	{
		hasDollar=searchDollar(token);
		if(tokenCheck(hasDollar) == true)
		{
			hasDollar = getenv(hasDollar);
		}
	}
	return hasDollar;
}

void prompt() 
{   
     printf(getenv("USER"));
     printf("@");
     printf(getenv("MACHINE"));
     printf(":");
     printf(getenv("PWD"));
     printf(">\n");
}
bool hasTilde(char *token)
{ /*first checks to see if there is a tilde */
	if(token[0] == '~')
		return true;
	return false;
}

void tildeExpansion(char *token)
{
	printf(getenv("HOME"));
		char *ntoken;
		if(token[0] == '~')
		{
			ntoken = token+1;
		}
	printf(ntoken);
	printf("\n");

}
void externalCommmand(tokenlist * tokenpath, tokenlist * tokens)
{
	for(int i = 0; i < 10; i++)
	{
		int length = strlen(tokens->items[0]);
		strncat(tokenpath->items[i], "/", 1);
		strncat(tokenpath->items[i], tokens->items[0], length);
			
	}
		int size = tokens->size;
		char *input, *output;
		bool arrow = false;
		bool stOut = false;
		bool stIn = false;
		bool background = false;
		int numPipe = 0;

		for(int i = 0; i<size; i++)
		{
			if(*tokens->items[i] == '<' || *tokens->items[i] == '>')
			{ //checks if there
				
				arrow = true;	//arrow in command
					if(*tokens->items[i] == '>')
					{ // echo hello > input.txt //puts hello in input.txt
						stOut = true;
						output = tokens-> items[size-2];
					}
					if(*tokens->items[i] == '<')
					{
						stIn = true;
						input = tokens->items[size-2];
					}
			}
			if(*tokens->items[i] == '|')
			{
				numPipe++;
			}
			if(*tokens->items[size-2] == '&')
			{ //if last character is &
				
				background = true;
			}
		}
		int inSpot, outSpot;
		if(stIn == true && stOut == true)
		{ // this is for cmd with both < and >
			for(int i = 0; i<size; i++)
			{
				if(*tokens->items[i] == '<')
				{
					input = tokens->items[i+1];
					inSpot = i+1;
				}

				if(*tokens->items[i] == '>')
				{
					output = tokens->items[i+1];
					outSpot = i+1;
				}
			}
		}
		int fd;
		char * x[size];
		for(int i = 0; i < 10; i++)
		{
			fd = access(tokenpath->items[i], F_OK);
			if(fd == -1)
				;
			else if(fd == 0)
			{
				x[0] = tokenpath->items[i];
				break;
			}
			
		}
		if(fd == -1)
			printf("%s\n", "command not found");
		else
		{
			if(size >= 2)
			{
				for(int i = 1; i < size; i++)
				{
					x[i] = tokens->items[i];
			
				}
			}
		}
x[size-1] = NULL;
//file redirection stuff
char *x2[size];
int fd1, fd2;
x2[0] = x[0];
x2[1] = '\0';

		if(stOut == true && stIn == true)
		{
				if(inSpot > outSpot)
				{  //cmd < int > out
					fd2 = open(input, O_RDONLY, 0777);
					fd1 = open(output,O_RDWR| O_CREAT, 0777);
					if(fd2==-1)
					{
						printf("%s\n","Can't open input" );
					}
				}
				else
				{
					fd1 = open(output,O_RDWR| O_CREAT, 0777);
					fd2 = open(input, O_RDONLY, 0777);
					if(fd2==-1)
					{
						printf("%s\n","Can't open input" );
					}
				}
		}
		if(stOut == true && stIn == false)
		{
			fd1 = open(output,O_RDWR| O_CREAT, 0777);
		}
		if(stIn == true && stOut == false)
		{
			fd2 = open(input, O_RDONLY, 0777);
			if(fd2==-1)
					{
						printf("%s\n","Can't open input" );
					}
		}
//Piping stuff starts here//

		char *x3[2];
		if(numPipe > 0)
		{
			if(numPipe == 1)
			{
				x2[0] = commandPath(x[2]);
				x2[1] = '\0';
				x[1] = '\0';
				onePipe(x,x2);
			}
			else if(numPipe == 2)
			{ 
				x2[0] = commandPath(x[2]);
				x2[1] = '\0';
				x3[0] = commandPath(x[4]);
				x3[1] = '\0';
				x[1] = '\0';
				twoPipes(x,x2,x3);
			}
			else
			{
				printf("%s\n", "Error: Too many pipes");
			}
		}
//back to redirection
		else if(arrow == true)
		{
			int pid = fork();
			if(pid == 0)
			{
				if(stIn == true && stOut == true)
				{
					close(0);
					dup(fd2);
					close(fd2);
					close(1); //0 is standard in, 1 is standard out
					dup(fd1);
					close(fd1);
					if(execv(x2[0], x2) == -1)
					{
					 	printf("%s\n", "Command not found");
				 	}
				}
				else if(stOut == true)
				{
					close(1); //0 is standard in, 1 is standard out
					dup(fd1);
					close(fd1);
					if(execv(x2[0], x2) == -1)
					{
	 					printf("%s\n", "Command not found");
					}

				}
				else if(stIn == true)
				{
					close(0);
					dup(fd2);
					close(fd2);
					if(execv(x2[0], x2) == -1)
					{
	 					printf("%s\n", "Command not found");
					}
				}
				 else
				 {
					 if(execv(x[0], x) == -1)
					 {
					 	printf("%s\n", "Command not found");
				 		}
				}
			}
			else //parent function
			{
				//store name of string from background process and create new string bc tokenlist will be freed up at some points
				waitpid(pid, NULL, 0);
			}
		}
//background processing stuff here
		else if(background == true)
		{
			int pid = fork();
            x[1] = '\0';
            if(pid == 0)
            {
                if(execv(x[0], x) == -1)
                {
                    printf("%s\n", "command not found");
                }
            }
        	else
        	{
            	int i = 0;
            	int status;
            	waitpid(pid, &status, WNOHANG);
            	i++;

           		if(status != 0)
           		{
                	printf("%s\n", "Status terminated");
            	}
             	printf("[%d", i);
                printf("]: ");
                printf("%d\n", pid);
        	}
			
		}
		else
		{
			int pid = fork();
			if(pid == 0){
			if(execv(x[0], x) == -1){
			printf("%s\n", "Command not found");
		}
	}
	else
	{
		waitpid(pid, NULL, 0);
	}
}
		}
void onePipe(char* firstCommand[2], char* secondCommand[2])
{
	int p_fds[2];
	pipe(p_fds);
	int pid = fork();
	if(pid == 0)
	{
		dup2(p_fds[1], 1); //piping
		close(p_fds[0]);
		if(execv(firstCommand[0], firstCommand) == -1)
		{
			printf("%s\n", " command not found" );
		}
	}
	else
	{
		pid = fork();
		if(pid == 0)
		{
			dup2(p_fds[0], 0); //piping
			close(p_fds[1]);
			if(execv(secondCommand[0], secondCommand) == -1)
			{
				printf("%s\n", " command not found" );
			}
		}
		else
		{
			close(p_fds[0]);
			close(p_fds[1]);
			waitpid(pid, NULL, 0);
		}
	}

}
void twoPipes(char* firstCommand[2], char* secondCommand[2], char* thirdCommand[2]){
	int p_fds[4];
	int status;
	pipe(p_fds); // for the first pipe
	pipe(p_fds+2); //for the second pipe

	int pid = fork();
	if(pid == 0)
	{
		dup2(p_fds[1],1); //takes first command stdout and replaces w/ 2nd command write
		for(int i = 0; i<4;i++)
		{
			close(p_fds[i]); //close pipes
		}
		if(execv(firstCommand[0], firstCommand) == -1)
		{
			printf("%s\n", "Command not found");
		}
	}
	else 
	{
		pid = fork();
		if(pid == 0)
		{
			dup2(p_fds[0], 0); //second command stdin ->read of first
			dup2(p_fds[3],1); //second command stdout -> write of third
			for(int i = 0; i<4;i++)
			{
				close(p_fds[i]); //close pipes
			}
			if(execv(secondCommand[0], secondCommand) == -1)
			{
				printf("%s\n", "command not found");
			}
		}
		else
		{
			pid = fork();
			if(pid == 0)
			{
				for(int i = 0; i<4;i++)
				{
					close(p_fds[i]); //close pipes
				}
				if(execv(thirdCommand[0], thirdCommand) == -1)
				{
					printf("%s\n", "command not found");
				}
			}
		}
	}
	for(int i = 0; i<4;i++)
	{
		close(p_fds[i]); //close pipes
	}
	for(int i = 0; i<3; i++)
		wait(&status);
}

char* commandPath(char* command)
{
	char* temp;
	temp = malloc(sizeof(char*)*5);
	strcpy(temp, "/usr/bin/");
	strcat(temp, command);	return temp;
}



