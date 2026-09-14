#cmake_minimum_required(VERSION 4.1)
#project(SubtitleNTrackRemove C)
#
#set(CMAKE_C_STANDARD 11)
#
#add_executable(SubtitleNTrackRemove main.c)

cmake_minimum_required(VERSION 3.25)
project(mkv_reader C)

set(CMAKE_C_STANDARD 11)

# 1. Define the path where you extracted the FFmpeg SDK
set(FFMPEG_DIR "C:/ffmpeg-sdk")

# 2. Tell CLion where to find the header files (fixes your current error)
include_directories(${FFMPEG_DIR}/include)

# 3. Tell CLion where to find the library files (.lib / .a)
link_directories(${FFMPEG_DIR}/lib)

# 4. Define your executable
add_executable(SubtitleNTrackRemove main.c
        Files/ParseTXT/ParseCSVFiles.c
        Files/Get_and_Run_Info/Get/RequiredFontPacks.c
        Files/InputDealing/Format_Outputs.c
        Files/Get_and_Run_Info/Run/Run_Process.c
        Files/InputDealing/Cache.c
        Files/PrintANDFree/Print_File.c
        Files/PrintANDFree/Free_File.c
        Files/CheckStr/CheckStr.c)
# Automatically copy all FFmpeg DLLs to the build folder where the .exe lives
add_custom_command(TARGET SubtitleNTrackRemove POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${FFMPEG_DIR}/bin"
        "$<TARGET_FILE_DIR:SubtitleNTrackRemove>"
)

# 5. Link the specific FFmpeg libraries to your executable
target_link_libraries(SubtitleNTrackRemove avformat avcodec avutil)