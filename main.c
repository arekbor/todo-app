#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_SIZE_OF_CONTENT 128
#define DATA_FILE_NAME "data"
#define TEMP_FILE_NAME "temp"
#define MAX_LINE_ELEMENTS 2

typedef struct {
    int line_number;
    char* content;
    int isDone;
} Todo;

void edit_todo(char* line_to_edit, char* new_content, int* new_isDone);
void delete_todo(char* line_to_delete);
void list_todos();
void create_todo(char* content);
void purge_todos();
void help();
int read_todos_from_file(Todo** todo);

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
        edit_todo(args[2], args[3], NULL);
        return 1;
    }

    if (argc == 3 && !strcmp(args[1], "-md")) {
        edit_todo(args[2], NULL, &(int){1});
        return 1;
    }

    if (argc == 2 && !strcmp(args[1], "-p")) {
        purge_todos();
        return 1;
    }

    help();
    return 0;
}

void edit_todo(char* line_to_edit, char* new_content, int* new_isDone) {
    if (line_to_edit == NULL) {
        return;
    }

    FILE *temp;
    temp = fopen(TEMP_FILE_NAME, "w");
    if (temp == NULL) {
        printf("error while opening %s\n", TEMP_FILE_NAME);
        return;
    }

    Todo* todo = NULL;
    int len = read_todos_from_file(&todo);
    size_t line_size = MAX_SIZE_OF_CONTENT*sizeof(MAX_SIZE_OF_CONTENT);

    for(int i = 0; i < len; i++) {
        if (todo[i].line_number == atoi(line_to_edit)) {
            char* new_line = malloc(line_size);

            int isDone = new_isDone == NULL ? todo[i].isDone : *new_isDone;
            char* content = new_content == NULL ? todo[i].content : new_content;
            
            sprintf(new_line, "\"%s\",%i\n", content, isDone); 
            fputs(new_line, temp);
            free(new_line);
            continue;
        }

        char* old_line = malloc(line_size);
        sprintf(old_line, "\"%s\",%i\n", todo[i].content, todo[i].isDone); 
        fputs(old_line, temp);
        free(old_line);
    }
    fclose(temp);
    remove(DATA_FILE_NAME);
    rename(TEMP_FILE_NAME, DATA_FILE_NAME);
}

void delete_todo(char* line_to_delete) {
    if (line_to_delete == NULL) {
        return;
    }

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
    Todo* todo = NULL;
    int len = read_todos_from_file(&todo);
    for(int i = 0; i < len; i++) {
        todo[i].isDone == 1 ? printf("\033[1;32m") : printf("\033[1;31m");
        printf("%d | %s\n", todo[i].line_number, todo[i].content);
        printf("\033[0m");
        free(todo[i].content);
    }
    free(todo);
}

void create_todo(char* content) {
    if (content == NULL) {
        return;
    }

    FILE *file;
    file = fopen(DATA_FILE_NAME, "a");
    int isDone = 0;
    
    fprintf(file, "\"%s\",%i\n", content, isDone);
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
    printf("-n [content]                            creates a new todo\n");
    printf("-ls                                     list todos\n");
    printf("-d [number of line]                     deletes todo by line\n");
    printf("-e [number of line] [new content]       edits todo by line\n");
    printf("-md [number of line]                    mark todo as done by line\n");
    printf("-p                                      purge all todos\n");
}

/**
 * Reads todo items from a file
 * 
 * @note The memory allocated for each content and the entire Todo array
 * must by freed.
*/
int read_todos_from_file(Todo** todo) {
    FILE * file;
    file = fopen(DATA_FILE_NAME, "r");
    if (file == NULL) {
        printf("error while opening %s\n", DATA_FILE_NAME);
        return 0;
    }

    int len=0;
    char buffer[MAX_SIZE_OF_CONTENT];
    while(fgets(buffer, MAX_SIZE_OF_CONTENT, file)) {
        len++;

        //separates entire line from string by comma
        char* p = strtok(buffer, ",");
        char* array_lines[MAX_LINE_ELEMENTS];
        for(int i = 0; i < MAX_LINE_ELEMENTS; i++) {
            array_lines[i] = p;
            p = strtok(NULL, ",");
        }

        //removes first and last char from string
        memmove(array_lines[0], array_lines[0]+1, strlen(array_lines[0]));
        array_lines[0][strlen(array_lines[0])-1] = '\0';
        
        //increases mem size and pass values to struct
        (*todo) = realloc((*todo), len * sizeof(Todo));
        (*todo)[(len - 1)].line_number = len;
        (*todo)[(len - 1)].content = strdup(array_lines[0]);
        (*todo)[(len - 1)].isDone = atoi(array_lines[1]);
    }
    fclose(file);
    return len;
}