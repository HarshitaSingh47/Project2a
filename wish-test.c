#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char **get_input(char *);
char **get_input_redir(char *);

int index_cmd;
int EXITSHELL = 0;
int wish_cd(char**);
int wish_loop(char**);
int wish_path(char**);
int wish_exit(char**);
int execSystemCommands(char**);
int index_path = 0; 
int isbatchfile = 0; 
char** path ;
char* path_temp; 
char *builtin_cmd[] = {"cd",  "exit", "loop", "path"}; // ,

int (*builtin_func[]) (char **) = {&wish_cd , &wish_exit, &wish_loop, &wish_path}; //,,  &wish_exit

int wish_path(char** command){
    //printf("We are setting the path\n");
    //free(path);
    int i=0;

    path = malloc(10*sizeof(char*));
    if( command[1] == NULL) {free(path);
    return 0;}
    while(command[i+1]!=NULL){
        path[i]=command[i+1];
       // printf("This was appended to the path %s on position %d\n", path[i] , i);
        i++;
    }

  // printf("Printing the path added %s\n", path[0]);
    //execSystemCommands(command);
    
    return 1;
}

int wish_loop(char** command){
    //printf("HERE IN WISH LOOP with %s \n", command[1]);
    if(command[1] == NULL){
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
    

    else if (atoi(command[1]) <=  0){
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
    else{

        int i = 0 ;
        int count = atoi(command[1]);
        char* countstr = command[1];
        int loopindex = -1;
        while(command[i]!=NULL){
            if(strcmp(command[i],"$loop")==0){
            //    printf("Found $loop variable\n");
                loopindex = i;
            }
            command[i] = command[i+2];
           // printf("Shifted command %s\n", command[i+2]);
            i++;
        }

        if(loopindex != -1){
            int i  =1 ; 
            while (i!=count+1){
            sprintf(countstr, "%d", i);
            command[loopindex - 2] = countstr;
           // printf("We replaced the $loop with%s", command[loopindex]);
            execSystemCommands(command);
            i++;

        }
        }
        else{
        while(count!=0){

            execSystemCommands(command);
            count--;
        }
        }

        
    }
    return 0;
}

int wish_exit(char** command){

    if(command[1]!=NULL){
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
    else{
    // EXITSHELL = 1;
    exit(0);
    }
	return 0;


}

int wish_cd(char** command){

    if (command[1] == NULL) 
	{
		char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
	} 
	else 
	{
		if (chdir(command[1]) != 0) 
		{
			char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
		}
	}
	return 1;
}

int execPathCommands(char** args, char* path_temp){
    pid_t child_pid;
    int stat_loc;
    int ret = 0;
    child_pid = fork();
  //  printf("Checking is path is still here %s\n", path[1]);

    if (child_pid == 0) {
            
        ret = execl(path_temp, args[0], NULL);
        //printf("The output for access is %d and the path is %s\n", access(path[0], X_OK), path[0]);
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        
    } else {
        waitpid(child_pid, &stat_loc, WUNTRACED);
    }
    return ret;
}

int execRedirCommands(char** command,char* filename){
    pid_t child_pid;
    int stat_loc;
    int ret = 0;
    child_pid = fork();
   //printf("running redir commands %s\n", filename);

    if (child_pid == 0) {
        close(STDOUT_FILENO);
        int ret = open(filename, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);  
       // printf("ret value %d\n", ret);

        ret = execvp(command[0], command);
        //printf("The output for access is %d and the path is %s\n", access(path[0], X_OK), path[0]);
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        
    } else {
        waitpid(child_pid, &stat_loc, WUNTRACED);
    }
    return ret;

}

int execSystemCommands(char** command){
    pid_t child_pid;
    int stat_loc;
    int ret = 0;
    child_pid = fork();
  //  printf("Checking is path is still here %s\n", path[1]);

    if (child_pid == 0) {
            
        ret = execvp(command[0], command);
        //printf("The output for access is %d and the path is %s\n", access(path[0], X_OK), path[0]);
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
        
    } else {
        waitpid(child_pid, &stat_loc, WUNTRACED);
    }
    return ret;

}

int execCommands(char** command){
    int ret = 0;
    int num = sizeof(builtin_cmd)/sizeof(char *);
    if(command[0]==NULL){
        return 1; 
    }

    
    for(int i=0; i< num ; i++){
     //   printf("Checking in exec commands %s %s\n", command[0], builtin_cmd[i]);

        if(strcmp(command[0],builtin_cmd[i])==0){
            return (*builtin_func[i])(command);
        }


    }

    ret = execSystemCommands(command);

    return ret;
}
int checkRedirection(char** command, char** output_filename){
   // printf("Checking redirection in method\n");
    int i,j; 
    int count =0;
    for(i=0; command[i]!=NULL; i++){
       // printf("Printing the command %s\n", command[i]);
        if(strcmp(command[i],">")==0){
         //   printf("Found at %d\n", i);
            count++;
            if(i==0)
                return -1; 

        }

        
        }

    //     for(i=0; command[i]!=NULL; i++){
    //    // printf("Printing the command %s\n", command[i]);

    //     if(strstr(command[i],">")!=NULL) return 2; 
    //     }

        if(count==0) return 0; 



        if(count>1){

           
            return -1; 

        }

        

        
        else{

            for(i=0; command[i]!=NULL;i++){
                if(strcmp(command[i],">")==0){

                    //free(command[i]);

                

                if(command[i+1]!=NULL && command[i+2]==NULL) *output_filename = command[i+1];
                else {
                    
                    return -1; 
                }

                for(j=i;command[j-1]!=NULL; j++){
                    command[j] = command[j+2];
                }
                
                return 1; 
                }
            }
        }

        return 0;
    
}

int runbatchmode(char filename[100]){
    FILE *fptr;
	char line[200];
	char **args;
	fptr = fopen(filename, "r");
    int isredirect; 
    char* output_filename;
    
    path_temp = malloc(32*sizeof(char));
    //path_temp = "/bin";
    
	if (fptr == NULL)
	{
		char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
		return -1;
	}
	else
	{

		//printf("\nFile Opened. Parsing. Parsed commands displayed first.");
        
		while(fgets(line, sizeof(line), fptr)!= NULL)
		{
			//printf("\n%s", line);
            //printf("Loop refresh...checking if path exists %s\n",path_temp);
            isbatchfile = 0;
			args=get_input(line);
            // if(strstr(line,".sh")!= NULL) {
            //     if(path_temp!=NULL){
            //     //printf("Found a batch file\n");
            //     isbatchfile = 1; 
            //     strcat(path_temp,"/");
            //     strcat(path_temp,line);
            //    // printf("this is path temp now%s\n", path_temp);
            //    // printf("%d\n", access(path_temp, X_OK));
            //     if(access(path_temp, X_OK)==0){
            //         execPathCommands(args,path_temp);
            //     }

            //     else{
            //     char error_message[60] = "An error has occurred nt found in access\n";
            //     write(STDERR_FILENO, error_message, strlen(error_message));

            //     }

            //     }
            // }
            
            
            isredirect = checkRedirection(args, &output_filename);
            // printf("Output from isredirect %d\n", isredirect);
            if(isredirect==1) {
              // printf("found redirection in file %s!!!\n", output_filename);


                
            }

            if(isredirect == 2){
                //printf("Found implicit redirection %s\n", args[1]);
                char** breakdown; 
                char* filename;
                breakdown = get_input_redir(args[1]);
                filename = breakdown[1];
                args[2] = NULL;
                execRedirCommands(args,filename);
                

            }
            else if(isredirect==-1) {

               char error_message[30] = "An error has occurred\n";
               write(STDERR_FILENO, error_message, strlen(error_message));

            }
           // printf("Returned from get input with value in c[0] as %s\n", args[0]);
			else {
                //printf("found no redirection\n");
                execCommands(args);
            //    printf("Is this a .sh file? %d\n", isbatchfile);
            //    printf("This is where the path is set%s\n",path[0]);

                // if(path[0]!=NULL){
                // strcpy(path_temp,path[0]);
            //    printf("%s\n", path_temp);
                }

                // else{
                // char error_message[30] = "An error has occurred\n";
                // write(STDERR_FILENO, error_message, strlen(error_message));
                // }


               }
		}


    free(args);
	fclose(fptr);
	return 1;
	}
	



int main(int argc, char **argv) {
    int i;
    
        if(argc==2){

            i  = runbatchmode(argv[1]);
            if(i==-1) return 1;
        }
        else if(argc==1){
        }
        else{
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));

            return 1;
        }

    return 0;
}


char **get_input_redir(char *input) {

    //printf("We are in get input\n");
    char **tokens = (char **)malloc(sizeof(char *) * 64);
	char *token;
	char delim[10] = ">";
	int index_cmd = 0, bufsize = 64;
	if (!tokens)
	{
		printf("\nBuffer Allocation Error.");
		exit(EXIT_FAILURE);
	}
	token = strtok(input, delim);
	while (token != NULL)
	{
		tokens[index_cmd] = token;
		index_cmd ++;
		if (index_cmd >= bufsize)
		{
			bufsize += 64;
			input = realloc(input, bufsize * sizeof(char *));
			if (!input) // Buffer Allocation Failed
			{
			printf("\nBuffer Allocation Error.");
			exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, delim);
	}
	tokens[index_cmd] = NULL;
	return tokens;
   
}     

char **get_input(char *input) {

    //printf("We are in get input\n");
    char **tokens = (char **)malloc(sizeof(char *) * 64);
	char *token;
	char delim[10] = " \t\n\r\a";
	int index_cmd = 0, bufsize = 64;
	if (!tokens)
	{
		printf("\nBuffer Allocation Error.");
		exit(EXIT_FAILURE);
	}
	token = strtok(input, delim);
	while (token != NULL)
	{
		tokens[index_cmd] = token;
		index_cmd ++;
		if (index_cmd >= bufsize)
		{
			bufsize += 64;
			input = realloc(input, bufsize * sizeof(char *));
			if (!input) // Buffer Allocation Failed
			{
			printf("\nBuffer Allocation Error.");
			exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, delim);
	}
	tokens[index_cmd] = NULL;
	return tokens;
   
}     