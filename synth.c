#include "synth.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

float filter_factor = 0;
float filter_state[2] = {0, 0};

void update_lowpass(float cutFreqHz)
{
    filter_factor = 1.0f / (1.0f / (2.0f * 3.14159265f * 1.0f / 44100 * cutFreqHz) + 1.0f);
}

float process_lowpass(float input)
{
    filter_state[0] = filter_factor * input + (1.0f - filter_factor) * filter_state[1];
    filter_state[1] = filter_state[0];

    return filter_state[0];
}

struct synth_voice
{
    float volume;
    float actual_volume;
    float vol_mult;
    float phase;
    float phase_inc;
};

struct synth_track_event
{
    unsigned char offset;
    unsigned char voice;
    unsigned char key;
};

struct synth_voice voices[4];
struct synth_track_event track[1024];
int track_pos = 9999;
int next_event_count = 0;

static inline float soft_clip(const float f)
{
    const float f2 = f * f;
    return f * (27 + f2) / (27 + 9 * f2);
}

void synth_process(float *buf, int size)
{
    for (int i = 0; i < size; i += 2)
    {
        while (track_pos < 1024 && next_event_count == 0)
        {
            next_event_count = track[track_pos].offset * 256;
            if (track[track_pos].voice == 0xF0)
                track_pos = 0;
            else if (track[track_pos].voice == 0xF1)
                track_pos = 9999;
            else
            {
                trigger_sound(track[track_pos].voice, track[track_pos].key);
                track_pos++;
            }
        }
        next_event_count--;
        float out = 0;
        for (int v = 0; v < 4; v++)
        {
            if (voices[v].phase_inc > 0)
            {
                voices[v].phase += voices[v].phase_inc;
                if (voices[v].phase > 1)
                    voices[v].phase -= 1;
                float val = voices[v].phase;
                val = v == 0 || v == 3
                          ? (val > 0.5 ? 1 : -1)
                          : (voices[v].phase - 0.5f);
                out += voices[v].actual_volume * val;
                voices[v].actual_volume *= voices[v].vol_mult;
            }
        }
        out = soft_clip(out);
        out = process_lowpass(out);
        buf[i] = out;
        buf[i + 1] = out;
    }
}

void synth_init(int flags)
{
    update_lowpass(6000);
    track_pos = 9999;
    memset(&voices, 0, sizeof(voices));
    for (int i = 0; i < 4; i++)
    {
        int vflags = (flags >> (i * 8)) & 0xFF;
        voices[i].volume = 1;
        voices[i].vol_mult = 1;
        if (vflags & 0xF0)
            voices[i].vol_mult = 1.0f - ((vflags >> 4) & 0x0F) / 15000.0f;
        if (vflags & 0x0F)
            voices[i].volume = (vflags & 0x0F) / 15.0f;
    }
}

void trigger_sound(int voice, float freq)
{
    if (voice >= 0 && voice < 4)
    {
        if (freq == 0)
            voices[voice].phase_inc = 0;
        else
            voices[voice].phase_inc = pow(2, freq / 12) * 16.352f / 44100.0f;
        voices[voice].actual_volume = voices[voice].volume;
    }
}

void set_sequence(const char *seq, int size)
{
    track_pos = 0;
    next_event_count = 0;
    if (size < 1024 * 3)
    {
        int i;
        for (i = 0; i < size / 3; i++)
        {
            track[i].offset = seq[i * 3];
            track[i].voice = seq[i * 3 + 1];
            track[i].key = seq[i * 3 + 2];
        }
    }
}