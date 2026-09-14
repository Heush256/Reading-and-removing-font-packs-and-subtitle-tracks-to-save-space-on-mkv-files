#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Format_Outputs.h"
#include "../Get_and_Run_Info/Get/RequiredFontPacks.h"

struct Cache {
    struct Medium** vid, **audio;
    Subtitle** subtitle;
    unsigned int VidSize, AudioSize, SubtitleSize;
};

extern const char* SwitchNames[];

struct Medium** medium(struct Medium** cache, unsigned int* medium_size, const struct Input* user,
    struct Medium* names, const char* TypeName) {
    struct Medium** vid = realloc(cache, (*medium_size + user->size) * sizeof(struct Medium*));
    if (!vid) {
        fprintf(stderr, "Fatal error: Inner pointer array allocation failed for %s!\n", TypeName);
        exit(1);
    }
    cache = vid;
    for (int i = 0; i < user->size; i++)
        if (strcmp(names[user->data[i] - 1].TitleName, "Unknown") != 0)
            cache[(*medium_size)++] = &names[user->data[i] - 1];
    return *medium_size == 0 ? NULL : vid;
}

Subtitle** subtitle(Subtitle** cache, unsigned int* subtitle_size, const struct Input* user, Subtitle* subtitle_names,
    const char* name) {
    Subtitle** subtitle = realloc(cache, (*subtitle_size + user->size) * sizeof(Subtitle*));
    if (!subtitle) {
        fprintf(stderr, "Fatal error: Inner pointer array allocation failed for %s!\n", name);
        exit(1);
    }
    cache = subtitle;
    for (int i = 0; i < user->size; i++)
        if (strcmp(subtitle_names[user->data[i] - 1].Type, "Untitled") != 0 &&
            strcmp(subtitle_names[user->data[i] - 1].lang, "unknown") != 0)
            cache[(*subtitle_size)++] = &subtitle_names[user->data[i] - 1];
    return *subtitle_size == 0 ? NULL: subtitle;
}

struct Cache* add_cache(struct Cache* cache, const Episode* Names, const struct User_Inputs* User) {
    if (!cache) {
        struct Cache* temp = calloc(1, sizeof(struct Cache));
        if (!temp) {
            perror("Unable to create cache!");
            return NULL;
        }
        cache = temp;
    }
    if (User->VidInput.data)
        cache->vid = medium(cache->vid, &cache->VidSize, &User->VidInput, Names->video, SwitchNames[0]);
    if (User->AudioInput.data)
        cache->audio = medium(cache->audio, &cache->AudioSize, &User->AudioInput, Names->audio, SwitchNames[1]);
    if (User->SubtitleInput.data)
        cache->subtitle = subtitle(cache->subtitle, &cache->SubtitleSize, &User->SubtitleInput, Names->subtitle,
            SwitchNames[2]);
    return cache;
}

bool repetition_Subtitle(const char* type, unsigned int** data, size_t* size, const unsigned int* amount,
    const unsigned int* cache_size, const Subtitle* subtitle, Subtitle* const* cache) {
    *data = malloc(*amount * sizeof(unsigned int));
    if (!*data) {
        fprintf(stderr, "Fatal error: Allocation failed for %s cache mapping!\n", type);
        exit(1);
    }
    bool ran = false;
    for (int i = 0; i < *cache_size; i++)
        for (int j = 0; j < *amount; j++)
            if (strcmp(subtitle[j].lang, cache[i]->lang) == 0 || strcmp(subtitle[j].Type, cache[i]->Type) == 0) {
                (*data)[(*size)++] = j + 1;
                ran = true;
                break;
            }
    if (*size > 0) {
        unsigned int* temp = realloc(*data, *size * sizeof(unsigned int));
        if (temp != NULL)
            *data = temp;
        else {
            fprintf(stderr, "Fatal error: Reallocation failed for %s tracks!\n", type);
            free(*data);
            *data = NULL;
            exit(1);
        }
    }
    else
        free(*data), *data = NULL;
    return ran;
}

bool repetition_Medium(const char* type, unsigned int** data, size_t* user_size, const unsigned int* amount,
    const unsigned int* CacheSize, const struct Medium* medium, struct Medium* const* cache) {
    *data = malloc(*amount * sizeof(unsigned int));
    if (!*data) {
        fprintf(stderr, "Fatal error: Allocation failed for %s cache mapping!\n", type);
        exit(1);
    }
    bool ran = false;
    for (int i = 0; i < *CacheSize; i++)
        for (int j = 0; j < *amount; j++)
            if (strcmp(medium[j].TitleName, cache[i]->TitleName) == 0 ||
                strcmp(medium[j].short_name_for_Medium, cache[i]->short_name_for_Medium) == 0) {
                (*data)[(*user_size)++] = j + 1;
                ran = true;
                break;
                }
    if (*user_size > 0) {
        unsigned int* temp = realloc(*data, *user_size * sizeof(unsigned int));
        if (temp != NULL)
            *data = temp;
        else {
            fprintf(stderr, "Fatal error: Reallocation failed for %s tracks!\n", type);
            free(*data), *data = NULL;
            exit(1);
        }
    }
    else
        free(*data), *data = NULL;
    return ran;
}

bool match_cache(struct Cache** cache_ptr, const Episode* Names, struct User_Inputs* Users, unsigned int* user_size) {
    if (!cache_ptr || !*cache_ptr) return false;
    const struct Cache* cache = *cache_ptr;
    struct User_Inputs* user = &Users[*user_size];
    user->VidInput.data = NULL, user->AudioInput.data = NULL, user->SubtitleInput.data = NULL,
    user->VidInput.size = 0, user->AudioInput.size = 0, user->SubtitleInput.size = 0;
    user->user_size = *user_size;
    bool found_match[3] = {false}, nothing_set[3] = {false};
    if (cache->VidSize > 0 && Names->VidAmount > 1)
        if (!((found_match[0] = repetition_Medium(SwitchNames[0], &user->VidInput.data, &user->VidInput.size,
            &Names->VidAmount, &cache->VidSize, Names->video, cache->vid))))
            nothing_set[0] = true;

    if (cache->AudioSize > 0 && Names->AudiosAmount > 0)
        if (!((found_match[1] = repetition_Medium(SwitchNames[1], &user->AudioInput.data, &user->AudioInput.size,
            &Names->AudiosAmount, &cache->AudioSize, Names->audio, cache->audio))))
            nothing_set[1] = true;

    if (cache->SubtitleSize > 0 && Names->SubtitleAmount > 0)
        if (!((found_match[2] = repetition_Subtitle(SwitchNames[2], &user->SubtitleInput.data, &user->SubtitleInput.size,
            &Names->SubtitleAmount, &cache->SubtitleSize, Names->subtitle, cache->subtitle))))
            nothing_set[2] = true;

    if (found_match[0] || found_match[1] || found_match[2]) {
        if (!found_match[0] && Names->VidAmount > 1)
            nothing_set[0] = print_loop_medium(SwitchNames[0], &Names->VidAmount, Names->video);
        if (!found_match[1] && Names->AudiosAmount > 0)
            nothing_set[1] = print_loop_medium(SwitchNames[1], &Names->AudiosAmount, Names->audio);
        if (!found_match[2] && Names->SubtitleAmount > 0)
            nothing_set[2] = print_loop_subtitle(SwitchNames[2], &Names->SubtitleAmount, Names->subtitle);
        if (nothing_set[0]) {
            const bool valid = true;
            char* input = NULL;
            get_input_function(SwitchNames[0], input, &user->VidInput.size, true, valid, &Names->VidAmount, &user->VidInput.data);
        }
        if (nothing_set[1]) {
            const bool valid = true;
            char* input = NULL;
            get_input_function(SwitchNames[1], input, &user->AudioInput.size, true, valid, &Names->AudiosAmount, &user->AudioInput.data);
        }
        if (nothing_set[2]) {
            const bool valid = true;
            char* input = NULL;
            get_input_function(SwitchNames[2], input, &user->SubtitleInput.size, true, valid, &Names->SubtitleAmount, &user->SubtitleInput.data);
        }
        if (nothing_set[0] || nothing_set[1] || nothing_set[2])
            *cache_ptr = add_cache(*cache_ptr, Names, user);
        (*user_size)++;
        return true;
    }
    return false;
}

void free_Cache(struct Cache* cache) {
    if (cache) {
        if (cache->vid)
            free(cache->vid), cache->vid = NULL;
        cache->VidSize = 0;
        if (cache->audio)
            free(cache->audio), cache->audio = NULL;
        cache->AudioSize = 0;
        if (cache->subtitle)
            free(cache->subtitle), cache->subtitle = NULL;
        cache->SubtitleSize = 0;
        free(cache);
    }
}
