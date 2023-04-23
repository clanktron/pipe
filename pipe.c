#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // return error if no args are specified
    if (argc <= 1) {
        fprintf(stderr, "Need at least one argument\n");
        exit(EINVAL);
    }

    // init file descriptor arrays
    int oldfd[2];
    int newfd[2];
    const char child_error[] = "Child Error: ";
    const char parent_error[] = "Parent Error: ";

    // loop over given commands
    for (int i = 1; i < argc; i++) {

        // if there is a next command create new pipe
        if (i < argc-1) {
            pipe(newfd);
        }

        int return_code = fork();

        // child process
        if (return_code == 0) {
            // if there was a previous command
            if (i > 1) {

                if (close(oldfd[1]) == -1) {
                    perror(child_error);
                    return errno;
                }
                if (dup2(oldfd[0], STDIN_FILENO) == -1) {
                    perror(child_error);
                    return errno;
                }; 
                if (close(oldfd[0]) == -1) {
                    perror(child_error);
                    return errno;
                }

            } 

            // if there is a next command
            if (i < argc-1) {

                if (close(newfd[0]) == -1) {
                    perror(child_error);
                    return errno;
                };
                if (dup2(newfd[1], STDOUT_FILENO) == -1) {
                    perror(child_error);
                    return errno;
                };
                if (close(newfd[1]) == -1) {
                    perror(child_error);
                    return errno;
                };
            }         

            if (execlp(argv[i], argv[i], NULL) == -1) {
                perror(child_error);
                return errno;
            }

        // parent process
        } else if (return_code > 0) {

            // if there was a previous command
            if (i > 1) {
                if (close(oldfd[0]) == -1) {
                    perror(parent_error);
                    return errno;
                };
                if (close(oldfd[1]) == -1) {
                    perror(parent_error);
                    return errno;
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
            return errno;
        }

    }

    return 0;
}
