#ifndef MKV_READER_FREE_FILE_H
#define MKV_READER_FREE_FILE_H

#include "../Get_and_Run_Info/Get/RequiredFontPacks.h"
#include "../../Files/ParseTXT/ParseCSVFiles.h"

void free_Episode_struct(const Episode* Names, const size_t* count1);

void free_combined_array(const FileNames* combined);

#endif //MKV_READER_FREE_FILE_H