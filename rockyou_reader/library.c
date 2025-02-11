#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h> // For boolean data type
#include <unistd.h> // For getcwd()
#include <string.h>
#include <sys/stat.h> // For stat()
#include <dirent.h> // For directory listing
#include <errno.h> // For errno


// Buffer alloc
#define CHUNK_SIZE 1024 * 1024 // 1 MB
#define MAX_KEYWORD_LENGTH 256
#define MAX_FILENAME_LENGTH 1024

// NOTE:  Ram alloc
#define MB1 (1L * 1024 * 1024)          // 1MB
#define MB2 (2L * 1024 * 1024)          // 2MB
#define MB4 (4L * 1024 * 1024)          // 4MB
#define MB8 (8L * 1024 * 1024)          // 8MB
#define MB16 (16L * 1024 * 1024)        // 16MB
#define MB32 (32L * 1024 * 1024)        // 32MB
#define MB64 (64L * 1024 * 1024)        // 64MB
#define MB128 (128L * 1024 * 1024)      // 128MB
#define MB256 (256L * 1024 * 1024)      // 256MB
#define MB512 (512L * 1024 * 1024)      // 512MB
#define GB1 (1L * 1024 * 1024 * 1024)   // 1 Gigabyte in bytes
#define GB4 (4L * 1024 * 1024 * 1024)   // 4 Gigabytes in bytes
#define GB8 (8L * 1024 * 1024 * 1024)   // 8 Gigabytes in bytes
#define GB16 (16L * 1024 * 1024 * 1024) // 16 Gigabytes in bytes
#define GB32 (32L * 1024 * 1024 * 1024) // 32 Gigabytes in bytes

// Functions
size_t getBufferSize(); // asks user for buffer to use in bytes
int userChoice(); // User choice to view file or search for a string
bool viewFile(const char *filename, size_t BUFFER_SIZE);// views conntents containing file
void search_in_chunk(const char *buffer, size_t BUFFER_SIZE, size_t chunk_start, const char *keyword, size_t *line_number); // called if search file is invoked
char* getCurrentWorkingDirectory();
char* getWorkingDirectory(); // Function to get the current working directory
void changeWorkingDirectory(); // Function to change the current working directory
void printCurrentWorkingDirectory(); // GET CWD
bool fileExists(const char *filename); // Check if a file exists in the current working directory
int is_usable_directory(const char *path); // Check if the directory exists
void listFilesInDirectory(); // List files in the current working directory


int main(int argc, char *argv[]) {


    // This will be used to time the 'viewFile' function and the 'searchFile' function
    // clock_t start_time, end_time;
    // start_time = clock();
    //double cpu_time_used;

    char KEYWORD[MAX_KEYWORD_LENGTH];
    char FILENAME[MAX_FILENAME_LENGTH];

    // Gets buffersize from user input
    size_t BUFFER_SIZE = getBufferSize();
    printf("[+] Ram Usage: %zu bytes\n", BUFFER_SIZE);


    // Get the working directory from the user
    char *workingDirectory;
    workingDirectory = getWorkingDirectory();
    printf("[+] (main) Working directory: %s\n", workingDirectory);

    printf("[!] Checking if the directory exists\n");
    // Edgecase for working directory
    if (workingDirectory == NULL) {
        perror("[-] Error getting working directory\n(CWD) .. exiting program\n");
        printCurrentWorkingDirectory();
        free(workingDirectory);
        return EXIT_FAILURE;
    }

    printf("[!] Checking if the directory is usable\n");

    if (is_usable_directory(workingDirectory)) {
        printf(" [+] The directory is usable.\n.. moving on\n");
    } else {
        printf("[-] The directory is not usable.\n");
        perror("[-] Error checking if the directory is usable\n.. exiting program\n");
        printCurrentWorkingDirectory();
        free(workingDirectory);
        return EXIT_FAILURE;
    }


    // Get the FILENAME from the user

    printf("****************************** \n[!] Listing files in the directory\n\n");
    listFilesInDirectory();
    printCurrentWorkingDirectory();
    printf(" ****************************"
           "\n\n[+] directory is valid.. "
           "\n[?] Enter the FILENAME to search in: ");


    // Edgecases for fileinput
    if (fgets(FILENAME, MAX_FILENAME_LENGTH, stdin) == NULL) {
        perror("[-] Error reading FILENAME, returning NULL\n .. exiting program ");
        printCurrentWorkingDirectory();
        free(workingDirectory);
        return EXIT_FAILURE;
    }
    printf("[!].. checking if FILENAME exists [enter the FILENAME again] \n");

    if (fgets(FILENAME, sizeof(FILENAME), stdin) != NULL) {
            // Remove the newline character if it exists
            FILENAME[strcspn(FILENAME, "\n")] = '\0';
            printf("[+] Filename entered: %s\n", FILENAME);
            // Check if the file exists
            if (fileExists(FILENAME)) {
                printf("[+] File '%s' exists.\n", FILENAME);
                printf("[!].. moving on ");
            } else {
                printf("[-] File '%s%s' does not exist.\n", FILENAME, "'\n.. exiting program\n");
                perror("[-] Error reading FILENAME, returning NULL\n .. exiting program ");
                return EXIT_FAILURE;
            }
        }


    printf("**** \n[+] Filename successfully entered: %s%s%s\n", FILENAME, "\n********************![!] CWD", "\n workingDirectory:\n ");
    printCurrentWorkingDirectory();


    // ask user what they want to do, view file or search for a string
    int choice;
    choice = userChoice();
    printf("[+] User choice: %d\n", choice);

    if (choice == 1) {
        double cpu_time_used;
        clock_t start_time, end_time;
        start_time = clock();

        printf("user choice is 1\n... \n viewing file\n%s", FILENAME);
        printf("Buffer size: %zu bytes\n", BUFFER_SIZE);
        printf("[!] Start Time: %ld\n", start_time);

        // Calls the viewFile function, returns bools for err / succ status
        bool viewFileStatus;
        viewFileStatus = viewFile(FILENAME, BUFFER_SIZE);
        if (viewFileStatus == 0) {
            perror("[-] Error viewing file\n");
            return EXIT_FAILURE;
        }

        // Timer upon sucess
        end_time = clock();
        cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
        printf("[+] Function Status: %b", viewFileStatus);
        printf("\nTime taken: %f %s seconds\n", cpu_time_used,
               "\n ..returning success and exiting program [copyleft] elitHaxxor\n");

        return EXIT_SUCCESS;
    } // ELSE IF CHOICE == 2; THEN


    // TODO: MAY NEED TO REMOVE ELSE
    else {


        // Get the KEYWORD from the user
        printf("[?] Enter the KEYWORD to search: ");
        if (fgets(KEYWORD, MAX_KEYWORD_LENGTH, stdin) == NULL) {
            free(workingDirectory);
            perror("[-] Error reading KEYWORD");
            return EXIT_FAILURE;
        }
        // Remove newline character if present
        size_t keyword_length = strlen(KEYWORD);
        if (KEYWORD[keyword_length - 1] == '\n') {
            KEYWORD[keyword_length - 1] = '\0';
            keyword_length--;
        }
        printf("[?] Keyword to search: %s\n", KEYWORD);

        // Remove newline character if present
        size_t filename_length = strlen(FILENAME);
        if (FILENAME[filename_length - 1] == '\n') {
            FILENAME[filename_length - 1] = '\0';
        }
        printf("[?] Enter the FILENAME to search in: %s\n", FILENAME);

        printf("[!]Opening file: %s\n", FILENAME);
        FILE *file = fopen(FILENAME, "r");
        if (file == NULL) {
            free(workingDirectory);
            free(file);
            perror("[-]Error opening file");
            return EXIT_FAILURE;
        }

        char *buffer = (char *) malloc(BUFFER_SIZE + keyword_length - 1);
        if (buffer == NULL) {
            perror("[-] Error allocating buffer");
            printf("[-] Buffer size: %zu bytes\n", BUFFER_SIZE + keyword_length - 1);
            free(workingDirectory);
            free(file);
            free(buffer);
            fclose(file);
            return EXIT_FAILURE;
        }
        clock_t start_time, end_time;
        start_time = clock();
        double cpu_time_used;

        printf("Buffer allocated with size: %zu bytes\n", BUFFER_SIZE + keyword_length - 1);
        printf("[+] Start Time: %ld\n", start_time);



        // TODO: CHECK THE LOGIC OF THIS, IF THE BUFFER IS NOT NULL, THEN CONTINUE
        size_t chunk_start = 0;
        size_t bytes_read;
        int count = 0; // Count the number of chunks read
        size_t line_number = 1;

        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
            printf("Read %zu bytes from file starting at position: %zu\n", bytes_read, chunk_start);
            buffer[bytes_read] = '\0'; // Null-terminate the buffer
            search_in_chunk(buffer, bytes_read, chunk_start, KEYWORD, &line_number);
            chunk_start += bytes_read;

            // Move the buffer's start to the position after the last overlap
            fseek(file, -(long) (keyword_length - 1), SEEK_CUR);
            chunk_start -= keyword_length - 1;
            printf("Adjusted file pointer to position: %zu\n", chunk_start);
            printf("Line: %d\n", count);
            count++; // Increment the count
        }

        // MARK: END OF OPTION 2 FROM USER INPUT (SEARCH FOR STRING)
        end_time = clock();
        printf("***************** \n Freeing buffer memory...\n");
        free(buffer);
        free(workingDirectory);
        fclose(file);
        free(file);
        printf("File closed.\n");
        // Calculate the elapsed time
        cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
        printf("********************** \n[+] Search complete. Time taken: %f seconds\n", cpu_time_used);
        printf("[+] Start time: %ld %s %ld \n", start_time, "[+] End Time: %ld\n", end_time);

        return EXIT_SUCCESS;
    }
    free(workingDirectory);
}





int userChoice() {
    int choice;
    char searchString[256];

    printf("************************** \n[?] What would you like to do?\n");
    printf("[1.] View the text file\n");
    printf("[2.] Search for a specific string in the text\n");
    printf("Enter your choice (1-2): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
           // viewFile(filename, bufferSize);
            return 1;
        case 2:
            printf("Enter the string to search for: ");
            scanf("%s", searchString);
           // searchFile(filename, bufferSize, searchString);
            return 2;
        default:
            printf("Invalid choice. Exiting.\n");
            return 0;
    }
}


// Asks user for buffer size to use in bytes and returns the size as [BUFFERSIZE]
size_t getBufferSize() {
    int choice;
    size_t bufferSize = 0;

    printf("[!] Choose the amount of RAM to use:\n*********************************************\n\n [0.] 1GB\n[1.] "
           "4GB\n[2.] 8GB\n[3.] 16GB\n[4.] 32GB\n [5.] 1MB\n[ 6.] 2MB\n[ 7.] 4MB\n [8.] 8MB\n, [9.] 16MB\n [10.] 32MB\n "
           "[11.] 64MB\n [12.] 128MB\n[ 13.] 256MB\n [14.] 512MB\n\n "
           "*********************************************\n");
    printf("Enter your choice [1-14]: [default 1MB, recommended 8MB - 64MB ]: ");
    scanf("%d", &choice);

    switch (choice) {
        case 0:
            bufferSize = GB1; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            //break;
        case 1:
            bufferSize = GB4; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
           // break;
        case 2:
            bufferSize = GB8; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;
        case 3:
            bufferSize = GB16; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;
        case 4:
            bufferSize = GB32; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;

       //     break;

       case 5:
            bufferSize = MB1; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;
        case 6:
            bufferSize = MB2; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;
        case 7:
            bufferSize = MB4; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;
        case 8:
            bufferSize = MB8; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;

        case 9:
            bufferSize = MB16; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // breakk

        case 10:
            bufferSize = MB32; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;
        case 11:
            bufferSize = MB64; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;
        case 12:
            bufferSize = MB128; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;
        case 13:
            bufferSize = MB256; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;
        case 14:
            bufferSize = MB512; printf("[BUFFER - RETURNING TO MAIN] Buffer size: %zu bytes\n", bufferSize);
            return bufferSize;
            // break;

        default:
            printf("Invalid choice. Defaulting to 4GB.\n");
            bufferSize = MB1;
            return bufferSize;
            //break;
    }

    return bufferSize;
}

bool viewFile(const char *filename, size_t bufferSize) {
    printf("[!] Starting 'VIEW FILE' for: %s\n", filename);
    FILE *file = fopen(filename, "r");

    // To view the file inputed by user
    if (file == NULL) {
        perror("[-]Failed to open file");
        return 0;
    } printf("[+] File opened. %s\n", filename);

    // Allocate a buffer of the chosen size
    char *buffer = malloc(bufferSize);
    if (buffer == NULL) {
        perror("[-]Failed to allocate buffer");
        printf("[!] Buffer size: %zu %s bytes\n", bufferSize , "\n.. returning error to main function\n");
        fclose(file);
        return 0;
    } printf("[+] Buffer successfully allocated with size: %zu bytes\n", bufferSize);

    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, bufferSize, file)) > 0) {
        // Write the chunk to the console
        fwrite(buffer, 1, bytesRead, stdout);
        printf("\n ****************************** \n[!]bytes read: %zu\n", bytesRead);

        // Optional: Prompt user to continue after each chunk
        printf("\n-- [?] Press Enter to continue --\n");
        //getchar(); // Wait for user input
    }
    if (ferror(file)) {
        perror("[-] Error reading file");
        free(buffer);
        fclose(file);
        return 0;
    }

    free(buffer);
    fclose(file);
    printf("[+] File closed.\n");
    return 1; // Success
}


// TODO: FIX THIS (SEE CONDITIONAL BELOW)
// This function searches for a keyword in a chunk of data when "search file" is invoked. the function uses strstr to find the keyword in the buffer
// and prints the position of the keyword in the chunk.
void search_in_chunk(const char *buffer, size_t buffer_size, size_t chunk_start, const char *keyword, size_t *line_number) {
    const char *pos = buffer;
    const char *line_start = buffer;
    size_t current_line = *line_number;
    FILE *result_file = fopen("search_res.txt", "w");

    if (result_file == NULL) {
        perror("[-] Error opening file to record search results");
    }

    int count = 0;
    while ((pos = strstr(pos, keyword)) != NULL) {
        size_t offset = pos - buffer; // Calculate the offset of the keyword in the chunk

        // Move line_start to the start of the line containing the found keyword
        while (line_start < pos && *line_start != '\n') {
            line_start++;
        }
        line_start++;
        // Calculate the line number
        const char *temp = buffer;
        while (temp < pos) {
            if (*temp == '\n') {
                current_line++;
            }
            temp++;
        }

        // Print and write the found line and position to the result file
        printf("[+] Keyword found at position: %zu on line: %zu\n", chunk_start + offset, current_line);
        fprintf(result_file, "Keyword found at position: %zu on line: %zu\n", chunk_start + offset, current_line);

        // Print and write the line content to the result file
        const char *line_end = pos;
        while (line_end < buffer + buffer_size && *line_end != '\n') {
            line_end++;
        }

        char line_content[line_end - line_start + 1];
        strncpy(line_content, line_start, line_end - line_start);
        line_content[line_end - line_start] = '\0';

        printf("Line content: %s\n", line_content);
        fprintf(result_file, "Line content: %s\n", line_content);

        pos += strlen(keyword);
    }

    // Update the line number for the next chunk
    while (line_start < buffer + buffer_size) {
        if (*line_start == '\n') {
            current_line++;
        }
        line_start++;
    }
    *line_number = current_line;
}

//}
//    while (((pos = strstr(pos, keyword)) != NULL) && ((pos < (buffer + buffer_size)))) {
//
//        if (pos >= buffer + buffer_size) {
//            printf("[-] Keyword not found in the chunk\n");
//            break;
//        }
//        size_t offset = pos - buffer;
//        printf("[+] Keyword found at position: %zu\n", chunk_start + offset);
//        fwrite(pos, 1, strlen(keyword), fp);
//
//        pos += strlen(keyword);
//        printf("[!] new pos  %s\n", pos);
//    }
//}

    //char *buffer = malloc(buffer_size);
    // TODO: FIX THIS
    /* If the keyword is found, print the position of the keyword in the chunk
     and the position in the file by adding the chunk start offset. */
//
//    size_t written = fwrite(buffer, 1, buffer_size, fp);
//
//    while ((pos = strstr(pos, keyword)) != NULL) {
//        size_t offset = pos - buffer;
//        printf("[!]Searching  %zu\n", chunk_start + offset);
//        printf("[!] Offset %zu\n", offset);
//
//        printf("[!] Keyword found at position: %zu\n", chunk_start + offset);
//        printf("[+] Number of elements written: %zu\n", written);
//
//        // Check if the keyword is at the start of the buffer
//        if ((pos == strstr(pos, keyword)) == 0) {
//            printf("[!] Keyword not at position: %zu\n", chunk_start + offset);
//        }
//        // Move to the next character to search for the keyword
//        else if (pos >= buffer + buffer_size) {
//            printf("[-] Keyword not found in the chunk\n");
//            break;
//        }
//    }
//}

// Function to get the current working directory
char* getCurrentWorkingDirectory() {
    char *cwd = (char *)malloc(FILENAME_MAX * sizeof(char));
    if (getcwd(cwd, FILENAME_MAX) != NULL) {
        printf("[+] Current working directory: %s\n", cwd);
        return cwd;
    } else {
        perror("[-] getcwd() error");
        free (cwd);
        return NULL;
    }
}




// Function to ask the user for the working directory, calls getCurrentWorkingDirectory() as a default
char* getWorkingDirectory() {
    char input[256];

    printf("[?] Enter the working directory (or type ['default']  or ['enter'] to use the current working directory. ");
    //scanf("%s", input);
    fgets(input, sizeof(input), stdin);

    if (fgets(input, sizeof(input), stdin) != NULL) {

        input[strcspn(input, "\n")] = 0; // Remove newline character
        printf("[+] Working directory entered: %s\n", input);


        // Check if the user wants to use the current working directory
        if (chdir(input) == 0) {
            printf("[+] Attepting  changed the working directory to: %s\n", input);
            char *directory = (char *) malloc((strlen(input) + 1) * sizeof(char));
            printf("[+] Assigning Working directory: %s\n", directory);

            // checks and prints the new working directory to confirm the change
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("[+] Current working directory: %s\n", cwd);

                strcpy(directory, input);
                return directory;

            } else {
                perror("[-] Error changing working directory, using CWD instead");
                return getCurrentWorkingDirectory();
            }
        }
    }
    return NULL;
}


void printCurrentWorkingDirectory() {
    char cwd[1024];  // Buffer to hold the current working directory
    printf("**************** \n [!] Function to print the current working directory called\n");
    // Get the current working directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("[+]..Current working directory: %s%s\n", getcwd(cwd, sizeof(cwd)), "\n ****************");

    } else {
        perror("[-] getcwd() error returning NULL  \n.. exiting program\n ****************************");
    }

}

// Function to check if a file exists in the current working directory
bool fileExists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        // File exists
       // fclose(file);
       printf("[+] File exists: %s\n", filename);
        return true;
    } else {
        // File does not exist
        printf("[-] File does not exist: %s\n", filename);
        return false;
    }
}

int is_usable_directory(const char *path) {
    struct stat info;

    printf("[!] ******************* \n Checking if the directory exists and is usable\n");
    // Check if the path exists and is a directory
    if (stat(path, &info) != 0) {
        perror("[-] stat: path does not exist");
        return 0; // Path does not exist
    }
    if (!(info.st_mode & S_IFDIR)) {
        printf("[-] The path is not a directory.\n");
        return 0; // Not a directory
    }

    // Check read and write permissions
    if (access(path, R_OK | W_OK) != 0) {
        perror("[-] access: no read or write permission for the directory");
        return 0; // No read or write permission
    }

    printf("[+] Directory exists and is usable usable\n.. returning to main function\n *******************");
    return 1; // Directory exists and is usable
}

// Function to list all files in the current directory
void listFilesInDirectory() {
    DIR *d;
    // Pointer for directory entry
    struct dirent *dir;

    // Open the current directory
    d = opendir(".");
    if (d) {
        printf("[+] Files in the current directory:\n");
        // Read and print each directory entry
        while ((dir = readdir(d)) != NULL) {
            // Skip the special entries "." and ".."
            if (dir->d_name[0] != '.') {
                printf("%s\n", dir->d_name);
            }
        }
        // Close the directory
        closedir(d);
    } else {
        perror("opendir() error");
    }
}
