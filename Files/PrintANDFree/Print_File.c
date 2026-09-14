#include <stdio.h>
#include "../Get_and_Run_Info/Get/RequiredFontPacks.h"
#include "../ParseTXT/ParseCSVFiles.h"

void print_Required_Font_Pack_Names(const Episode* Names, const size_t* count1) {
    if (!Names) {
        printf("\nNULL pointer passed!\n");
        return;
    }
    for (int i = 0; i < *count1; i++) {
        printf("\n-----------Name -> %s-----------\nVid count -> %d\nAudio count -> %d\nSubtitle count -> %d\n",
            Names[i].ShowName, Names[i].VidAmount, Names[i].AudiosAmount, Names[i].SubtitleAmount);
        if (Names[i].video != NULL)
            for (int j = 0; j < Names[i].VidAmount; j++)
                printf("~~~~~~~~~~~~~Vid -> Type -> %s, Lang -> %s, is default -> %s~~~~~~~~~~~~~\n",
                    Names[i].video[j].TitleName, Names[i].video[j].short_name_for_Medium,
                        Names[i].video[j].is_default ? "true" : "false");
        if (Names[i].audio != NULL)
            for (int j = 0; j < Names[i].AudiosAmount; j++)
                printf("~~~~~~~~~~~~~Aud -> Type -> %s, Lang -> %s, is default -> %s~~~~~~~~~~~~~\n",
                    Names[i].audio[j].TitleName, Names[i].audio[j].short_name_for_Medium,
                        Names[i].audio[j].is_default ? "true" : "false");
        if (Names[i].subtitle != NULL)
            for (int j = 0; j < Names[i].SubtitleAmount; j++) {
                printf("~~~~~~~~~~~~~Type -> %s, Lang -> %s, is default -> %s~~~~~~~~~~~~~\n",
                    Names[i].subtitle[j].Type, Names[i].subtitle[j].lang,
                        Names[i].subtitle[j].is_default ? "true" : "false");
                if (Names[i].subtitle[j].attachment != NULL)
                    for (int k = 0; k < Names[i].subtitle[j].AttachmentAmount; k++)
                        printf("Font name -> %s, is default -> %s, pos in mkv -> %d, if converted to str length: %d\n",
                            Names[i].subtitle[j].attachment[k].FontName,
                            Names[i].subtitle[j].attachment[k].is_default ? "true" : "false",
                            Names[i].subtitle[j].attachment[k].value,
                            Names[i].subtitle[j].attachment[k].allocateAmount);
            }
        printf("\n");
    }
}

void print_File_Contents(const char* type, const FileNames* contents) {
    printf("\nApart of -> %s\n\n", type);
    for (int i = 0; i < contents->count; i++)
        printf("Type -> %s\n", contents[i].File_Name);
}