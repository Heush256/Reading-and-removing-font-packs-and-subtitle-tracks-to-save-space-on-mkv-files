#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../InputDealing/Format_Outputs.h"
#include "../Get/RequiredFontPacks.h"

bool exit_fnc(const char* info, const char* mes) {
    if (!info) {
        fprintf(stderr, "Memory allocation failed");
        printf(" %s\n", mes);
        return false;
    }
    return true;
}

char* fix_str(const size_t *temp_len, const unsigned int subLength, const unsigned int* temp_mem, const Subtitle* sub) {
    char* temp = calloc(*temp_len + 1, sizeof(char));
    if (!temp) return NULL;
    unsigned int offset = 0;
    for (unsigned int i = 0; i < subLength; i++) {
        if (!sub[temp_mem[i]].attachment) continue;
        const unsigned int local_attachments = sub[temp_mem[i]].AttachmentAmount;
        for (unsigned int j = 0; j < local_attachments; j++) {
            if (sub[temp_mem[i]].attachment[j].is_default || sub[temp_mem[i]].attachment[j].value == 0) continue;
            const unsigned int val = sub[temp_mem[i]].attachment[j].value;
            const int added = snprintf(temp + offset, *temp_len + 1 - offset,
                                       "%s%d",
                                       offset > 0 ? "," : "",
                                       val);
            if (added < 0 || offset + added > *temp_len) {
                printf("Error: String buffer is too small #2!\n");
                return temp;
            }
            offset += added;
        }
    }
    return temp;
}

void parse_str(unsigned int* temp_mem, const unsigned int* length_subtitle, unsigned int* extra,
    unsigned int* sum, const Subtitle* subtitle) {
    for (int i = 0; i < *length_subtitle; i++){
        temp_mem[i] -= 1;
        const unsigned int parsed_value = temp_mem[i], local = subtitle[parsed_value].AttachmentAmount;
        *sum += local;
        unsigned int temp_sum = 0;
        for (int j = 0; j < local; j++)
            if (!subtitle[parsed_value].attachment[j].is_default && subtitle[parsed_value].attachment[j].value != 0)
                temp_sum += subtitle[parsed_value].attachment[j].allocateAmount;
        *extra += temp_sum - local;
    }
}

const unsigned int* correct_pos(const unsigned int* pos, const size_t* size, const unsigned int* offset) {
    unsigned int* new_pos = malloc(*size * sizeof(unsigned int));
    if (!new_pos) return NULL;
    for (int i = 0; i < *size; i++)
        new_pos[i] = pos[i] + *offset - 1;
    return new_pos;
}

char* generic_str(const size_t* temp_len, const unsigned int* align_pos, const size_t* loop_amount) {
    char* valid_str = calloc(*temp_len + 1, sizeof(char));
    if (!exit_fnc(valid_str, "for audio tracks -> valid_str")) return NULL;
    unsigned int offset = 0, partial_loop = 0;
    for (int j = 0; j < *loop_amount; j++) {
        const int added = snprintf(valid_str + offset, *temp_len - offset, "%s%d",
                                   offset > 0 ? "," : "", align_pos[partial_loop++]);
        if (added < 0 || offset + added > *temp_len) {
            printf("Error: String buffer is too small #1!\n");
            break;
        }
        offset += added;
    }
    valid_str[*temp_len] = '\0';
    return valid_str;
}

char* create_str(const struct Input input, const unsigned int* pos_offset) {
    const unsigned int* align_pos2 = correct_pos(input.data, &input.size, pos_offset);
    if (!align_pos2) return NULL;
    unsigned int* str_size_for_each_pos2 = calloc(input.size, sizeof(unsigned int)), sum_Temp = 0;
    if (!str_size_for_each_pos2) {
        fprintf(stderr, "THERE WAS A PROBLEM IN ALLOCATING");
        free((void*) align_pos2);
        return NULL;
    }
    for (unsigned int increment = 0; increment < input.size; increment++) {
        unsigned int temp = align_pos2[increment];
        str_size_for_each_pos2[increment] = 1;
        while (temp > 9) {
            temp /= 10;
            str_size_for_each_pos2[increment]++;
        }
        sum_Temp += str_size_for_each_pos2[increment];
    }
    const size_t temp_len = 2 * input.size + (sum_Temp - input.size);
    char* str_valid_user_audio = generic_str(&temp_len, align_pos2, &input.size);
    if (!exit_fnc(str_valid_user_audio, "for audio tracks -> str_valid_user_audio")) {
        free((void*) align_pos2);
        free(str_size_for_each_pos2);
        return NULL;
    }
    free((void*) align_pos2);
    free(str_size_for_each_pos2);
    return str_valid_user_audio;
}

int MKV_merge_process(const Episode* Names, const struct User_Inputs* User) {
    unsigned int capacity = 16;
    char** MKV_merge_str = calloc(capacity, sizeof(char*));
    int i = 0;
    if (!MKV_merge_str) return -1;
    MKV_merge_str[i++] = strdup("mkvmerge");
    MKV_merge_str[i++] = strdup("-o");
    const size_t len = strlen("Edited - ") + strlen(Names->ShowName) + 3;
    char* edited_name = malloc(len);
    if (!edited_name) {
        fprintf(stderr, "Memory allocation failed for edited_name\n");
        return -1;
    }
    snprintf(edited_name, len, "\"Edited - %s\"", Names->ShowName);
    MKV_merge_str[i++] = edited_name;
    MKV_merge_str[i++] = strdup("--no-date");
    MKV_merge_str[i++] = strdup("--no-track-tags");
    MKV_merge_str[i++] = strdup("--no-global-tags");
    MKV_merge_str[i++] = strdup("--disable-track-statistics-tags");
    MKV_merge_str[i++] = strdup("--no-buttons");
    MKV_merge_str[i++] = strdup("--title");
    MKV_merge_str[i++] = strdup("\"\"");
    MKV_merge_str[i++] = strdup("--engage");
    MKV_merge_str[i++] = strdup("no_cue_duration");
    MKV_merge_str[i++] = strdup("--engage");
    MKV_merge_str[i++] = strdup("no_cue_relative_position");
    MKV_merge_str[i++] = strdup("--cues");
    MKV_merge_str[i++] = strdup("0:iframes");
    const unsigned int* align_pos = NULL;
    if (User->AudioInput.size > 0) {
        align_pos = correct_pos(User->AudioInput.data, &User->AudioInput.size, &Names->VidAmount);
        unsigned int* str_size_for_each_pos = calloc(User->AudioInput.size, sizeof(unsigned int));
        for (unsigned int increment = 0; increment < User->AudioInput.size; increment++) {
            unsigned int temp = align_pos[increment];
            str_size_for_each_pos[increment] = 1;
            while (temp > 9) {
                temp /= 10;
                str_size_for_each_pos[increment]++;
            }
        }
        for (int j = 0; j < User->AudioInput.size; j++) {
            if (i + 2 >= capacity) {
                capacity += 2;
                char** temp2d = realloc(MKV_merge_str, capacity * sizeof(char*));
                if (!temp2d) {
                    fprintf(stderr, "Memory allocation failed for MKV_merge_str\n");
                    break;
                }
                MKV_merge_str = temp2d;
            }
            MKV_merge_str[i++] = strdup("--cues");
            const size_t loop_len = strlen(":none") + str_size_for_each_pos[j] + 1;
            char* temp1d = malloc(loop_len);
            if (!exit_fnc(temp1d, " for track cue argument")) return -2;
            snprintf(temp1d, loop_len, "%d:none", align_pos[j]);
            MKV_merge_str[i++] = temp1d;
        }
        free(str_size_for_each_pos);
    }
    if (i + 8 >= capacity) {
        capacity += 8;
        char** temp_str = realloc(MKV_merge_str, capacity * sizeof(char*));
        if (!temp_str) {
            fprintf(stderr, "Failed to reallocate MKV_merge_str\n");
            free(MKV_merge_str);
            return -2;
        }
        MKV_merge_str = temp_str;
    }
    if (User->AudioInput.size > 0) {
        MKV_merge_str[i++] = strdup("--audio-tracks");
        char* str_valid_user_audio = create_str(User->AudioInput, &Names->VidAmount);
        if (!exit_fnc(str_valid_user_audio, "for audio tracks -> str_valid_user_audio")) return -2;
        MKV_merge_str[i++] = str_valid_user_audio;
    }
    const unsigned int subLength = User->SubtitleInput.size;
    if (subLength > 0) {
        MKV_merge_str[i++] = strdup("--subtitle-tracks");
        const unsigned int temp_offset = Names->VidAmount + Names->AudiosAmount;
        char* str_valid_user_subtitle = create_str(User->SubtitleInput, &temp_offset);
        if (!exit_fnc(str_valid_user_subtitle, "for audio tracks -> str_valid_user_audio")) return -2;

        MKV_merge_str[i++] = str_valid_user_subtitle;



        unsigned int extra = 0, sum = 0, *temp_mem = calloc(subLength, sizeof(unsigned int));
        if (!temp_mem) {
            printf("Unable to set up!");
            free(temp_mem);
            return -4;
        }
        memcpy(temp_mem, User->SubtitleInput.data, subLength * sizeof(unsigned int));
        parse_str(temp_mem, &subLength, &extra, &sum, Names->subtitle);
        if (sum > 0) {
            const size_t temp_len_last = 2 * sum + extra;
            char* temp = fix_str(&temp_len_last, subLength, temp_mem, Names->subtitle);
            if (!exit_fnc(temp, "for audio tracks -> str_valid_user_audio")) return -2;
            if (strlen(temp) > 0) {
                MKV_merge_str[i++] = strdup("--attachments");
                MKV_merge_str[i++] = temp;
            }
            else
                MKV_merge_str[i++] = strdup("--no-attachments");
        }
        else
            MKV_merge_str[i++] = strdup("--no-attachments");
        free(temp_mem);
    }
    else {
        MKV_merge_str[i++] = strdup("--no-subtitles");
        MKV_merge_str[i++] = strdup("--no-attachments");
    }





    const size_t original_show_name_len = strlen(Names->ShowName) + 3;
    char* original_show_name = malloc(original_show_name_len);
    if (!exit_fnc(original_show_name, "for edited_name")) {
        if (align_pos) free((void*) align_pos);
        return -5;
    }
    snprintf(original_show_name, original_show_name_len, "\"%s\"", Names->ShowName);
    MKV_merge_str[i++] = original_show_name;
    for (int j = 0; j < i; j++)
        printf("%s ", MKV_merge_str[j]);
    MKV_merge_str[i] = NULL;
    //For testing
    /*char character;
    printf("\nAre you happy with that command? (y to execute)\n");
    scanf(" %c", &character);
    int exit_code;
    if (character == 'y' || character == 'Y')
        exit_code = (int)_spawnvp(_P_WAIT, MKV_merge_str[0], (const char *const *) MKV_merge_str);
    else
        exit_code = 0;*/
    const int exit_code = (int) _spawnvp(_P_WAIT, MKV_merge_str[0], (const char *const *) MKV_merge_str);
    if (align_pos) free((void*) align_pos);
    for (int j = 0; j < i; j++) {
        free(MKV_merge_str[j]);
    }
    free(MKV_merge_str);
    return exit_code;
}
