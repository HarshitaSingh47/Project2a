#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/wait.h>

char **get_input(char *);

int main() {
    char **command;
    pid_t child_pid;
    int stat_loc;
    char* buffer;
    size_t bufsize = 32;

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
        

        if (!command[0]) {      /* Handle empty commands */
            free(buffer);
            free(command);
            continue;
        }

        else if(strcmp(command[0],"cd")==0){

            
            child_pid = fork();
            if (child_pid == 0) {
                /* Never returns if the call is successful */
                int ret = chdir(command[1]);
                
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

    return 0;
}

char **get_input(char *input) {
    char **command = malloc(8 * sizeof(char *));
    char *separator = " ";
    char *parsed;
    int index = 0;

    parsed = strtok(input, separator);
    while (parsed != NULL) {
        command[index] = parsed;
        index++;

        parsed = strtok(NULL, separator);
    }

    command[index] = NULL;
    return command;
}