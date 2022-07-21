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
 crystal.c allegro_compat.c common.c game_data.c globals.c graphics.c main.c synth.c ^
 liballegro_audio.dll.a ^
 liballegro_font.dll.a ^
 liballegro_image.dll.a ^
 liballegro_primitives.dll.a ^
 liballegro.dll.a ^
 -O3 -o crystal

@echo off

echo crystal.exe gmods\mad_dat.dat %%1 %%2 %%3 %%4 %%5 > super-mad-mode.bat

echo crystal.exe gmods\legacy.dat %%1 %%2 %%3 %%4 %%5 > legacy-mode.bat

echo crystal.exe gmods\modern.dat %%1 %%2 %%3 %%4 %%5 > modern-mode.bat
