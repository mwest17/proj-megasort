#include <stdlib.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>


#include <errno.h>

char* splitPath;
char* sortPath;
char* mergePath;

// char* splitName;
// char* sortName;
// char* mergeName;

long maxLines;

void splitCall(char* inputFile, long lines) {
    pid_t childPID = fork();

    if (childPID == 0) {
        char linesString[1 << 13]; // TODO****
        sprintf(linesString, "%ld", lines); 
        int err = execl(splitPath, "split_prog", inputFile, linesString, (char *) NULL);
        
        printf("Split Program Call Failed: %d\n", errno);
        printf("%s\n", splitPath);
        
        exit(1);
    } else {
        waitpid(childPID, NULL, 0);
    }
}


// HERE ***
void sortCall(char* inputPath, char* outputPath) {
    pid_t childPID = fork();
    
    if (childPID == 0) {
        int outputFile = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        int inputFile = open(inputPath, O_RDONLY);

        dup2(outputFile, STDOUT_FILENO);
        dup2(inputFile, STDIN_FILENO);
        close(outputFile);
        close(inputFile);

        execl(sortPath, "sort_prog", (char *) NULL); // NEED TO WRITE INPUT FILE TO STDIN

        printf("Sort Program Call Failed: %d\n", errno);

        exit(1);
    } else {
        waitpid(childPID, NULL, 0);
    }
    
}


// HERE**
void mergeCall(char* parentOutput, char* childOutput, char* outputPath) {
    pid_t childPID = fork();
    
    if (childPID == 0) {
        int outputFile = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);


        char cwd[PATH_MAX]; 
        getcwd(cwd, PATH_MAX);
        printf("%s\n", cwd);
        printf("%s\n", parentOutput);
        printf("%s\n", outputPath);

        dup2(outputFile, STDOUT_FILENO);
        close(outputFile);

        execl(mergePath, "merge_prog", parentOutput, childOutput, (char *) NULL);
        printf("Merge Program Call Failed: %d\n", errno);

        exit(1);
    } else {
        waitpid(childPID, NULL, 0);
    }
}

void mergeSort(char* inputPath, char* outputPath, long lineCount) {
    // Base Case: We can sort directly
    if (lineCount <= maxLines) {
        sortCall(inputPath, outputPath);
        return;
    }

    // Split file 
    long newLineCount = (lineCount + 2 - 1) / 2; // NEED TO HANDLE ODD LENGTHS
    splitCall(inputPath, newLineCount);
    
    
    // Create new output Files
    char* childOutput = "output1.txt";
    char* parentOutput = "output0.txt";

    char* childInput = "../part1.txt";
    char* parentInput = "../part0.txt";

    // Go into new directory (so workspace isn't cluttered)
    pid_t PID = getpid();
    char newDir[8];
    sprintf(newDir, "%d", PID); 
    mkdir(newDir, 0777);
    chdir(newDir);

    // Create a child process (fork)
    pid_t childPID = fork() == 0;

    if (childPID == 0) { // Child process
        // Both recursively call mergeSort() on their corresponding files
        mergeSort(childInput, childOutput, newLineCount);
    } else {
        mergeSort(parentInput, parentOutput, newLineCount);
    }

    // If parent, then we need to merge files
    if (childPID != 0) {
        waitpid(childPID, NULL, 0); // Must wait for child to be done sorting
        mergeCall(parentOutput, childOutput, outputPath); // Merge files together
    }

    chdir("..");

    return;
}

long countLines(char* filePath) {
    FILE* file = fopen(filePath, "r");
    long counter = 0;
    char buffer[1 << 13];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, (1 << 13), file)) > 0) {
        for (size_t i = 0; i < bytesRead; i++)
        {
            if (buffer[i] == '\n') {
                counter++;
            }
        }
    }

    // if (buffer[bytesRead - 1] != '\n') {
    //     counter++;
    // }

    return counter;
}


int main(int argc, char* argv[]) {
    if (argc != 7) {
        printf("Invalid number of arguments: %d\n", argc);
    } 

    if (argv[1][0] != '/') {
        char cwd[PATH_MAX]; 
        getcwd(cwd, PATH_MAX);
        splitPath = (char*) malloc(sizeof(cwd) + sizeof(argv[1]) - 1);
        sortPath  = (char*) malloc(sizeof(cwd) + sizeof(argv[2]) - 1);
        mergePath = (char*) malloc(sizeof(cwd) + sizeof(argv[3]) - 1);

        strcpy(splitPath, cwd);
        strcpy(sortPath,  cwd);
        strcpy(mergePath, cwd);
        
        // NEED TO DEAL WITH PATH****
        int offset = (argv[1][0 == '.'])? 2 : 0;
        strcat(splitPath, argv[1] + offset);
        
        strcat(sortPath,  argv[2] + offset);
        strcat(mergePath, argv[3] + offset);
    } else {
        splitPath = argv[1];
        sortPath  = argv[2];
        mergePath = argv[3];
    }

    char* inputPath = argv[4];
    maxLines = atoi(argv[5]);
    char* outputPath = argv[6];

    long lineCount = countLines(inputPath); 

    mergeSort(inputPath, outputPath, lineCount);

    return 0;
}