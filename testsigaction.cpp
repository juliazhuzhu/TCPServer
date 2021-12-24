#include <iostream>
#include <signal.h>
#include <unistd.h>

void sighandler(int sig){

    std::cout << "recieve sig:" << sig << std::endl;
}

int main(int argc, char* argv[]){
    struct sigaction act, oact;

    act.sa_handler = sighandler;
    sigfillset(&act.sa_mask);//捕获\
    // 运行期间,屏蔽中断
    act.sa_flags = 0;

    sigaction(SIGINT, &act, &oact);

    pause();

    return 0;


}