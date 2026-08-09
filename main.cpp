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
            // Handle >
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
                // stdout → file
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
