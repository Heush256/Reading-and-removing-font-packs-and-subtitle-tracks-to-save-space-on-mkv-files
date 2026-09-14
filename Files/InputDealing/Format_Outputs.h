#ifndef MKV_READER_FORMAT_OUTPUTS_H
#define MKV_READER_FORMAT_OUTPUTS_H

#include "../Get_and_Run_Info/Get/RequiredFontPacks.h"

struct User_Inputs {
    struct Input{
        unsigned int* data;
        size_t size;
    } VidInput, AudioInput, SubtitleInput;
    unsigned int user_size;
};

void format_print_Ep_info(Episode* Names, const size_t* names_size);

bool print_loop_medium(const char* name, const unsigned int* size, const struct Medium* medium);

bool print_loop_subtitle(const char* name, const unsigned int* size, const Subtitle* subtitle);

void get_input_function(const char* label, char* input, size_t* len, bool get_input, bool valid,
    const unsigned int* amount, unsigned int** int_version);

#endif //MKV_READER_FORMAT_OUTPUTS_H