#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
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
void built_in(tokenlist * tokens);
char * pathSearch(tokenlist * tokens, tokenlist * tokenpath);


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
		//externalCommmand(tokenpath, tokens);
		built_in(tokens);
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
	printf("this is the input %s\n", input);
	printf("this is the output %s\n", output);
	printf("this is the input spot %d\n", inSpot);
	printf("this is the output spot %d\n", outSpot);

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
		printf("the size is: ");
		printf("%d\n", size );
		 x[size-1] = NULL;

//file redirection stuff
char *x2[size];
int fd1, fd2;

bool dash = false;
if(arrow == true) // make x only command
{
	for(int i = 0; i<size-1;i++){ // checks for commands like ls -al
		if(*x[i] == '-')
			{
				printf("%s\n", "found dash");
				dash = true;
			}
		}
	}
		if(dash == true){
				x2[0] = x[0];
				x2[1] = x[1];
				printf("%s\n", x2[1] );
				x2[2] = '\0';
			}
		else{
			x2[0] = x[0];
			printf("x2: %s\n", *x2);
			x2[1] = '\0';
		}
			if(stOut == true && stIn == true){
				printf("%s\n", "made it here both");
				if(inSpot > outSpot){
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
		//	int	fd1 = open(output,O_RDWR| O_CREAT, 0777);
			int pid = fork();
			if(pid == 0){
				printf("I am a child\n");
						//print to file
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
				// else if(stIn == true && stOut == true && larger == false){
				//
				// }
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
						execv(x[0], x); //originally x
					}
				// if(execv(x[0], x) == -1);
				// 	printf("%s\n", "command not found");

				 //taken from recitation, needs correction
				printf("it didnt work\n");
			}
			else
			{
				printf("I am a parent\n");
				waitpid(pid, NULL, 0);
			}
		}
	}

	void built_in(tokenlist * tokens){
		if(strcmp(tokens->items[0], "cd") == 0){
			if(tokens->size == 1)
				chdir(getenv("HOME"));
			//char * path = tokens->items[1];

		}
		else if(strcmp(tokens->items[0], "jobs") == 0){

		}
		else if(strcmp(tokens->items[0], "exit[2]") == 0){

		}
		else if(strcmp(tokens->items[0], "echo") == 0){

		}
	}

	char * pathSearch(tokenlist * tokens, tokenlist * tokenpath){
		for(int i = 0; i < 10; i++){
			int length = strlen(tokens->items[0]);
			strncat(tokenpath->items[i], "/", 1);
			strncat(tokenpath->items[i], tokens->items[0], length);
			//printf("%s\n", tokenpath->items[i]);
		}
		for(int i = 0; i < 10; i++){
		int fd = access(tokenpath->items[i], F_OK);
		if(fd == -1)
			;
		else if(fd == 0){
			//printf("fd: %d\n", fd);
			return tokenpath->items[i];
			break;
		}
			//printf("%s\n", "found");
	}

	}
