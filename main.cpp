#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

int main() {
    while(true){
        std::cout << "mini-shell> ";
        string command;
        std::getline(std::cin, command);

        //exit wali line, build in 
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

        //build in command for changing directory
        if(args[0]=="cd"){
            if(args.size()<2){
                std::cerr << "cd: missing argument" << std::endl;
            } 
            else if(chdir(args[1].c_str()) != 0){
                std::cerr << "cd: no such file or directory: " << args[1] << std::endl;
            }
            continue;
        }

        if(args.empty()) {
            continue; // No command entered, prompt again
        }

        string outputFile;
        bool redirectOutput = false;
        string inputFile;
        bool redirectInput = false;

        for (int i = 0; i < (int)args.size(); i++) {

            if (args[i] == ">") {

                redirectOutput = true;

                // Check whether filename exists
                if (i + 1 >= (int)args.size()) {
                    cerr << "Syntax error: missing output file\n";
                    redirectOutput = false;
                    break;
                }

                outputFile = args[i + 1];

                // Remove ">" and filename
                args.erase(args.begin() + i,
                           args.begin() + i + 2);

                break;
            }
        }  
        
        for (int i = 0; i < (int)args.size(); i++) {

            if (args[i] == "<") {

                redirectInput = true;

                if (i + 1 >= (int)args.size()) {
                    cerr << "Syntax error: missing input file\n";
                    redirectInput = false;
                    break;
                }

                inputFile = args[i + 1];

                // Remove "<" and filename
                args.erase(args.begin() + i,
                        args.begin() + i + 2);

                break;
            }
        }

        if(args.empty()) {
            continue; // No command entered, prompt again
        }   

        int pipeIndex = -1;
        for (int i = 0; i < (int)args.size(); i++) {
            if (args[i] == "|") {
                pipeIndex = i;
                break;
            }
        }
        if (pipeIndex != -1) {
            vector<string> leftArgs;
            vector<string> rightArgs;
            for (int i = 0; i < pipeIndex; i++) {
                leftArgs.push_back(args[i]);
            }
            for (int i = pipeIndex + 1; i < (int)args.size(); i++) {
                rightArgs.push_back(args[i]);
            }
            if (leftArgs.empty() || rightArgs.empty()) {
                cerr << "Syntax error near pipe\n";
                continue;
            }
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("pipe");
                continue;
            }
            // First child
            pid_t pid1 = fork();

            if (pid1 < 0) {
                perror("fork");
                close(pipefd[0]);
                close(pipefd[1]);
                continue;
            }

            if (pid1 == 0) {

                // stdout → pipe
                dup2(pipefd[1], STDOUT_FILENO);

                close(pipefd[0]);
                close(pipefd[1]);

                vector<char*> argv1;

                for (auto& arg : leftArgs) {
                    argv1.push_back(const_cast<char*>(arg.c_str()));
                }

                argv1.push_back(nullptr);

                execvp(argv1[0], argv1.data());

                perror("execvp left");
                exit(1);
            }
            // Second child
            pid_t pid2 = fork();
            if (pid2 < 0) {
                    perror("fork");
                    close(pipefd[0]);
                    close(pipefd[1]);
                    waitpid(pid1, nullptr, 0);
                    continue;
            }

            if (pid2 == 0) {

                    // pipe → stdin
                    dup2(pipefd[0], STDIN_FILENO);

                    close(pipefd[0]);
                    close(pipefd[1]);

                    vector<char*> argv2;

                    for (auto& arg : rightArgs) {
                        argv2.push_back(const_cast<char*>(arg.c_str()));
                    }

                    argv2.push_back(nullptr);

                    execvp(argv2[0], argv2.data());

                    perror("execvp right");
                    exit(1);
            }

            // Parent
            close(pipefd[0]);
            close(pipefd[1]);

            waitpid(pid1, nullptr, 0);
            waitpid(pid2, nullptr, 0);

            continue;
        }
            pid_t pid = fork();

            //fork fail
            if(pid < 0) {
                std::cerr << "Fork failed." << std::endl;
                return 1;
            }

        //child process
        else if (pid == 0) {

            // Input redirection <
            if (redirectInput) {

                int fd = open(inputFile.c_str(), O_RDONLY);

                if (fd < 0) {
                    perror("open");
                    exit(1);
                }

                if (dup2(fd, STDIN_FILENO) < 0) {
                    perror("dup2");
                    close(fd);
                    exit(1);
                }

                close(fd);
            }

            // Output redirection >
            if (redirectOutput) {

                int fd = open(
                    outputFile.c_str(),
                    O_WRONLY | O_CREAT | O_TRUNC,
                    0644
                );

                if (fd < 0) {
                    perror("open");
                    exit(1);
                }

                if (dup2(fd, STDOUT_FILENO) < 0) {
                    perror("dup2");
                    close(fd);
                    exit(1);
                }

                close(fd);
            }

            vector<char*> argv;

            for (auto& arg : args) {
                argv.push_back(arg.data());
            }

            argv.push_back(nullptr);


            // Execute command
            execvp(argv[0], argv.data());

            // Reached only if execvp fails
            cerr << "Command not found: "
                 << args[0] << "\n";

            exit(1);
        }
       
        // Parent process
        else {
            waitpid(pid, nullptr, 0);
        }
    }
    return 0;
}
