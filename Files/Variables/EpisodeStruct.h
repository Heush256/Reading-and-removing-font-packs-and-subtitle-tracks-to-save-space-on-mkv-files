#ifndef MKV_READER_EPISODESTRUCT_H
#define MKV_READER_EPISODESTRUCT_H

#include <stdbool.h>

typedef struct {
    char* FontName;
    unsigned int value, allocateAmount;
    bool is_default;
} Attachment;

typedef struct {
    Attachment* attachment;
    char* Type, *lang;
    unsigned int AttachmentAmount;
    bool is_default;
} Subtitle;

typedef struct Medium{
    char* short_name_for_Medium, *TitleName;
    bool is_default;
} Audio, Video;

enum Type {
    B,
    KB,
    MB,
    GB,
    TB,
    PB,
    EB,
    ZB,
    YB
};

typedef struct {
    float amount;
    enum Type type;
} SpaceSaved;

typedef struct {
    SpaceSaved* originalSize, *finalSize;
} Paket;

typedef struct {
    Paket* paket;
    Subtitle* subtitle;
    Audio* audio;
    Video* video;
    char* ShowName;
    unsigned int VidAmount, AudiosAmount, SubtitleAmount;
} Episode;

#endif //MKV_READER_EPISODESTRUCT_H
