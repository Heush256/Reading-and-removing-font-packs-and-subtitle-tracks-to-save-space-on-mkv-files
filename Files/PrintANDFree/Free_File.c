#include <stdlib.h>
#include "Free_File.h"

void free_Episode_struct(const Episode* Names, const size_t* count1) {
    if (Names == NULL || count1 == NULL) return;
    for (int i = 0; i < *count1; i++) {
        free(Names[i].ShowName), free(Names[i].paket->finalSize), free(Names[i].paket->originalSize), free(Names[i].paket);
        if (Names[i].video != NULL) {
            for (int j = 0; j < Names[i].VidAmount; j++)
                free(Names[i].video[j].short_name_for_Medium), free(Names[i].video[j].TitleName);
            free(Names[i].video);
        }
        if (Names[i].audio != NULL) {
            for (int j = 0; j < Names[i].AudiosAmount; j++)
                free(Names[i].audio[j].TitleName), free(Names[i].audio[j].short_name_for_Medium);
            free(Names[i].audio);
        }
        if (Names[i].subtitle != NULL) {
            for (int j = 0; j < Names[i].SubtitleAmount; j++) {
                free(Names[i].subtitle[j].Type), free(Names[i].subtitle[j].lang);
                if (Names[i].subtitle[j].attachment != NULL) {
                    for (int k = 0; k < Names[i].subtitle[j].AttachmentAmount; k++)
                        free(Names[i].subtitle[j].attachment[k].FontName);
                    free(Names[i].subtitle[j].attachment);
                }
            }
            free(Names[i].subtitle);
        }
    }
    free((void*) Names);
}

void free_File_Contents(const FileNames* contents) {
    for (int i = 0; i < contents->count; i++)
        free(contents[i].File_Name);
    free((void*) contents);
    contents = NULL;
}
