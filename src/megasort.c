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

long maxLines;

// Makes a call to the split program 
void splitCall(char* inputFile, long lines) {
    pid_t childPID = fork();

    if (childPID == 0) {
        char linesString[1 << 13];
        sprintf(linesString, "%ld", lines); 

        execlp(splitPath, "split_prog", inputFile, linesString, (char *) NULL);
        
        printf("Split Program Call Failed: %d\n", errno);
        exit(1);
    } else {
        waitpid(childPID, NULL, 0);
    }
}

// Makes a call to the sort program, giving it input from the inputPath file and storing output to outputPath file
void sortCall(char* inputPath, char* outputPath) {
    pid_t childPID = fork();
    
    if (childPID == 0) {
        int outputFile = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        int inputFile = open(inputPath, O_RDONLY);

        // Store stdout to outputFile
        dup2(outputFile, STDOUT_FILENO);
        // Replace stdin with inputFile
        dup2(inputFile, STDIN_FILENO);
        close(outputFile);
        close(inputFile);

        execlp(sortPath, "sort_prog", (char *) NULL);

        printf("Sort Program Call Failed: %d\n", errno);
        exit(1);
    } else {
        waitpid(childPID, NULL, 0);
    }
    
}

// Makes a call to the merge program and stores in outputPath
void mergeCall(char* parentOutput, char* childOutput, char* outputPath) {
    pid_t PID = getpid();
    pid_t childPID = fork();
    char tmpOutput[1 << 13];
    sprintf(tmpOutput, "tmpOutput%d.txt", PID);

    if (childPID == 0) {
        // Make a tmp output file so output doesn't overwrite any input files
        int outputFile = open(tmpOutput, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        dup2(outputFile, STDOUT_FILENO);
        close(outputFile);

        execlp(mergePath, "merge_prog", parentOutput, childOutput, (char *) NULL);
        printf("Merge Program Call Failed: %d\n", errno);

        exit(1);
    } else {
        waitpid(childPID, NULL, 0);

        // Replace contents of outputPath with merged
        rename(tmpOutput, outputPath);
    }
}


void mergeSortNodes(int left, int right) {
    if (left > right) return;

    // If only sorting 1 file, simply sort
    if (left == right) {
        char inputName[128], outputName[128];
        sprintf(inputName, "part%d.txt", left);
        sprintf(outputName, "output%d.txt", left);

        sortCall(inputName, outputName);
        return;
    }

    // Divide remaining work in 2
    int mid = (left + right) / 2;

    // Create child to process left half of files
    pid_t leftChild = fork();
    if (leftChild == 0) { 
        mergeSortNodes(left, mid); 
        exit(0); 
    }

    // Create child to process right half of files
    pid_t rightChild = fork();
    if (rightChild == 0) { 
        mergeSortNodes(mid + 1, right); 
        exit(0); 
    }

    // Wait for both to finish
    waitpid(leftChild, NULL, 0);
    waitpid(rightChild, NULL, 0);

    // Combine both left and right sets of files
    char input1Name[128], input2Name[128];
    sprintf(input1Name, "output%d.txt", left);
    sprintf(input2Name, "output%d.txt", mid + 1);
    mergeCall(input1Name, input2Name, input1Name);
    remove(input2Name);

    exit(0);
}


void mergeSort(char* inputPath, char* outputPath, long lineCount) {
    // Split file into allowed sorting size
    splitCall(inputPath, maxLines);
    // Calculate the number of files created
    long numFiles = (lineCount + maxLines - 1) / maxLines;

    // Start recursive sorting
    pid_t childPID = fork();

    if (childPID == 0) {
        mergeSortNodes(0, numFiles - 1);
    }

    waitpid(childPID, NULL, 0);

    rename("output0.txt", outputPath);
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
    return counter;
}


int main(int argc, char* argv[]) {
    if (argc != 7) {
        printf("Invalid number of arguments: %d\n", argc);
    } 

    splitPath = argv[1];
    sortPath  = argv[2];
    mergePath = argv[3];

    char* inputPath = argv[4];
    maxLines = atoi(argv[5]);
    char* outputPath = argv[6];

    long lineCount = countLines(inputPath); 

    mergeSort(inputPath, outputPath, lineCount);

    return 0;
}