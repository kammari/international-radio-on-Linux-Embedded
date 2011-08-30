#ifndef ALSA_H
#define ALSA_H

#define SAMPLE_RATE 44100
#define CHANNELS 2
#define PCM_DEVICE "plughw:0,0"


int init_alsa(unsigned long samplerate, int channals );
int recovery_alsa(int err);
long write_to_alsa(unsigned char *output_buffer, unsigned int n);

#endif
