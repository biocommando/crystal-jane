@echo off
set vers=2.0

set file=build_info.h
echo #ifndef BUILD_INFO_H > %file%
echo #define BUILD_INFO_H >> %file%
echo #define VERSION "%vers%" >> %file%
echo #define BUILD_DATE "%date%, time %time%" >> %file%
echo #endif >> %file%

@echo on
gcc -static-libstdc++ pomppu.c allegro_compat.c synth.c music.c -Iinclude ^
 liballegro_audio.dll.a ^
 liballegro_font.dll.a ^
 liballegro_image.dll.a ^
 liballegro_primitives.dll.a ^
 liballegro.dll.a ^
 -O3 -o crystal
