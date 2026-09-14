#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "Format_Outputs.h"
#include "Cache.h"
#include "../Get_and_Run_Info/Get/RequiredFontPacks.h"
#include "../Get_and_Run_Info/Run/Run_Process.h"
#include "../ParseTXT/ParseCSVFiles.h"
#include "../Variables/EpisodeStruct.h"

const char* SwitchNames[] = {"Video", "Audio", "Subtitle"};

extern SpaceSaved* space_saved;

void free_Users_Input(struct User_Inputs* Users, const unsigned int* user_size) {
    for (int i = 0; i < *user_size; i++) {
        if (Users[i].VidInput.data)
            free(Users[i].VidInput.data), Users[i].VidInput.data = NULL, Users[i].VidInput.size = 0;
        if (Users[i].AudioInput.data)
            free(Users[i].AudioInput.data), Users[i].AudioInput.data = NULL, Users[i].AudioInput.size = 0;
        if (Users[i].SubtitleInput.data)
            free(Users[i].SubtitleInput.data), Users[i].SubtitleInput.data = NULL, Users[i].SubtitleInput.size = 0;
    }
    free(Users);
}

bool is_valid_track_list(const char* str, const unsigned int* max_val, const char* name, unsigned int* value, size_t* len) {
    if (str == NULL || *str == '\0') return false;
    const char* p = str;
    while (*p != '\0') {
        // If it's not a digit (letters or consecutive commas like "1,,2" or "1,a,2")
        if (!isdigit(*p)) {
            printf("This is not a digit -> %c in %s variable\n", *p, name);
            return false;
        }
        int num = 0;
        while (isdigit(*p))
            num = num * 10 + (*p - '0'), p++;
        // Track numbers must be > 0 and <= max_val
        if (num <= 0 || num > *max_val) {
            printf("This value is not in the range! -> %c / %d in %s variable\nTry again!\n", *(p - 1), num, name);
            return false;
        }
        value[(*len)++] = num;
        if (*p == ',') {
            p++;
            if (*p == '\0') {
                printf("Invalid entry in %s variable of -> %c\nTry again\n", name, *(p - 1));
                return false;
            }
        }
        // If it's anything else besides a comma or the end of the string
        else if (*p != '\0') {
            printf("Invalid entry in %s variable of -> %c\nTry again\n", name, *p);
            return false;
        }
    }
    return true;
}

// IMPORTANT 'm' tells scanf to malloc exactly what is needed, and the address the pointer must be used.
static bool process_track_input(const char *type_label, const unsigned int* max_amount, char **str_out,
    unsigned int **arr_out, size_t* len) {
    if (scanf("%ms", str_out) != 1) {
        fprintf(stderr, "Error reading input for %s track.\n", type_label);
        exit(1);
    }
    const size_t capacity = (strlen(*str_out) + 1) / 2;
    *arr_out = calloc(capacity > 0 ? capacity : 1, sizeof(unsigned int));
    if (!*arr_out) {
        fprintf(stderr, "Fatal error: Allocation failed for %s tracks!\n", type_label);
        exit(1);
    }
    return is_valid_track_list(*str_out, max_amount, type_label, *arr_out, len);
}

void get_input_function(const char* label, char* input, size_t* len, const bool get_input, bool valid,
    const unsigned int* amount, unsigned int** int_version) {
    if (get_input)
        do {
            if (*int_version) free(*int_version), *int_version = NULL;
            free(input), input = NULL, *len = 0;
            printf("Enter the number for the %s tracks you want to keep: ", label);
            valid = process_track_input(label, amount, &input, int_version, len);
        } while (!valid);
}

struct User_Inputs user_input(const unsigned int* VidAmt, const unsigned int* AudAmt, const unsigned int* SubtitleAmt,
    const bool get_vid, const bool get_audio, const bool get_subtitle, const unsigned int* size) {
    char* VidInput = NULL, *AudioInput = NULL, *SubtitleInput = NULL;
    unsigned int* IntVid = NULL, *IntAud = NULL, *IntSubtitle = NULL;
    size_t VidLen = 0, AudLen = 0, SubtitleLen = 0;
    get_input_function(SwitchNames[0], VidInput, &VidLen, get_vid, true, VidAmt, &IntVid);
    get_input_function(SwitchNames[1], AudioInput, &AudLen, get_audio, true, AudAmt, &IntAud);
    get_input_function(SwitchNames[2], SubtitleInput, &SubtitleLen, get_subtitle, true, SubtitleAmt, &IntSubtitle);
    free(VidInput);
    free(AudioInput);
    free(SubtitleInput);
    return (struct User_Inputs) {
        {IntVid, VidLen},
        {IntAud,AudLen},
        {IntSubtitle,SubtitleLen},
        (*size)
    };
}

bool print_loop_medium(const char* name, const unsigned int* size, const struct Medium* medium) {
    for (int j = 0; j < *size; j++)
        printf("%s #%d: [%s] Title: %s, Lang: %s\n", name, j + 1, name, medium[j].TitleName,
            medium[j].short_name_for_Medium);
    printf("--------\n");
    return true;
}

bool print_loop_subtitle(const char* name, const unsigned int* size, const Subtitle* subtitle) {
    for (int j = 0; j < *size; j++)
        printf("%s #%d: [%s] Title: %s, Lang: %s\n", name, j + 1, name, subtitle[j].Type, subtitle[j].lang);
    printf("--------\n");
    return true;
}

void no_Cache_found(const Episode* Names, unsigned int* user_size, const int* i, struct User_Inputs* Users) {
    printf("Show #%d -> %s\n", *i + 1, Names[*i].ShowName);
    bool get_vid_input = false, get_audio_input = false, get_subtitle_input = false;
    if (Names[*i].VidAmount > 1)
        get_vid_input = print_loop_medium(SwitchNames[0], &Names[*i].VidAmount, Names[*i].video);
    if (Names[*i].AudiosAmount > 0)
        get_audio_input = print_loop_medium(SwitchNames[1], &Names[*i].AudiosAmount, Names[*i].audio);
    if (Names[*i].SubtitleAmount > 0)
        get_subtitle_input = print_loop_subtitle(SwitchNames[2], &Names[*i].SubtitleAmount, Names[*i].subtitle);
    (*user_size)++;
    Users[*i] = user_input(&Names[*i].VidAmount, &Names[*i].AudiosAmount,
    &Names[*i].SubtitleAmount, get_vid_input, get_audio_input, get_subtitle_input, user_size);
}

void while_loop_fnc(const SpaceSaved* larger, SpaceSaved* smaller) {
    while (smaller->type != larger->type)
        smaller->amount /= 1024, smaller->type++;
}

void sum_amount_saved(SpaceSaved** saved_space, Paket** paket) {
    while_loop_fnc((*paket)->originalSize, (*paket)->finalSize);
    SpaceSaved temp = {(*paket)->originalSize->amount - (*paket)->finalSize->amount, (*paket)->finalSize->type};
    temp.type > (*saved_space)->type ? while_loop_fnc(&temp, *saved_space) : while_loop_fnc(*saved_space, &temp);
    (*saved_space)->amount += temp.amount;
}

void format_print_Ep_info(Episode* Names, const size_t* names_size) {
    struct User_Inputs* Users = malloc(*names_size * sizeof(struct User_Inputs));
    unsigned int user_size = 0;
    if (Users == NULL) return;
    int i = 0;
    struct Cache* cache = NULL;
    SpaceSaved* original_amount = calloc(1, sizeof(SpaceSaved));
    memcpy(original_amount, space_saved, sizeof(SpaceSaved));
    do {
        if (!cache || !match_cache(&cache, &Names[i], Users, &user_size)) {
            no_Cache_found(Names, &user_size, &i, Users);
            cache = add_cache(cache, &Names[i], &Users[user_size - 1]);
            if (cache == NULL) break;
        }
        else
            printf("%s\n", Names[i].ShowName);
        const int exit_code = MKV_merge_process(&Names[i], &Users[i]);
        if (exit_code != 0)
            printf("\nExit code: %d\n", exit_code);
        const size_t len = strlen("Edited - ") + strlen(Names[i].ShowName) + 1;
        char* edited_name = malloc(len);
        if (!edited_name) {
            fprintf(stderr, "Memory allocation failed for edited_name\n");
            exit(1);
        }
        snprintf(edited_name, len, "Edited - %s", Names[i].ShowName);
        Names[i].paket->finalSize = create_SpaceSaved_struct(short_call(edited_name));
        free(edited_name), edited_name = NULL;
        sum_amount_saved(&space_saved, &Names[i].paket);
        i++;
    } while (i < *names_size);
    while_loop_fnc(space_saved, original_amount);
    write_to_txt(space_saved, "../CSV texts/Amount Saved so far.txt", original_amount);
    free_Users_Input(Users, &user_size), free_Cache(cache);
}