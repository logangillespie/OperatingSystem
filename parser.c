#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
	int size;
	char **items;
} tokenlist;

char *get_input(void);
tokenlist *get_tokens(char *input);

tokenlist *new_tokenlist(void);
void add_token(tokenlist *tokens, char *item);
void free_tokens(tokenlist *tokens);

bool tokenCheck(char *token);
bool dollarSignCheck(char *token);
char *searchDollar(char *token);
char* returnenv(char *token);
void prompt ();

int main()
{
	while (1) {
		prompt();

		/* input contains the whole command
		 * tokens contains substrings from input split by spaces
		 */

		char *input = get_input();
		printf("whole input: %s\n", input);

		tokenlist *tokens = get_tokens(input);
		//char *newToken, dollarCheck, command;

		for (int i = 0; i < tokens->size; i++) {
			printf("token %d: (%s)\n", i, tokens->items[i]);

			char *newToken = tokens->items[i];
			char* example = returnenv(newToken);
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