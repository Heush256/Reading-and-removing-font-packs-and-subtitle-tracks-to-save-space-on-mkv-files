# Variables

Hierarchy of structs meant to encapsulate information contained in an episode (below)

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
    
    typedef struct {
        Paket* paket;
        Subtitle* subtitle;
        Audio* audio;
        Video* video;
        char* ShowName;
        unsigned int VidAmount, AudiosAmount, SubtitleAmount;
    } Episode;
Each episode of mkv files can contain multiple video, audio, and subtitle streams, and multiple font packs. The boolean variable in the attachment struct checks if the font pack is default on windows, and if so, proceeds to remove it too. The booleans in the higher structs, while not implemented yet, is for when the inputs "*" for default choice.

A small struct added is the total amount of space saved over the total runs done for curiosity. It takes the difference between all of the episodes, and then sums them up at the end of the program, then stores the value into a file.

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

For user inputs, there is a cache implemented so that the user does not need to input the same choice multiple times, and they will only be prompted to input if there is no match

below is the cache struct

    struct Cache {
        struct Medium** vid, **audio;
        struct Subtitle** subtitle;
        unsigned int VidSize, AudioSize, SubtitleSize;
    };
Stores pointers to the video, audio, or subtitle stream, usually the first episode, unless there was not a match then, from any episode.

Creates dynamic command line for mkvtoolnix turning the pos of the font packs in the files, subtitles, and audios into strings in the command line.
