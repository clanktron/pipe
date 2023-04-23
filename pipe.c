#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

    // init file descriptor arrays

    int oldfd[2];
    int newfd[2];
    const char child_error[] = "Child Error: ";
    const char parent_error[] = "Parent Error: ";

    for (int i = 1; i < argc; i++) {

        // if there is a next command create new pipe
        if (i < argc-1) {
            // fprintf(stderr, "creating new pipe...\n");
            pipe(newfd);
        }

        int return_code = fork();

        if (return_code == 0) {
            // printf("Child process %d is running...\n", i);

            // if there was a previous command
            if (i > 1) {

                // fprintf(stderr, "child process %s, closing old pipe write end %d, assigning stidn to old read end, closing old pipe read end %d...\n", argv[i], oldfd[1], oldfd[0]);
                if (close(oldfd[1]) == -1) {
                    perror(child_error);
                    return -1;
                }
                if (dup2(oldfd[0], STDIN_FILENO) == -1) {
                    perror(child_error);
                    return -1;
                }; 
                if (close(oldfd[0]) == -1) {
                    perror(child_error);
                    return -1;
                }

            } 

            // if there is a next command
            if (i < argc-1) {

                // fprintf(stderr, "child process %s, closing new pipe read end %d, assigning stdout to new pipe write end, closing new pipe write end %d...\n", argv[i], newfd[0], newfd[1]);
                if (close(newfd[0]) == -1) {
                    perror(child_error);
                    return -1;
                };
                if (dup2(newfd[1], STDOUT_FILENO) == -1) {
                    perror(child_error);
                    return -1;
                };
                if (close(newfd[1]) == -1) {
                    perror(child_error);
                    return -1;
                };
            }         

            if (execlp(argv[i], argv[i], NULL) == -1) {
                perror(child_error);
                return -1;
            }

        } else if (return_code > 0) {

            // if there was a previous command
            if (i > 1) {
                if (close(oldfd[0]) == -1) {
                    perror(parent_error);
                    return -1;
                };
                if (close(oldfd[1]) == -1) {
                    perror(parent_error);
                    return -1;
                };
            } 

            // if there is a next command
            if (i < argc-1) {
                oldfd[0] = newfd[0];
                oldfd[1] = newfd[1];
            } 

            int pid = return_code;
            int child_status;
            waitpid(pid, &child_status, 0);

            if (WEXITSTATUS(child_status) != 0) {
                fprintf(stderr, "child %d ended abnormally with exit status %d\n", i, WEXITSTATUS(child_status));
                return -1;
            }

        } else {
            fprintf(stderr, "Failed to create child process properly\n");
            return -1;
        }

    }

    return 0;
}
