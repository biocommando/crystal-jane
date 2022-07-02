@echo off
set vers=2.0

set file=build_info.h
echo #ifndef BUILD_INFO_H > %file%
echo #define BUILD_INFO_H >> %file%
echo #define VERSION "%vers%" >> %file%
echo #define BUILD_DATE "%date%, time %time%" >> %file%
echo #endif >> %file%

if %1==static (
    set gccextra=-static-libstdc++
) ELSE (
    set gccextra=
)
@echo on
gcc %gccextra% pomppu.c allegro_compat.c synth.c music.c -Iinclude liballegro_monolith.dll.a -O3 -o crystal
