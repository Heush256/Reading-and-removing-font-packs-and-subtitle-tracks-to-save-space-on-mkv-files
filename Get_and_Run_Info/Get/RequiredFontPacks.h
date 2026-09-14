#ifndef MKV_READER_REQUIREDFONTPACKS_H
#define MKV_READER_REQUIREDFONTPACKS_H

#include <stdio.h>
#include "../../Variables/EpisodeStruct.h"
#include <libavformat/avformat.h>
#include <stdint.h>

SpaceSaved* create_SpaceSaved_struct(const AVFormatContext* ifmt_ctx);

AVFormatContext* short_call(const char* name);

Episode* ffmpegParsing(const char* name, Episode* ep, size_t* count);

#endif //MKV_READER_REQUIREDFONTPACKS_H