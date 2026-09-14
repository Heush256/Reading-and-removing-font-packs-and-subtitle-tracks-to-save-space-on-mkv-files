#ifndef MKV_READER_PRINT_FILE_H
#define MKV_READER_PRINT_FILE_H

#include "../Get_and_Run_Info/Get/RequiredFontPacks.h"
#include "../ParseTXT/ParseCSVFiles.h"

void print_Required_Font_Pack_Names(const Episode* Names, const size_t* count1);

void print_File_Contents(const char* type, const FileNames* contents);

#endif //MKV_READER_PRINT_FILE_H