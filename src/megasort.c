#include <stdlib.h>
#include <stdio.h>



void splitCall() {
    
}


void sortCall() {


}


void mergeCall() {
    // Fork
    // Exec if child
    // Wait if parent
}

void mergeSort() { // input file handle, isChild flag, # Lines, 
    // If len of file <= max_lines
        // Sort file
        // return
    
    // Create a child process (fork)
    // Store if parent or child
    // Split file 
    // Open both smaller files
    // Both recursively call mergeSort() on their corresponding files
        // appropriate isChild flag, # Lines / 2 (ensure if odd # lines works), 
    
    // If child, kill process
    // If parent, wait until child is finished, 
        //then merge child's file 
        // Must save stdout output to a file
        // Then if we are child too, kill
        // Else return (might be fine to just return no matter what)
}


int main(int argc, char* argv[]) {
    if (argc != 6) {
        printf("Invalid number of arguments: %d\n", argc);
    } 

    char* splitProg = argv[1];
    char* sortProg = argv[2];
    char* mergeProg = argv[3];

    char* inputFile = argv[4];
    long maxLines = stoi(argv[5]);
    char* outputFile = argv[6];

    // Find number of lines in file
        // This way costly io to read # lines is only taken once

    mergeSort();

    return 0;
}