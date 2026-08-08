#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main() {
    while(true){
        std::cout << "mini-shell> ";
        string command;
        std::getline(std::cin, command);

        //exit wali line
        if(command == "exit") {
            std::cout << "Exiting mini-shell." << std::endl;
            return 0;
        }

        //parsing the command into arguments
        stringstream ss(command);
        vector<string> args;
        string token;
        while (ss >> token) {
            args.push_back(token);
        }

        if(args.empty()) {
            continue; // No command entered, prompt again
        }

        pid_t pid = fork();

        //fork fail
        if(pid < 0) {
            std::cerr << "Fork failed." << std::endl;
            return 1;
        }

        //child process
        else if (pid == 0) {
            vector<char*> argv;
            for (auto& arg : args) {
                argv.push_back(arg.data());
            }
            argv.push_back(nullptr);
            execvp(argv[0], argv.data());
            // Only reached if execvp fails
            cerr << "Command not found: " << args[0] << "\n";
            exit(1);
        }
        
        // Parent process
        else {
            waitpid(pid, nullptr, 0);
        }
    }
    return 0;
}
