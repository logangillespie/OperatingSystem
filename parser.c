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

	while (1) {
		prompt();

		/* input contains the whole command
		 * tokens contains substrings from input split by spaces
		 */
		tokenlist *tokenpath = get_paths(getenv("PATH"));
		// for(int i = 0; i < tokenpath->size; i++){
		// 	printf("tokenpath %d: (%s)\n", i, tokenpath->items[i]);
		//}
		char *input = get_input();
		printf("whole input: %s\n", input);
		tokenlist *tokens = get_tokens(input);
		externalCommmand(tokenpath, tokens);
		//char *newToken, dollarCheck, command;
		//makeArray(tokens);
		for (int i = 0; i < tokens->size; i++) {
			//printf("token %d: (%s)\n", i, tokens->items[i]);
			printf("token %d: (%s)\n", i, tokens->items[i]);
			char *newToken = tokens->items[i]; // grabs individual words from sentence
			char* example = returnenv(newToken);
			if(hasTilde(newToken) == true){
				tildeExpansion(newToken);
			}


			printf("%s\n", example );



		}
		if (strcmp(tokens->items[0],"exit") == 0)
					{
							exit_command();
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
	while (fgets(line, 5, stdin) != NULL) {
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
		add_token(tokens, tok); //tok is individual word
		tok = strtok(NULL, " ");
	}
	add_token(tokens, "\0");
	//printf("size is: (%d)", tokens->size);
	free(buf);
	return tokens;
}

tokenlist *get_paths(char *input)
{
	char *buf = (char *) malloc(strlen(input) + 1);
	strcpy(buf, input);

	tokenlist *tokens = new_tokenlist();

	char *tok = strtok(buf, ":");
	while (tok != NULL) {
		add_token(tokens, tok); //tok is individual word
		tok = strtok(NULL, ":");
	}

	free(buf);
	return tokens;
}

void free_tokens(tokenlist *tokens)
{
	for (int i = 0; i < tokens->size; i++)
		free(tokens->items[i]);

	free(tokens);
}
bool tokenCheck(char *token){
	char *check = getenv(token);
//	printf("%sfunction tokencheck\n", check);
	if(check != NULL)
		return true;
	return false;
}
bool dollarSignCheck(char *token){
		if(token[0] == '$')
			return true;
		return false;
}
char *searchDollar(char* token){
	size_t len = strlen(token);
	char* substr;
	for(int i = 0; i<len; i++){
		if(token[i] == '$'){
		//	printf("%sfunction SD \n", token);
		//	printf("   ");
			substr = token + 1;
		//	printf("%sfunction SD \n", substr);
		}
	}
	return substr;
}

char * returnenv(char * token){
	char* hasDollar = token;
	if(dollarSignCheck(token) == true){
				hasDollar=searchDollar(token);
				if(tokenCheck(hasDollar) == true){
					hasDollar = getenv(hasDollar);
					//printf("%s\n", hasDollar );
					return hasDollar;
				}
			}
			return hasDollar;
}

void prompt() // for part 3 of the project
{
    //my plan as of now is to wait til we have the env variables from part 1 set up and then just call the variable names instead of the hardcoded stuff i have now
    //also this might not end up being a void function

     printf(getenv("USER"));
     printf("@");
     printf(getenv("MACHINE"));
     printf(":");
     printf(getenv("PWD"));
     printf(">\n");


}
bool hasTilde(char *token){ /*first checks to see if there is a tilde */
	if(token[0] == '~')
		return true;
	return false;
}

void tildeExpansion(char *token){
	printf(getenv("HOME"));
		char *ntoken;
		if(token[0] == '~'){
			ntoken = token+1;
		}

	printf(ntoken);
	printf("\n");

}
void externalCommmand(tokenlist * tokenpath, tokenlist * tokens)
{
	for(int i = 0; i < 10; i++){
			int length = strlen(tokens->items[0]);
			strncat(tokenpath->items[i], "/", 1);
			strncat(tokenpath->items[i], tokens->items[0], length);
			//printf("%s\n", tokenpath->items[i]);
		}
		int size = tokens->size;
		printf("the size is: ");
		printf("%d\n", tokens->size );


		char *input, *output;
		bool arrow = false;
		bool stOut = false;
		bool stIn = false;
		int numPipe = 0;


		for(int i = 0; i<size; i++){
			if(*tokens->items[i] == '<' || *tokens->items[i] == '>'){ //checks if there
				printf("%s\n", "found <, >" );
				arrow = true;												//arrow in command
					if(*tokens->items[i] == '>'){ // echo hello > input.txt //puts hello in input.txt
						stOut = true;
						output = tokens-> items[size-2];
						//printf("output file: %s\n", output);
					}
					if(*tokens->items[i] == '<'){
						stIn = true;
						input = tokens->items[size-2];
					}
				}
				if(*tokens->items[i] == '|'){
					numPipe++;
				}
		}

		int inSpot, outSpot;

		if(stIn == true && stOut == true){ // this is for cmd with both < and >
			for(int i = 0; i<size; i++){
				if(*tokens->items[i] == '<'){
					input = tokens->items[i+1];
					inSpot = i+1;
				}

				if(*tokens->items[i] == '>'){
					output = tokens->items[i+1];
					outSpot = i+1;
				}
		}
	}

		int fd;
		char * x[size];
		for(int i = 0; i < 10; i++){
		fd = access(tokenpath->items[i], F_OK);
		if(fd == -1)
			;
		else if(fd == 0){
			//printf("fd: %d\n", fd);
			x[0] = tokenpath->items[i];
			break;
		}
			//printf("%s\n", "found");
	}

		if(fd == -1)
			printf("%s\n", "command not found");
		else{
			if(size >= 2){
		for(int i = 1; i < size; i++){
			x[i] = tokens->items[i];
			//printf("tokens->items: %s\n", x[i]);
		}
	}
}

		printf("the size is: ");
		printf("%d\n", size );
		 x[size-1] = NULL;
		// for(int i = 0; i < size; i++)
		// 	printf("hehe: %s\n", x[i]);

//file redirection stuff
char *x2[size];
int fd1, fd2;
x2[0] = x[0];
x2[1] = '\0';

			if(stOut == true && stIn == true){
				printf("%s\n", "made it here both");
				if(inSpot > outSpot){  //cmd < int > out
					fd2 = open(input, O_RDONLY, 0777);
					fd1 = open(output,O_RDWR| O_CREAT, 0777);
				}
				else{
					fd1 = open(output,O_RDWR| O_CREAT, 0777);
					fd2 = open(input, O_RDONLY, 0777);
				}
			}
		if(stOut == true && stIn == false){
			fd1 = open(output,O_RDWR| O_CREAT, 0777);
				printf("%s\n", "made it here std out only" );
		}

		if(stIn == true && stOut == false){

			fd2 = open(input, O_RDONLY, 0777);
			printf("%s\n", "made it here std in only" );
		}
//Piping stuff starts here//

char *x3[2];
if(numPipe > 0){
		if(numPipe == 1){
			x2[0] = commandPath(x[2]);
			x2[1] = '\0';
			x[1] = '\0';
		//	printf("I have 1 pipe\n");
			onePipe(x,x2);
		}
		else if(numPipe == 2){ // numPipe = 2
		//	printf("I have 2 pipes\n");
		x2[0] = commandPath(x[2]);
		x2[1] = '\0';
		x3[0] = commandPath(x[4]);
		x3[1] = '\0';
		x[1] = '\0';
			twoPipes(x,x2,x3);
		}
		else{
			printf("%s\n", "Error: Too many pipes");
		}
	}
//back to redirection
	//	int	fd1 = open(output,O_RDWR| O_CREAT, 0777);
	if(arrow == true){
		printf("I have redirection\n");
			int pid = fork();
			if(pid == 0){
				printf("I am a child\n");

				if(stIn == true && stOut == true){// && larger == true){
					close(0);
					dup(fd2);
					close(fd2);

					close(1); //0 is standard in, 1 is standard out
					dup(fd1);
					close(fd1);

					if(execv(x2[0], x2) == -1){
					 printf("%s\n", "command not found");
				 }
				}
				else if(stOut == true){
					close(1); //0 is standard in, 1 is standard out
					dup(fd1);
					close(fd1);
					 if(execv(x2[0], x2) == -1){
	 					printf("%s\n", "command not found");
					}

			}
		else if(stIn == true){
					 close(0);
					 dup(fd2);
					 close(fd2);

					 if(execv(x2[0], x2) == -1){
	 					printf("%s\n", "command not found");
					}
				}

				 else{
					 if(execv(x[0], x) == -1){
					 printf("%s\n", "command not found");
				 }
			}
				 //taken from recitation, needs correction
				printf("it didnt work\n");
			}
		else //parent function
			{
//store name of string from background process and create new string bc tokenlist will be freed up at some points
				printf("I am a parent\n");
				waitpid(pid, NULL, 0);
				}
			}
		}
void onePipe(char* firstCommand[2], char* secondCommand[2]){
	int p_fds[2];
	pipe(p_fds);

	int pid = fork();
	if(pid == 0){
	//	close(1);
		dup2(p_fds[1], 1); //piping
		close(p_fds[0]);
		if(execv(firstCommand[0], firstCommand) == -1){
			printf("%s\n", " command not found" );
		}
	}
	else{
		pid = fork();
			if(pid == 0){
				dup2(p_fds[0], 0); //piping
				close(p_fds[1]);
				if(execv(secondCommand[0], secondCommand) == -1){
					printf("%s\n", " command not found" );
				}
			}
				else{
					close(p_fds[0]);
					close(p_fds[1]);

					printf("I am a parent\n");
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
	if(pid == 0){
		//	printf("%s\n", "in the first child1");
		dup2(p_fds[1],1); //takes first command stdout and replaces w/ 2nd command write
		for(int i = 0; i<4;i++){
			close(p_fds[i]); //close pipes
		}
		if(execv(firstCommand[0], firstCommand) == -1){
						printf("%s\n", "command not found");
					}
	}
	else //fork second child {
		{
			pid = fork();
			if(pid == 0){
			//	printf("%s\n", "in the second child");
				dup2(p_fds[0], 0); //second command stdin ->read of first
				dup2(p_fds[3],1); //second command stdout -> write of third
				for(int i = 0; i<4;i++){
					close(p_fds[i]); //close pipes
				}
				if(execv(secondCommand[0], secondCommand) == -1){
				printf("%s\n", "command not found");
			}
		}
			else{
				pid = fork();
				if(pid == 0){
				//	printf("%s\n", "in the third child");
					dup2(p_fds[2], 0);
					for(int i = 0; i<4;i++){
						close(p_fds[i]); //close pipes
					}
					if(execv(thirdCommand[0], thirdCommand) == -1){
					printf("%s\n", "command not found");
					}
				}
			}
		}
		for(int i = 0; i<4;i++){
			close(p_fds[i]); //close pipes
		}
		for(int i = 0; i<3; i++)
			wait(&status);
}
void exit_command()
{
    printf("Shell ran for %d",(int)time(NULL));
    printf("%s"," seconds and took " );
    //print((Get-History)[-1].EndExecutionTime - (Get-History)[-1].StartExecutionTime)
    printf("%d", '$end_time - $start_time');
    printf("%s\n"," seconds to execute one command" );

    exit(0);
    //printf("Timestamp: %d\n",(int)time(NULL));
}
char* commandPath(char* command){
	char* temp;
	temp = malloc(sizeof(char*)*5);
	strcpy(temp, "/usr/bin/");
	strcat(temp, command);
//	printf("this is the command path%s\n", temp);
	return temp;
}
