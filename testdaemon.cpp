#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

void daemonize() {
    pid_t pid;
    pid = fork();
    if (pid <0 ) {
        std::cout <<"can't create sub process !" << std::endl;
    }else {
        if (pid != 0) {
            std::cout << pid << std::endl;
            exit ( 0);
        }
    }

    setsid();
    if (chdir("/") < 0){
        std::cout <<"chdir dir failure !" << std::endl;
        exit(-1);
    }

    int fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
}

int main(int argc, char* argv[]){

    daemonize();
    while (1){
        sleep(1);
    }

    return 0;

}