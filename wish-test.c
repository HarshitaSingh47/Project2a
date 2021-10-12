#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char **get_input(char *);
int index_cmd;
int EXITSHELL = 0;
int wish_cd(char**);
int wish_loop(char**);
int wish_path(char**);
int wish_exit(char**);
int execSystemCommands(char**);
int index_path = 0 ; 
char *builtin_cmd[] = {"cd",  "exit", "path", "loop"}; // "path",

int (*builtin_func[]) (char **) = {&wish_cd , &wish_exit, &wish_path, &wish_loop}; //,&wish_path,  &wish_exit

int wish_path(char** command){
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
        while(command[i]!=NULL){
            command[i] = command[i+2];
           // printf("Shifted command %s\n", command[i+2]);
            i++;
        }

        while(count!=0){
            execSystemCommands(command);
            count--;
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

int execSystemCommands(char** command){
    pid_t child_pid;
    int stat_loc;
    int ret = 0;
    child_pid = fork();
    if (child_pid == 0) {
            
        ret = execvp(command[0], command);
        printf("This won't be printed if execvp is successul\n");
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

        // if(strstr(command[i],">")!=NULL){
        //     int index = strstr(command[i],">");
        //     int j = index;
        //     int k=0;
        //     while(command[i][j]!='\0'){
        //         output_filename[k]= command[i][j];
        //         j++;
        //         k++;

        //     }

        //     output_filename[k]='\0';
        // }
        }
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
			args=get_input(line);
            isredirect = checkRedirection(args, &output_filename);
            // printf("Output from isredirect %d\n", isredirect);
            if(isredirect==1) {
                printf("found redirection in file %s!!!\n", output_filename);
                

            }
            else if(isredirect==-1) {

               char error_message[30] = "An error has occurred\n";
               write(STDERR_FILENO, error_message, strlen(error_message));

            }
           // printf("Returned from get input with value in c[0] as %s\n", args[0]);
			else {
                //printf("found no redirection\n");
                execCommands(args);
               }
		}
	}
	free(args);
	fclose(fptr);
	return 1;
}


int main(int argc, char **argv) {
    char **command;
    pid_t child_pid;
    int stat_loc;
    char* buffer;
    size_t bufsize = 32;
    char **path = calloc(32, sizeof(char *));
    int i;
    
        if(argc==2){

            i  = runbatchmode(argv[1]);
            if(i==-1) return 1;
            
        
        }
        else if(argc==1){

         while (1) {

        buffer = (char *)malloc(bufsize * sizeof(char));
        if( buffer == NULL)
        {
            perror("Unable to allocate buffer");
            exit(1);
        }

        printf("> wish ");
        getline(&buffer,&bufsize,stdin);
        //printf("%s",buffer);
        buffer[strcspn(buffer, "\n")] = 0;

        command = get_input(buffer);
       // printf("This is the size: %d\n",index_cmd);

        if (!command[0]) {      /* Handle empty commands */
            free(buffer);
            free(command);
            continue;
        }

        else if(strcmp(command[0],"path")==0){

           
            if(index_cmd==1) {
                *path = NULL;
                index_path = 0;
            }

            else{
                int l;
                int m = 1; 
                l = index_path;
                int ele = index_cmd-1;
                printf("This is the index the path is at %d\n", index_path);
                printf("The size of the command array is %d\n", index_cmd);

                while (ele!=0)
                {
                    if(command[m]!=NULL){
                    printf("Adding element to position %d\n",l);
                    path[l] = command[m];
                    printf("Writing %s to the path\n",command[m]);
                    printf("This is what is written %s\n",path[l]);
                    l++;
                    m++;
                    ele--;
                    }
                    else break;

                    /* code */
                }

                index_path = l; 
                printf("This is where the final index is at %d\n",index_path);
                
            }
            int n = 0 ;
            printf("This is where the final index is at %d\n",index_path);

            while(n < index_path){

                printf("%s\t", path[n]);
                n++;

            }

        

        }

        else if(strcmp(command[0],"exit")==0){
            if(index_cmd == 1)
            exit(0);
            else {

                char error_message[30] = "An error has occurred\n";
                write(STDERR_FILENO, error_message, strlen(error_message));

            }

        }

        else if(strcmp(command[0],"loop")==0){
            int i = 0;
            if(strcmp(command[index_cmd-1],"$loop")!=0){
                char** command2  = (char**)malloc(sizeof(char*)*3);

                int j= 0; 

                while(j<index_cmd){
                    command2[j] = command[j+2];
                    j++;
                }
                command2[j] = NULL;

                

                
                int limit = atoi(command[1]);


                while(i<limit){
                i++;
                child_pid = fork();
                if (child_pid == 0) {
                     
                    /* Never returns if the call is successful */
                    execvp(command2[0], command2);
                    printf("This won't be printed if execvp is succesfull\n");
                } else {
                    
                    waitpid(child_pid, &stat_loc, WUNTRACED);
                }
               

                }

                
                free(command2);
            
            }

            else{

                printf("We are in the variable loop\n");
                int count = atoi(command[1]);
                int k = 1; 

                char** command2  = (char**)malloc(sizeof(char*)*3);

                int j= 0; 

                while(j<index_cmd-1){
                    command2[j] = command[j+2];
                    j++;
                }
                while(k <= count){

                    command2[j] = k;
                  
                    command2[j+1] = NULL;



                    child_pid = fork();
                    if (child_pid == 0) {
                        
                        /* Never returns if the call is successful */
                        execvp(command2[0], command2);
                        printf("This won't be printed if execvp is succesfull\n");
                    } else {
                        
                        waitpid(child_pid, &stat_loc, WUNTRACED);
                    }

                    k++;

                }

                free(command2);

                }



            }
            
       
            
        

        else if(strcmp(command[0],"cd") == 0 ){

            if(index_cmd==2){

            
            child_pid = fork();
            if (child_pid == 0) {
                /* Never returns if the call is successful */
                int ret = chdir(command[1]);
                if(ret!=0){
                    char error_message[30] = "An error has occurred\n";
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
                
            } else {
                waitpid(child_pid, &stat_loc, WUNTRACED);
            }

        }

        else{
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message)); 
        }
        }

        else if(strcmp(command[1],">")==0){
            if(index_cmd != 3){
            
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message)); 


            }

            else{
                //do something
                child_pid = fork();
                if (child_pid == 0) {
                    /* Never returns if the call is successful */
                    close(STDOUT_FILENO);
                    open(command[3], O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
                    execve(command[0], command, path);
                    printf("This won't be printed if execve is successul\n");
                } else {
                    waitpid(child_pid, &stat_loc, WUNTRACED);
                }
            }
        }

        else if(strstr(command[0],"sh")){

        child_pid = fork();
        if (child_pid == 0) {
            /* Never returns if the call is successful */
            execve(command[0], command, path);
            printf("This won't be printed if execve is successul\n");
        } else {
            waitpid(child_pid, &stat_loc, WUNTRACED);
        }

        }

        else{
        child_pid = fork();
        if (child_pid == 0) {
            /* Never returns if the call is successful */
            execvp(command[0], command);
            printf("This won't be printed if execvp is successul\n");
        } else {
            waitpid(child_pid, &stat_loc, WUNTRACED);
        }

        }

        free(buffer);
        free(command);
    }
    }

    else {
        
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message)); 
        return 1; 
    }

    return 0;
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
    // char **command = malloc(8 * sizeof(char *));
    // char *separator = " >\n";
    // char *parsed;
    // index_cmd = 0;
    // printf("We are in get input\n");
    // parsed = strtok(input, separator);
    // while (parsed != NULL) {
        
    //     command[index_cmd] = parsed;
    //     index_cmd++;

    //     parsed = strtok(NULL, separator);
    // }

    // command[index_cmd] = NULL;
    // return command;
}