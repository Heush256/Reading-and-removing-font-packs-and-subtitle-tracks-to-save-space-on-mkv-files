#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include "../../ParseTXT/ParseCSVFiles.h"
#include "RequiredFontPacks.h"
#include "../../CheckStr/CheckStr.h"

extern const FileNames* default_paks;

bool check_defaultCSVStruct(const char* token) {
    for (int i = 0; i < default_paks->count; i++)
        if (strcmp(token, default_paks[i].File_Name) == 0)
            return true;
    return false;
}

void append_to_Pack_Names_struct(const char* token, Subtitle** subtitle, const unsigned int* count) {
    Attachment* attachment = (*subtitle)[*count].attachment;
    const unsigned int size = (*subtitle)[*count].AttachmentAmount;
    Attachment* temp_attachment = realloc(attachment, (size + 1) * sizeof(Attachment));
    if (!temp_attachment) {
        fprintf(stderr, "Memory allocation failed for attachment struct!\n");
        exit(1);
    }
    (*subtitle)[*count].attachment = temp_attachment;
    attachment = temp_attachment;
    attachment[size].FontName = strdup(token);
    if (!attachment[size].FontName) {
        fprintf(stderr, "Memory allocation for String duplication failed!\n");
        exit(1);
    }
    attachment[size].is_default = check_defaultCSVStruct(token);
    attachment[size].value = 0;
    attachment[size].allocateAmount = 0;
    (*subtitle)[*count].AttachmentAmount++;
}

void parse_ass_fonts(const uint8_t* extraData, const int size, Subtitle** subtitle, const unsigned int* count) {
    if (!extraData || size == 0) return;
    char* header = malloc(size + 1);
    if (!header) return;
    memcpy(header, extraData, size);
    header[size] = '\0';
    for (const char* line = strtok(header, "\r\n"); line != NULL; line = strtok(NULL, "\r\n")) {
        if (strncmp(line, "Style:", 6) != 0) continue;
        char* style_copy = strdup(line + 7);
        if (!style_copy) continue;
        const char* first_comma_location = strchr(style_copy, ',');
        if (!first_comma_location) {
            free(style_copy);
            continue;
        }
        const char* font_start = first_comma_location + 1;
        char* second_comma = strchr(font_start, ',');
        if (!second_comma) {
            free(style_copy);
            continue;
        }
        *second_comma = '\0';
        const char* token = font_start;
        while (*token == ' ') token++;
        // printf("|-> Subtitle requires font: \"%s\"\n", token);
        append_to_Pack_Names_struct(token, subtitle, count);
        free(style_copy);
    }
    free(header);
}

bool matchPosToName(const char* fontName, const unsigned int* pos, Subtitle** subtitle, const unsigned int* size) {
    bool flag = false;
    for (int j = 0; j < *size; j++)
        for (int k = 0; k < (*subtitle)[j].AttachmentAmount; k++)
            if ((flag = strcasecmp(fontName, (*subtitle)[j].attachment[k].FontName) == 0 &&
                (*subtitle)[j].attachment[k].value == 0))
                (*subtitle)[j].attachment[k].value = *pos, (*subtitle)[j].attachment[k].allocateAmount = convert(pos);
    return flag;
}

Paket* create_Paket(Paket** paket) {
    Paket* temp = realloc(*paket, sizeof(Paket));
    if (!temp) {
        fprintf(stderr, "Memory allocation failed for Paket struct!\n");
        exit(1);
    }
    temp->originalSize = NULL;;
    temp->finalSize = NULL;
    return temp;
}

Episode* create_Ep_Struct(const char* name, const size_t* count, Episode* content) {
    Episode* temp = realloc(content, (*count + 1) * sizeof(Episode));
    if (!temp) {
        fprintf(stderr, "Memory allocation failed for Base Subtitle struct!\n");
        exit(1);
    }
    temp[*count].ShowName = strdup(name);
    temp[*count].VidAmount = 0;
    temp[*count].SubtitleAmount = 0;
    temp[*count].AudiosAmount = 0;
    temp[*count].video = NULL;
    temp[*count].subtitle = NULL;
    temp[*count].audio = NULL;
    temp[*count].paket = NULL;
    temp[*count].paket = create_Paket(&temp[*count].paket);
    return temp;
}

struct Medium* create_medium(struct Medium* medium, const char* title, const char* lang, unsigned int* pos) {
    struct Medium* temp = realloc(medium, (*pos + 1) * sizeof(struct Medium));
    if (!temp) {
        fprintf(stderr, "Fatal error: realloc failed in create_medium!\n");
        exit(1);
    }
    medium = temp;
    medium[*pos].short_name_for_Medium = strdup(lang);
    medium[*pos].TitleName = strdup(title);
    medium[*pos].is_default = false;
    (*pos)++;
    return medium;
}

void set_default(const Episode* ep, const unsigned int* which_case_is_Default) {
    switch (*which_case_is_Default) {
        case 1:
            if (ep->video && ep->VidAmount > 0)
                ep->video[ep->VidAmount - 1].is_default = true;
            break;
        case 2:
            if (ep->audio && ep->AudiosAmount > 0)
                ep->audio[ep->AudiosAmount - 1].is_default = true;
            break;
        case 3:
            if (ep->subtitle && ep->SubtitleAmount > 0)
                ep->subtitle[ep->SubtitleAmount - 1].is_default = true;
            break;
        default:
            break;
    }
}

Subtitle* create_subtitle(Subtitle* subtitle, const char* title, const char* lang, unsigned int* pos) {
    Subtitle* temp = realloc(subtitle, (*pos + 1) * sizeof(Subtitle));
    if (!temp) {
        fprintf(stderr, "Fatal error: realloc failed in create_medium!\n");
        exit(1);
    }
    subtitle = temp;
    subtitle[*pos].lang = strdup(lang);
    subtitle[*pos].Type = strdup(title);
    subtitle[*pos].attachment = NULL;
    subtitle[*pos].is_default = false;
    subtitle[*pos].AttachmentAmount = 0;
    (*pos)++;
    return subtitle;
}

uint16_t read_u16(const uint8_t *p) {
    return p[0] << 8 | p[1];
}

uint32_t read_u32(const uint8_t *p) {
    return (uint32_t)p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
}

void get_internal_ttf_names(const uint8_t *data, const size_t size, char ***names_out, int *count_out) {
    if (!data || size < 12) {
        *names_out = NULL, *count_out = 0;
        return;
    }

    const uint16_t num_tables = read_u16(data + 4);
    size_t table_offset = 12;
    char **names = NULL;
    int capacity = 0;

    for (int i = 0; i < num_tables; i++) {
        if (table_offset + 16 > size) break;
        if (memcmp(data + table_offset, "name", 4) != 0) {
            table_offset += 16;
            continue;
        }

        const uint32_t offset = read_u32(data + table_offset + 8);
        if (offset >= size) break;

        const uint8_t *name_table = data + offset;
        const uint16_t count = read_u16(name_table + 2), string_offset = read_u16(name_table + 4);
        const uint8_t *strings = name_table + string_offset;

        for (int j = 0; j < count; j++) {
            const uint8_t *rec = name_table + 6 + j * 12;
            const uint16_t platform_id = read_u16(rec + 0), encoding_id = read_u16(rec + 2),
            name_id = read_u16(rec + 6), length  = read_u16(rec + 8), str_off = read_u16(rec + 10);

            if ((name_id != 1 && name_id != 4) || (size_t)(strings + str_off + length - data) > size)
                continue;

            const uint8_t *raw = strings + str_off;
            char *utf8 = NULL;
            if (platform_id == 3 || platform_id == 0) {
                // Windows (UTF-16BE) or Unicode platform
                utf8 = malloc(length * 3 + 1);  // safe upper bound for UTF‑8
                if (!utf8) {
                    fprintf(stderr, "FAILED TO ALLOCATE for font name!\n");
                    exit(1);
                }
                size_t out = 0;
                for (size_t k = 0; k + 1 < length; k += 2) {
                    const uint16_t code = raw[k] << 8 | raw[k+1];
                    if (code < 0x80)
                        utf8[out++] = (char)code;
                    else if (code < 0x800)
                        utf8[out++] = (char)(0xC0 | code >> 6), utf8[out++] = (char)(0x80 | code & 0x3F);
                    else
                        utf8[out++] = (char)(0xE0 | code >> 12), utf8[out++] = (char)(0x80 | code >> 6 & 0x3F),
                        utf8[out++] = (char)(0x80 | code & 0x3F);
                }
                utf8[out] = '\0';
            }
            else if (platform_id == 1 && encoding_id == 0) {
                // Macintosh Roman – approximate conversion to UTF‑8
                // (for simplicity, we treat bytes as Latin‑1, which covers most Western characters)
                utf8 = malloc(length * 2 + 1);
                if (!utf8) {
                    fprintf(stderr, "FAILED TO ALLOCATE for font name!\n");
                    exit(1);
                }
                size_t out = 0;
                for (size_t k = 0; k < length; k++) {
                    const unsigned char c = raw[k];
                    if (c < 0x80)
                        utf8[out++] = (char) c;
                    else
                        utf8[out++] = (char)(0xC0 | c >> 6), utf8[out++] = (char)(0x80 | c & 0x3F);
                }
                utf8[out] = '\0';
            }
            else continue;

            // Add the decoded string to the array
            char **temp = realloc(names, (capacity + 1) * sizeof(char*));
            if (!temp) {
                fprintf(stderr, "FAILED TO ALLOCATE for names array!\n");
                free(utf8);
                if (names) {
                    for (int k = 0; k < capacity; k++) free(names[k]);
                    free(names);
                }
                exit(1);
            }
            names = temp;
            names[capacity] = utf8;
            capacity++;
        }
        break;
    }
    *names_out = names, *count_out = capacity;
}

float convert_bytes(float size, SpaceSaved** saved_space) {
    while (size >= 1024 && (*saved_space)->type <= YB)
        size /= 1024, (*saved_space)->type++;
    return size;
}

SpaceSaved* create_SpaceSaved_struct(const AVFormatContext* ifmt_ctx) {
    SpaceSaved* temp = calloc(1, sizeof(SpaceSaved));
    temp->amount = convert_bytes((float) avio_size(ifmt_ctx->pb), &temp);
    return temp;
}

AVFormatContext* short_call(const char* name) {
    //Hides internal FFmpeg logs to hide probesize/codec warnings
    av_log_set_level(AV_LOG_QUIET);
    AVFormatContext *ifmt_ctx = NULL;
    if (avformat_open_input(&ifmt_ctx, name, NULL, NULL) < 0) {
        printf("Could not open the MKV file: %s\n", name);
        return NULL;
    }
    //Scan the container fields
    if (avformat_find_stream_info(ifmt_ctx, NULL) < 0) {
        printf("Could not find stream information.\n");
        avformat_close_input(&ifmt_ctx);
        return NULL;
    }
    return ifmt_ctx;
}

Episode* ffmpegParsing(const char* name, Episode* ep, size_t* count) {
    AVFormatContext *ifmt_ctx = short_call(name);
    if (!ifmt_ctx) return NULL;
    ep = create_Ep_Struct(name, count, ep);
    ep[*count].paket->originalSize = create_SpaceSaved_struct(ifmt_ctx);
    // printf("\n--- Tracks inside %s ---\n", name);
    unsigned int attachmentCounter = 1, which_case_is_Default = 0;
    for (unsigned int i = 0; i < ifmt_ctx->nb_streams; i++) {
        const AVStream *stream = ifmt_ctx->streams[i];
        const AVCodecParameters *codec_par = stream->codecpar;
        // Fetch common metadata tags if they exist
        const AVDictionaryEntry *title_tag = av_dict_get(stream->metadata, "title", NULL, 0),
        *lang_tag  = av_dict_get(stream->metadata, "language", NULL, 0);
        // printf("    Track #%d: ", i);
        switch (codec_par->codec_type) {
            case AVMEDIA_TYPE_VIDEO: {
                // printf("[Video]");
                const char *title_str = lang_tag && lang_tag->value ? lang_tag->value : "Untitled",
                *lang_str  = title_tag && title_tag->value ? title_tag->value : "Unknown";
                // printf(" Title: %s", title_str);
                ep[*count].video = create_medium(ep[*count].video, title_str, lang_str, &ep[*count].VidAmount);
                which_case_is_Default = 1;
                break;
            }
            case AVMEDIA_TYPE_AUDIO: {
                // printf("[Audio]");
                const char *title_str = title_tag && title_tag->value ? title_tag->value : "Untitled",
                *lang_str  = lang_tag && lang_tag->value ? lang_tag->value : "Unknown";
                // printf(" Title: %s Lang: %s", title_str, lang_str);
                ep[*count].audio = create_medium(ep[*count].audio, title_str, lang_str, &ep[*count].AudiosAmount);
                which_case_is_Default = 2;
                break;
            }
            case AVMEDIA_TYPE_SUBTITLE: {
                const char *title = title_tag && title_tag->value ? title_tag->value : "Untitled",
                *lang  = lang_tag  && lang_tag->value  ? lang_tag->value  : "unknown";
                // printf("[Subtitle] Lang: %s Title: %s", lang, title);
                ep[*count].subtitle = create_subtitle(ep[*count].subtitle,  title, lang, &ep[*count].SubtitleAmount);
                if ((codec_par->codec_id == AV_CODEC_ID_ASS || codec_par->codec_id == AV_CODEC_ID_SSA) &&
                    codec_par->extradata && codec_par->extradata_size > 0) {
                        // printf(" (ASS Subtitle Extradata found, size: %d bytes)\n", codec_par->extradata_size);
                        unsigned int ind = ep[*count].SubtitleAmount - 1;
                        parse_ass_fonts(codec_par->extradata, codec_par->extradata_size, &ep[*count].subtitle, &ind);
                        which_case_is_Default = 3;
                    }
                break;
            }
            case AVMEDIA_TYPE_ATTACHMENT: {
                // printf("[Attachment]");
                // Attachments store their file name under "filename" instead of "title"
                // const AVDictionaryEntry *file_tag = av_dict_get(stream->metadata, "filename", NULL, 0);
                // const char *fontName = file_tag && file_tag->value ? file_tag->value : "Untitled";
                // printf(" Name: %s", fontName);
                if (stream->codecpar->extradata && stream->codecpar->extradata_size > 0) {
                    int name_count = 0;
                    char** headers = NULL;
                    get_internal_ttf_names(stream->codecpar->extradata, stream->codecpar->extradata_size, &headers, &name_count);
                    if (headers && name_count > 0) {

                        // printf("\t\t-> Internal Names: ");
                        for (int k = 0; k < name_count; k++) {
                            // printf("[%s] ", headers[k]);
                            if (matchPosToName(headers[k], &attachmentCounter, &ep[*count].subtitle,
                                &ep[*count].SubtitleAmount))
                                break;
                        }
                        // printf("\n");


                        for (int k = 0; k < name_count; k++)
                            free(headers[k]);
                        free(headers);
                    }
                }
                // else
                    // printf(" Name: Unknown");
                attachmentCounter++;
                break;
            }
            default:
                // printf("[Unknown Track Type]");
                break;
        }
        if (stream->disposition & AV_DISPOSITION_DEFAULT)
            set_default(&ep[*count], &which_case_is_Default);
        // else
            // printf("\n");
        which_case_is_Default = 0;
    }
    for (unsigned int i = 0; i < ep[*count].SubtitleAmount; i++)
        for (unsigned int j = 0; j < ep[*count].subtitle[i].AttachmentAmount; j++) {
            const Attachment* att = &ep[*count].subtitle[i].attachment[j];
            if (!att->is_default && att->value == 0)
                printf("\n***[WARNING from ep #%llu]***: Required font '%s' is missing from the MKV.\n",
                    *count + 1, att->FontName);
        }
    avformat_close_input(&ifmt_ctx);
    (*count)++;
    return ep;
}
