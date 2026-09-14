#include "CheckStr.h"
#include <ctype.h>
#include <string.h>
#include "../Variables/EpisodeStruct.h"
#include "../PrintANDFree/Print_File.h"

bool basic_match(const char* longer, const char* shorter) {
    for (; *longer != '\0'; longer++)
        if (tolower(*longer) == tolower(*shorter)) {
            const char* l = longer, *s = shorter;
            while (*l && *s && tolower(*l) == tolower(*s))
                l++, s++;
            if (*s == '\0') return true;
        }
    return false;
}

unsigned int convert(const unsigned int* pos_in_file) {
    if (*pos_in_file < 10) return 1;
    unsigned int to_return = *pos_in_file, amount = 1;
    while (to_return > 9) amount++, to_return /= 10;
    return amount;
}

void compare_strs(const char* name, Subtitle* const* subtitle, const unsigned int* subtitleSize, const unsigned int* pos,
    Episode* ep, size_t* count) {
    for (unsigned int i = 0; i < *subtitleSize; i++) {
        for (unsigned int j = 0; j < (*subtitle)[i].AttachmentAmount; j++) {
            Attachment* attachment = &(*subtitle)[i].attachment[j];
            if (attachment->value == 0) {
                const char* name2 = attachment->FontName;
                const bool fixed = strlen(name) >= strlen(name2) ? basic_match(name, name2) :
                basic_match(name2, name);
                if (fixed)
                    attachment->value = *pos, attachment->allocateAmount = convert(pos);
            }
        }
    }
    // print_Required_Font_Pack_Names(ep, count);
}
