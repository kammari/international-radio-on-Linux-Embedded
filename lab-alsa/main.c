#include <alsa/asoundlib.h>

#define  SAMPLE_RATE  44100

#define  CHANNELS    1

#define  LATENCY   (500000)

#define NBLOCKS 16
#define BLOCK_SIZE 1024

unsigned char buffer[NBLOCKS*BLOCK_SIZE];     

static char *device = "default";   /* playback device */

int main(int argc, char ** argv)
{
	int i;
	int j;
	int err, ret_code = 0;
	snd_pcm_t *handle;
	snd_pcm_sframes_t frames;

	for (i = 0; i < sizeof(buffer); i++)
	{
		buffer[i] = 0xff;
	}
	for(i = 1; i<sizeof(buffer);i=i+40)
	{
		for(j=0;j<4;j++)
			buffer[i+j] = 0x00;
	}

	err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	if ((err = snd_pcm_set_params(handle,
					SND_PCM_FORMAT_U8,
					SND_PCM_ACCESS_RW_INTERLEAVED,
					CHANNELS,
					SAMPLE_RATE,
					1,
					LATENCY)) < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < 10; i++) { 
		frames = snd_pcm_writei(handle, buffer, sizeof(buffer));
		if (frames < 0)
			frames = snd_pcm_recover(handle, frames, 0);
		if (frames < 0) {
			printf("snd_pcm_writei failed: %s\n", snd_strerror(err));
			ret_code = -1;
			break;
		}
		if (frames > 0 && frames < (long)sizeof(buffer))
			printf("Short write (expected %li, wrote %li)\n",
					 (long)sizeof(buffer), frames);
	}

	snd_pcm_close(handle);

	return ret_code;
}

