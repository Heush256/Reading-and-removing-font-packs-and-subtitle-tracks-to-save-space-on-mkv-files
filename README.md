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
Each episode of mkv files can contain multiple video, audio, and subtitle streams, and multiple font packs.

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
