#include <alsa/asoundlib.h>

#include "alsa.h"


#define SAMPLE_RATE 44100
#define CHANNELS 2
#define PCM_DEVICE "plughw:0,0"


static snd_pcm_hw_params_t *hwparams = NULL;
static snd_pcm_t *pcm_handle = NULL;

int init_alsa(unsigned long samplerate, int pcm_channels )
{
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
    char *pcm_name;
    unsigned int exact_rate, rate; 
	unsigned int channels;

	if (samplerate == 0) {
		rate = SAMPLE_RATE; 
	}	
	else {
		rate = samplerate;
	}
	pcm_name = strdup (PCM_DEVICE);

	if (pcm_channels == 1) {
		channels = pcm_channels;
	}
	else {
		channels = 2;
	}


    snd_pcm_hw_params_alloca (&hwparams);
    if (snd_pcm_open (&pcm_handle, pcm_name, stream, 0) < 0)
    {
        fprintf (stderr, "Error opening PCM device %s\n", pcm_name);
        return -1;
    }
    if (snd_pcm_hw_params_any (pcm_handle, hwparams) < 0)
    {
        fprintf (stderr, "Can not configure this PCM device.\n");
        return -1;
    }
    if (snd_pcm_hw_params_set_access (pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
    {
        fprintf (stderr, "Error setting access.\n");
        return -1;
    }
    if (snd_pcm_hw_params_set_format (pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0)
    {
        fprintf (stderr, "Error setting format.\n");
        return -1;
    }
    exact_rate = rate;
    if (snd_pcm_hw_params_set_rate_near (pcm_handle, hwparams, &exact_rate, 0) < 0)
    {
        fprintf (stderr, "Error setting rate.\n");
        return -1;
    }
    if (rate != exact_rate)
    {
        fprintf (stderr, "The rate %d Hz is not supported by your hardware.\n==> Using %d Hz instead.\n",
                rate, exact_rate);
    }
    if (snd_pcm_hw_params_set_channels (pcm_handle, hwparams, channels) < 0)
    {
        fprintf (stderr, "Error setting channels.\n");
        return -1;
    }
    if (snd_pcm_hw_params (pcm_handle, hwparams) < 0)
    {
        fprintf (stderr, "Error setting HW params.\n");
        return -1;
    }
    return 0;
}


long write_to_alsa(unsigned char *output_buffer, unsigned int n) {
	return (long)snd_pcm_writei(pcm_handle, output_buffer, n);
}

int recovery_alsa(int err)
{
        printf("stream recovery\n");
    if (err == -EPIPE) {    /* under-run */
        err = snd_pcm_prepare(pcm_handle);
        if (err < 0)
            printf("Can't recovery from underrun, prepare failed: %s\n", snd_strerror(err));
        return 0;
    } else if (err == -ESTRPIPE) {
        while ((err = snd_pcm_resume(pcm_handle)) == -EAGAIN)
//            sleep(1);       /* wait until the suspend flag is released */
        if (err < 0) {
            err = snd_pcm_prepare(pcm_handle);
            if (err < 0)
                printf("Can't recovery from suspend, prepare failed: %s\n", snd_strerror(err));
        }
        return 0;
    }
    return err;
}

