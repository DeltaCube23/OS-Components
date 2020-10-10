#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int flag = 0; // 1 when exit
int batch_file = 0; // 1 for batch input


int cd(char **ptr_str_arr, int idx)
{
	int status = 0, pid = 0;
	while((pid = wait(&status)) > 0) ;
	
	if(ptr_str_arr[idx+1] == NULL)
		printf("\nToo few arguments");
	else if(ptr_str_arr[idx+2] != NULL)
		printf("\nToo many arguments");
	else
	{
		if(chdir(ptr_str_arr[idx+1]) != 0)
			printf("\nInvalid arguments");
	}
	
	while(ptr_str_arr[++idx] != NULL) ;
	return idx;
}


int exit_shell(char **ptr_str_arr, int idx)
{
	if(ptr_str_arr[idx+1] == NULL)//if it has no parameters
	{
		flag = 1;
		idx++;
	}
	else
	{
		while(ptr_str_arr[++idx] != NULL) ;
		printf("\nexit has no parameters");
	}
	return idx;
}


//Separates each word in different array cd file -> [ ['c', 'd'], ['f', 'i', 'l', 'e'] ]
char **parse(char *line)
{
	int buffer_size = 20;
	int arr_size = buffer_size, cur_size = 0, next = 0;
	char **var = (char **)malloc(arr_size * (sizeof(char *)));
	
	if(var == NULL)
	{
		printf("\nMemory allocation failed");
	}
	
	while(line[next] != 10) // 10 is lf in ASCII which is linefeed or newlin char
	{
		if(cur_size == (arr_size - 1))
		{
			arr_size += buffer_size;
			var = (char **)realloc(var, arr_size * (sizeof(char *)));
			if(var == NULL)
			{
				printf("\nMemory allocation failed");
			}
		}
		
		//if it is semicolon end it with NULL
		if(line[next] == ';')
		{
			var[cur_size] = NULL;
			next++;
			cur_size++;
			continue;
		}
		
		int word_idx = 0; //if word
		if((line[next] != ' ') && (line[next] != 10))
		{
			var[cur_size] = (char *)malloc(20 * sizeof(char));
			while(line[next] != ';' && line[next] != ' ' && line[next] != 10)
			{
				var[cur_size][word_idx] = line[next];
				word_idx++;
				next++;
			}
			var[cur_size][word_idx] = '\0';
			cur_size++;
		}
		//skip spaces
		if(line[next] == ' ')
			while(line[++next] == ' ') ;
	}
	//NULL followed by an array with null char '\0' to show end of line
	var[cur_size] = NULL;
	cur_size++;
	var[cur_size] = (char *)malloc(1 * sizeof(char));
	var[cur_size][0] = '\0';
	return var;
}


//fork creates new child process different from parent since it returns different fork value.
//execvp provide an array of pointers to null-terminated strings that represent the argument list available to the new program. 
//The first argument, by convention, should point to the filename associated with the file being executed. 
//The array of pointers must be terminated by a NULL pointer. 
//wait is used for parent process to wait for it's child processes.
		
void execute(char **ptr_str_arr)
{
	int idx = 0, status = 0;
	pid_t pid = 0;
	
	do{
		// skip NULLs
		if(ptr_str_arr[idx] == NULL)
			while(ptr_str_arr[++idx] == NULL) ;
		
		if(strcmp(ptr_str_arr[idx], "exit") == 0)
			idx = exit_shell(ptr_str_arr, idx);
		else if(strcmp(ptr_str_arr[idx], "cd") == 0)
			idx = cd(ptr_str_arr, idx);
		else if(ptr_str_arr[idx][0] != '\0')
		{
			pid = fork();
			if(pid < 0)
			{
				printf("\nFork error");
				break;
			}
			else if(pid == 0)//child
			{
				if(execvp(ptr_str_arr[idx], &(ptr_str_arr[idx])) == -1)
					printf("\nError in child process");
				exit(0);
			}
			else//parent
			{
				while(ptr_str_arr[++idx] != NULL)
					;
			}
		}
		
		if(ptr_str_arr[idx] == NULL)
			while(ptr_str_arr[++idx] == NULL) ;
	}while(ptr_str_arr[idx][0] != '\0');
	//wait for child process value should be 0 if child is done
	if(pid > 0)
		while((pid = wait(&status)) > 0) ;
}
			
		
void loop()
{
	char *line;
	char **ptr_str_arr;
	int bytes;
	while(flag == 0)
	{
		if(batch_file == 0)
			printf("\ncli>>");
		
		size_t buffer_size = 20;
		line = (char *)malloc(buffer_size * sizeof(char));
		bytes = getline(&line, &buffer_size, stdin);
		
		if(line == NULL) // error in malloc
		{
			printf("\nMemory allocation falied");
			continue;
		}
		else if(bytes == 1) // no input only newline
		{
			continue;
		}
		
		ptr_str_arr = parse(line);
		execute(ptr_str_arr);
	}
}

		
int main(int argc, char **argv)
{
	if(argc > 1)
	{
		if(argc > 2)
		{
			printf("\nExtra arguments");
			exit(0);
		}
		else
		{
			freopen(argv[1], "r", stdin);
			batch_file = 1;
		}
	}
	printf("\nEntering . . . ");
	loop();
	printf("\nTerminating . . .");
	return 0;
}
		
