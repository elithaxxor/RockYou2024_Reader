#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 1024 * 1024 // 1 MB
#define MAX_KEYWORD_LENGTH 256
#define MAX_FILENAME_LENGTH 1024

void search_in_chunk(const char *buffer, size_t buffer_size, size_t chunk_start, const char *keyword) {
    const char *pos = buffer;
    while ((pos = strstr(pos, keyword)) != NULL) {
        size_t offset = pos - buffer;
        printf("Keyword found at position: %zu\n", chunk_start + offset);
        pos += strlen(keyword);
    }
}

int main() {
    char keyword[MAX_KEYWORD_LENGTH];
    char filename[MAX_FILENAME_LENGTH];

    // Get the keyword from the user
    printf("Enter the keyword to search: ");
    if (fgets(keyword, MAX_KEYWORD_LENGTH, stdin) == NULL) {
        perror("Error reading keyword");
        return EXIT_FAILURE;
    }
    // Remove newline character if present
    size_t keyword_length = strlen(keyword);
    if (keyword[keyword_length - 1] == '\n') {
        keyword[keyword_length - 1] = '\0';
        keyword_length--;
    }

    // Get the filename from the user
    printf("Enter the filename to search in: ");
    if (fgets(filename, MAX_FILENAME_LENGTH, stdin) == NULL) {
        perror("Error reading filename");
        return EXIT_FAILURE;
    }
    // Remove newline character if present
    size_t filename_length = strlen(filename);
    if (filename[filename_length - 1] == '\n') {
        filename[filename_length - 1] = '\0';
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    char *buffer = (char *)malloc(CHUNK_SIZE + keyword_length - 1);
    if (buffer == NULL) {
        perror("Error allocating buffer");
        fclose(file);
        return EXIT_FAILURE;
    }

    size_t chunk_start = 0;
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, file)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the buffer
        search_in_chunk(buffer, bytes_read, chunk_start, keyword);
        chunk_start += bytes_read;

        // Move the buffer's start to the position after the last overlap
        fseek(file, -(long)(keyword_length - 1), SEEK_CUR);
        chunk_start -= keyword_length - 1;
    }

    free(buffer);
    fclose(file);

    return EXIT_SUCCESS;
}

