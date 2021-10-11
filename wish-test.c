#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

char **get_input(char *);
int index_cmd;
char** path ; 
int index_path = 0; 

int main() {
    char **command;
    pid_t child_pid;
    int stat_loc;
    char* buffer;
    size_t bufsize = 32;
    path = (char**) malloc(bufsize*sizeof(char*));

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

    return 0;
}

char **get_input(char *input) {
    char **command = malloc(8 * sizeof(char *));
    char *separator = " ";
    char *parsed;
    index_cmd = 0;

    parsed = strtok(input, separator);
    while (parsed != NULL) {
        command[index_cmd] = parsed;
        index_cmd++;

        parsed = strtok(NULL, separator);
    }

    command[index_cmd] = NULL;
    return command;
}