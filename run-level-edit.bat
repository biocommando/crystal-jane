
gcc -static-libstdc++ -Iinclude ^
 level_edit.c allegro_compat.c common.c crystal.c game_data.c globals.c graphics.c synth.c ^
 liballegro_audio.dll.a ^
 liballegro_font.dll.a ^
 liballegro_image.dll.a ^
 liballegro_primitives.dll.a ^
 liballegro.dll.a ^
 -O3 -o level-crystal

 level-crystal %1 %2 %3 %4 %5