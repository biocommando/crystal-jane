#ifndef synth_h
#define synth_h

void synth_process(float *buf, int size);

void synth_init(int flags);

void set_sequence(const char *seq, int size);

void trigger_sound(int voice, float freq);

#endif