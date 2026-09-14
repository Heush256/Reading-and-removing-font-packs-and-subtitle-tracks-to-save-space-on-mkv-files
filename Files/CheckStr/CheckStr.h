#ifndef MKV_READER_CHECKSTR_H
#define MKV_READER_CHECKSTR_H

#include <stddef.h>
#include "../Variables/EpisodeStruct.h"

unsigned int convert(const unsigned int* pos_in_file);

void compare_strs(const char* name, Subtitle* const* subtitle, const unsigned int* subtitleSize, const unsigned int* pos,
    Episode* ep, size_t* count);

#endif //MKV_READER_CHECKSTR_H