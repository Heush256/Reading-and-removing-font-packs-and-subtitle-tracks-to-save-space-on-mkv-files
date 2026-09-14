#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ParseCSVFiles.h"
#include "../Variables/EpisodeStruct.h"

void append_File_Contents(FileNames** contents, const char* beginning) {
    const unsigned int current_count = *contents == NULL ? 0 : (*contents)[0].count;
    FileNames *temp = realloc(*contents, (current_count + 1) * sizeof(FileNames));
    if (!temp) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
    *contents = temp;
    (*contents)[current_count].File_Name = strdup(beginning);
    (*contents)[0].count = current_count + 1;
}

bool check_if_name_already_is_in_struct(FileNames* unique_name, const char* second_half) {
    bool flag = false;
    if (!unique_name) return flag;
    for (int i = 0; i < unique_name->count; i++)
        if ((flag = strcmp(unique_name[i].File_Name, second_half) == 0)) break;
    return flag;
}

FileNames* readCSVfile(const char* file_path) {
    FILE* f = fopen(file_path, "r");
    if (!f) {
        printf("Creating an empty file!");
        f = fopen(file_path, "w");
        if (!f) {
            perror("Could not create file");
            return NULL;
        }
    }
    FileNames* array_to_return = NULL;
    int current_file_value, amount_read = 1;
    while ((current_file_value = fgetc(f)) != EOF) {
        if (current_file_value == '\n') {
            char* temp_file_name_str = malloc(amount_read + 1);
            if (!temp_file_name_str) {
                fprintf(stderr, "UNABLE OT ALLOC FOR THE FILE!");
                exit(1);
            }
            fseek(f, -(amount_read + 1), SEEK_CUR);
            fread(temp_file_name_str, 1, amount_read - 1, f);
            temp_file_name_str[amount_read - 1] = '\0';
            fgetc(f);
            if (!check_if_name_already_is_in_struct(array_to_return, temp_file_name_str))
                append_File_Contents(&array_to_return, temp_file_name_str);
            free(temp_file_name_str);
            amount_read = 1;
        }
        else
            amount_read++;
    }
    if (amount_read > 1) {
        char* temp_file_name_str = malloc(amount_read + 3);
        if (!temp_file_name_str) {
            fprintf(stderr, "UNABLE OT ALLOC FOR THE FILE!");
            exit(1);
        }
        if (fseek(f, -(amount_read - 1), SEEK_CUR) != 0) {
            perror("Seek failed!");
            free(temp_file_name_str);
            fclose(f);
            return NULL;
        }
        fread(temp_file_name_str, 1, amount_read, f);
        temp_file_name_str[amount_read - 1] = '\0';
        // printf("LAST!! -> %s -- %s\n", beginning, second_half);
        if (!check_if_name_already_is_in_struct(array_to_return, temp_file_name_str))
            append_File_Contents(&array_to_return, temp_file_name_str);
        free(temp_file_name_str);
    }
    fclose(f);
    return array_to_return;
}

enum Type call_switch(const int value) {
    switch (value) {
        case 0:
            return B;
        case 1:
            return KB;
        case 2:
            return MB;
        case 3:
            return GB;
        case 4:
            return TB;
        case 5:
            return PB;
        case 6:
            return EB;
        case 7:
            return ZB;
        case 8:
            return YB;
        default:
            printf("Invalid value -> %c", value);
            return B;
    }
}

SpaceSaved* readTXTfile(const char* file_path) {
    FILE* f = fopen(file_path, "r");
    if (!f) {
        printf("Creating an empty file!");
        f = fopen(file_path, "w");
        if (!f) {
            perror("Could not create file");
            return NULL;
        }
    }
    SpaceSaved* space_saved = calloc(1, sizeof(SpaceSaved));
    int current_file_value, amount_read = 1;
    char* temp = NULL, *end_ptr = NULL;
    while ((current_file_value = fgetc(f)) != EOF) {
        if (amount_read == 1)
            space_saved->type = call_switch(current_file_value - '0');
        if (amount_read > 2) {
            char* temp_temp = realloc(temp, (amount_read - 1) * sizeof(char));
            if (!temp_temp) {
                fprintf(stdin, "Unable to alloc for savespace!");
                exit(1);
            }
            temp = temp_temp;
            temp_temp[amount_read - 3] = (char) current_file_value;
            temp_temp[amount_read - 2] = '\0';
            amount_read++;
        }
        else
            amount_read++;
    }
    fclose(f);
    if (amount_read > 1) {
        if (temp != NULL) space_saved->amount = strtof(temp, &end_ptr);
        free(temp);
    }
    return space_saved;
}

void write_to_txt(const SpaceSaved* space_saved, const char* file_path, const SpaceSaved* original_amount) {
    FILE* f = fopen(file_path, "w");
    printf("\n-> %f %d space was saved!!\n", space_saved->amount - original_amount->amount, space_saved->type);
    fprintf(f, "%d %f", call_switch(space_saved->type), space_saved->amount);
    fclose(f);
}
