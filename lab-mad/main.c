#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <mad.h>
#include <alsa/asoundlib.h>

#define SAMPLE_RATE 44100
#define CHANNELS 2
#define PCM_DEVICE "default"

static snd_pcm_hw_params_t *hwparams = NULL;
static snd_pcm_t *pcm_handle = NULL;
struct buffer
{
	unsigned char const *start;
	unsigned long length;
};

static int decode (unsigned char const *, unsigned long);
static int init_alsa ();

int main (int argc, char *argv[])
{
	struct stat stat;
	void *fdm;
	char const *file;
	int fd;
	file = argv[1];
	if (argv[1] == NULL) {
		printf("please input mp3 file name .\n");
		return -1;
	}
	printf("%s\n", file);

	fd = open (file, O_RDONLY);
	if (fstat (fd, &stat) == -1 || stat.st_size == 0)
		return -1;
//	printf("%ld\n", stat.st_size/20);
	fdm = mmap (0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (fdm == MAP_FAILED)
		return -1;
	if (init_alsa () == -1)
	{
		fprintf (stderr, "init_alsa() error\n");
		return -1;
	}
//	printf("pre decode\n");
	decode(fdm, stat.st_size);
	if (munmap (fdm, stat.st_size) == -1)
		return -1;
	return 0;
}

static int init_alsa ()
{
	snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
	char *pcm_name;
	int rate = SAMPLE_RATE;     /* Sample rate */
	int exact_rate;             /* Sample rate returned by */
	pcm_name = strdup (PCM_DEVICE);
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
	if (snd_pcm_hw_params_set_channels (pcm_handle, hwparams, CHANNELS) < 0)
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

static enum mad_flow input (void *data, struct mad_stream *stream)
{
	struct buffer *buffer = data;
	if (!buffer->length)
		return MAD_FLOW_STOP;
	mad_stream_buffer (stream, buffer->start, buffer->length);
	buffer->length = 0;
//	printf("input func:stream->buffer=%x,stream->bufend=%x,stream->this_frame=%x\n",
//				(unsigned long)stream->buffer, stream->bufend, stream->this_frame);
	return MAD_FLOW_CONTINUE;
}

static inline signed int scale (mad_fixed_t sample)
{
	sample += (1L << (MAD_F_FRACBITS - 16));
	if (sample >= MAD_F_ONE)
		sample = MAD_F_ONE - 1;
	else if (sample < -MAD_F_ONE)
		sample = -MAD_F_ONE;
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}

static enum mad_flow output (void *data, struct mad_header const *header, struct mad_pcm *pcm)
{
	static int ji = 0;
	unsigned int nchannels, nsamples, n;
	mad_fixed_t const *left_ch, *right_ch;
	unsigned char Output[6912], *OutputPtr;
	int fmt, wrote, speed, exact_rate, err, dir;
	static int i = 0;
//printf("output ...,i=%d\n",i++);
	nchannels = pcm->channels;
	n = nsamples = pcm->length;
	left_ch = pcm->samples[0];
	right_ch = pcm->samples[1];
	fmt = AFMT_S16_LE;
	speed = pcm->samplerate * 2;
	OutputPtr = Output;
	while (nsamples--)
	{
		signed int sample;
		sample = scale (*left_ch++);
		*(OutputPtr++) = sample >> 0;
		*(OutputPtr++) = sample >> 8;
		if (nchannels == 2)
		{
			sample = scale (*right_ch++);
			*(OutputPtr++) = sample >> 0;
			*(OutputPtr++) = sample >> 8;
		}
	}
	OutputPtr = Output;
	snd_pcm_writei (pcm_handle, OutputPtr, n);
	OutputPtr = Output;
/*	if (++ji == 1000) {
		return MAD_FLOW_STOP;
	}*/
	return MAD_FLOW_CONTINUE;
}

static enum mad_flow error (void *data, struct mad_stream *stream, struct mad_frame *frame)
{
	return MAD_FLOW_CONTINUE;
}

static int decode (unsigned char const *start, unsigned long length)
{
	struct buffer buffer;
	struct mad_decoder decoder;
	int result;
	buffer.start = start;
	buffer.length = length;
	mad_decoder_init (&decoder, &buffer, input, 0, 0, output, error, 0);
	mad_decoder_options (&decoder, 0);
	result = mad_decoder_run (&decoder, MAD_DECODER_MODE_SYNC);
	mad_decoder_finish (&decoder);
	return result;
}
