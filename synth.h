#ifndef synth_h
#define synth_h

void synth_process(float *buf, int size);

void synth_init(int flags);

void trigger_sound(int voice, float freq);

void set_music(int track);

#endif