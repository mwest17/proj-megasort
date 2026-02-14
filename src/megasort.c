#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

char* splitProg;
char* sortProg;
char* mergeProg;

long maxLines;

void splitCall(char* inputFile, long lines) {
    pid_t childPID = fork();

    if (childPID == 0) {
        execl(splitProg, inputFile, lines, (char *) NULL);
    } else {
        waitpid(childPID);
    }
}


void sortCall(char* inputFileName, char* outPutFileName) {
    
    
    // Stores to file
}


void mergeCall() {
    // Fork
    // Exec if child
    // Wait if parent
}

void mergeSort(char* inputFileName, char* outputFileName, long lineCount) { //(FILE* inputFile, char* inputFileName, FILE* outputFile, long lineCount) {
    // Base Case: We can sort directly
    if (lineCount <= maxLines) {
        sortCall(inputFileName, outputFileName);
        return;
    }

    // lineCount > maxLines so recursively sort files

    // Split file 
    long newLineCount = (lineCount + 2 - 1) / 2; // NEED TO HANDLE ODD LENGTHS
    splitCall(inputFileName, newLineCount);
    
    
    // Create new output Files
    FILE* childOutput = fopen("output1.txt", "w");
    FILE* parentOutput = fopen("output0", "w");

    FILE* childInput = fopen("part1.txt", "r");
    FILE* parentInput = fopen("part0.txt", "r");

    // Go into new directory (so workspace isn't cluttered)
    pid_t PID = getpid();
    char newDir[8];
    sprintf(newDir, "%d", PID); 
    mkdir(newDir);
    chdir(newDir);

    // Create a child process (fork)
    pid_t childPID = fork() == 0;

    if (childPID == 0) { // Child process
        // Both recursively call mergeSort() on their corresponding files
        mergeSort(childInput, childOutput, newLineCount);
    } else {
        mergeSort(parentInput, parentOutput, newLineCount);
    }

    chdir("..");

    // If parent, then we need to merge files
    if (childPID != 0) {
        waitpid(childPID); // Must wait for child to be done sorting
        mergeCall(parentOutput, childOutput, outputFile); // Merge files together
    }

    fclose(childInput);
    fclose(parentInput);
    fclose(parentOutput);
    fclose(childOutput);

    return;
}


int main(int argc, char* argv[]) {
    if (argc != 6) {
        printf("Invalid number of arguments: %d\n", argc);
    } 

    splitProg = argv[1]; // Append path to current directory to front
    sortProg = argv[2];
    mergeProg = argv[3];

    char* inputPath = argv[4];
    maxLines = stoi(argv[5]);
    char* outputPath = argv[6];

    FILE* inputFile = fopen(inputPath, "r");
    FILE* outputFile = fopen(outputPath, "w");

    long lineCount; // TODO**
    // Find number of lines in file
        // This way costly io to read # lines is only taken once

    mergeSort(inputFile, inputPath, outputFile, lineCount);

    return 0;
}