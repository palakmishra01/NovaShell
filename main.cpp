#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main() {

    while(true){
    std::cout << "mini-shell> ";
    string command;
    std::getline(std::cin, command);
    if(command == "exit") {
        std::cout << "Exiting mini-shell." << std::endl;
        return 0;
    }
    pid_t pid = fork();
    if(pid < 0) {
        std::cerr << "Fork failed." << std::endl;
        return 1;
    }
    else if (pid == 0) {
        // Child process
        execlp(command.c_str(), command.c_str(), nullptr);

        // Only reached if exec fails
        cerr << "Command execution failed\n";
        exit(1);
    }
    else {
        // Parent process
        waitpid(pid, nullptr, 0);
    }
    }
    return 0;
}
