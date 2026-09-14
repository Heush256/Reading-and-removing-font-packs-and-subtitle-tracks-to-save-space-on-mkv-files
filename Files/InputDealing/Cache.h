#ifndef MKV_READER_CACHE_H
#define MKV_READER_CACHE_H

struct Cache {
    struct Medium** vid, **audio;
    struct Subtitle** subtitle;
    unsigned int VidSize, AudioSize, SubtitleSize;
};

void free_Cache(struct Cache* cache);

struct Cache* add_cache(struct Cache* cache, const Episode* Names, const struct User_Inputs* User);

bool match_cache(struct Cache** cache_ptr, const Episode* Names, struct User_Inputs* Users, unsigned int* user_size);

#endif //MKV_READER_CACHE_H