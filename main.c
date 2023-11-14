#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_TODOS 10
#define MAX_SIZE_OF_CONTENT 128
#define DATA_FILE_NAME "data"
#define TEMP_FILE_NAME "temp"

void edit_todo(char* line_to_edit, char* new_content);
void delete_todo(char* line_to_delete);
void list_todos();
void create_todo(char* content);
void purge_todos();
void help();

int main(int argc, char* args[]){
    if (argc == 3 && !strcmp(args[1], "-n")) {
        create_todo(args[2]);
        return 1;
    }

    if (argc == 2 && !strcmp(args[1], "-ls")) {
        list_todos();
        return 1;
    }

    if (argc == 3 && !strcmp(args[1], "-d")) {
        delete_todo(args[2]);
        return 1;
    }

    if (argc == 4 && !strcmp(args[1], "-e")) {
        edit_todo(args[2], args[3]);
        return 1;
    }

    if (argc == 2 && !strcmp(args[1], "-p")) {
        purge_todos();
        return 1;
    }

    help();
    return 0;
}

void edit_todo(char* line_to_edit, char* new_content) {
    const int parset_line_to_edit = atoi(line_to_edit);
    FILE *file, *temp;
    char buffer[MAX_SIZE_OF_CONTENT];
    int line=0;

    file = fopen(DATA_FILE_NAME, "r");
    if (file == NULL) {
        printf("error while opening %s\n", DATA_FILE_NAME);
        return;
    }

    temp = fopen(TEMP_FILE_NAME, "w");
    if (file == NULL) {
        printf("error while opening %s\n", TEMP_FILE_NAME);
        return;
    }

    while(fgets(buffer, MAX_SIZE_OF_CONTENT, file)) {
        line++;
        if (line == parset_line_to_edit) {
            char* cwnl = malloc(MAX_SIZE_OF_CONTENT*sizeof(MAX_SIZE_OF_CONTENT));
            sprintf(cwnl, "%s\n", new_content); 
            fputs(cwnl, temp);
            free(cwnl);
            continue;
        }
        fputs(buffer, temp);
    }

    fclose(file);
    fclose(temp);

    remove(DATA_FILE_NAME);
    rename(TEMP_FILE_NAME, DATA_FILE_NAME);
}

void delete_todo(char* line_to_delete) {
    const int parsed_line_to_delete = atoi(line_to_delete);

    FILE *file, *temp;
    int line=0;
    char buffer[MAX_SIZE_OF_CONTENT];
    file = fopen(DATA_FILE_NAME, "r");
    if (file == NULL) {
        printf("error while opening %s\n", DATA_FILE_NAME);
        return;
    }

    temp = fopen(TEMP_FILE_NAME, "w");
    if (file == NULL) {
        printf("error while opening %s\n", TEMP_FILE_NAME);
        return;
    }

    while(fgets(buffer, MAX_SIZE_OF_CONTENT, file)) {
        line++;
        if(line != parsed_line_to_delete) {
            fputs(buffer, temp);
        }
    }

    fclose(temp);
    fclose(file);

    remove(DATA_FILE_NAME);
    rename(TEMP_FILE_NAME, DATA_FILE_NAME);
}

void list_todos() {
    FILE *file;
    file = fopen(DATA_FILE_NAME, "r");
    if (file == NULL) {
        printf("error while opening %s\n", DATA_FILE_NAME);
        return;
    }
    char buffer[MAX_SIZE_OF_CONTENT];
    int line = 0;
    while(fgets(buffer, MAX_SIZE_OF_CONTENT, file)) {
        line++;
        printf("%i | %s", line, buffer);
    }

    fclose(file);
}

void create_todo(char* content) {
    FILE *file;
    file = fopen(DATA_FILE_NAME, "a");
    fprintf(file, "%s\n", content);
    fclose(file);
}

void purge_todos() {
    char input;
    printf("This operation will remove data\n");

    printf("Are you shure you want to continue? [y/n]\n");
    scanf("%c", &input);

    if (input == 'y') {
        FILE *file;
        file = fopen(DATA_FILE_NAME, "r");
        if (file == NULL) {
            printf("error while opening %s\n", DATA_FILE_NAME);
            return;
        }

        fclose(file);
        remove(DATA_FILE_NAME);
    }

    if (input == 'n') {
        exit(1);
    }
}

void help() {
    printf("Commands:\n");
    printf("-n [content]                        creates a new todo\n");
    printf("-ls                                 list todos\n");
    printf("-d [number of line]                 deletes todo by line\n");
    printf("-e [number of line] [new content]   edits todo by line\n");
    printf("-p                                  purge all todos\n");
}