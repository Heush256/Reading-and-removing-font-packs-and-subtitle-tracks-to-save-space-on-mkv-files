#include <stdio.h>
#include <dirent.h>
#include <windows.h>
#include "Files/ParseTXT/ParseCSVFiles.h"
#include "Files/Get_and_Run_Info/Get/RequiredFontPacks.h"
#include "Files/InputDealing/Format_Outputs.h"
#include "Files/PrintANDFree/Print_File.h"
#include "Files/PrintANDFree/Free_File.h"
#include "Files/Variables/EpisodeStruct.h"

const FileNames* default_paks = NULL;

SpaceSaved* space_saved = NULL;

const char** currentDir(const char* folder_name, int size) {
    char** list_to_return = NULL;
    DIR *dir = opendir(folder_name);
    if (!dir) return NULL;
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_name[0] == '.') continue;
        const size_t temp_len = strlen(folder_name) + strlen(ent->d_name) + 2;
        char* full_path = calloc(temp_len, sizeof(char));
        snprintf(full_path, temp_len, "%s/%s", folder_name, ent->d_name);
        const DWORD attribs = GetFileAttributesA(full_path);
        if (attribs != INVALID_FILE_ATTRIBUTES && attribs & FILE_ATTRIBUTE_DIRECTORY) continue;
        free(full_path);
        full_path = NULL;
        char** temp = realloc(list_to_return, (size + 2) * sizeof(char*));
        if (!temp) {
            perror("Failed to allocate memory");
            if (list_to_return != NULL) {
                for (int i = 0; i < size; i++)
                    free(list_to_return[i]);
                free(list_to_return);
            }
            closedir(dir);
            return NULL;
        }
        list_to_return = temp;
        list_to_return[size++] = strdup(ent->d_name);
        list_to_return[size] = NULL;
    }
    closedir(dir);
    return (const char**)list_to_return;
}

int early_exit(const char* folder_name, const char** name_of_files_in_folder_dir) {
    printf("Failed to change directory to: %s\n", folder_name);
    for (int i = 0; name_of_files_in_folder_dir[i] != NULL; i++)
        free((void*) name_of_files_in_folder_dir[i]);
    free(name_of_files_in_folder_dir);
    return -3;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    space_saved = readTXTfile("../CSV texts/Amount Saved so far.txt"),
    default_paks = readCSVfile("../CSV texts/Default Subtitle Names on Windows.csv");
    if (!default_paks || !space_saved) return -1;
    // print_File_Contents("Default", default_paks);
    const char* folder_name = "../Vid", **VidNamesInFolder = currentDir(folder_name, 0);
    if (!VidNamesInFolder) return -2;
    if (SetCurrentDirectoryA(folder_name) == 0) return early_exit(folder_name, VidNamesInFolder);
    printf("Successfully moved inside folder: %s\n", folder_name);
    Episode* Names = NULL;
    size_t epsCount = 0;


    for (int i = 0; VidNamesInFolder[i] != NULL; i++)
        Names = ffmpegParsing(VidNamesInFolder[i], Names, &epsCount), free((void*) VidNamesInFolder[i]);
    free(VidNamesInFolder), VidNamesInFolder = NULL;



    // print_Required_Font_Pack_Names(Names, &epsCount);



    format_print_Ep_info(Names, &epsCount);


    free_Episode_struct(Names, &epsCount);
    Names = NULL;

    return 0;
}