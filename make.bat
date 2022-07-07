@echo off
set vers=2.0

set file=build_info.h
echo #ifndef BUILD_INFO_H > %file%
echo #define BUILD_INFO_H >> %file%
echo #define VERSION "%vers%" >> %file%
echo #define BUILD_DATE "%date%, time %time%" >> %file%
echo #endif >> %file%

@echo on
gcc -static-libstdc++ -Iinclude ^
 crystal.c allegro_compat.c common.c game_data.c globals.c graphics.c synth.c ^
 liballegro_audio.dll.a ^
 liballegro_font.dll.a ^
 liballegro_image.dll.a ^
 liballegro_primitives.dll.a ^
 liballegro.dll.a ^
 -O3 -o crystal

@echo off

echo crystal.exe gmad_dat.dat %%1 %%2 %%3 %%4 %%5 > super-mad-mode.bat

echo crystal.exe glegacy.dat %%1 %%2 %%3 %%4 %%5 > legacy-mode.bat
