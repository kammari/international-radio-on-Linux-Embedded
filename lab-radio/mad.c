#include <stdio.h>
#include <pthread.h>
#include <mad.h>
#include <sys/soundcard.h>
#include <alsa/asoundlib.h>

#include "alsa.h"
#include "mad.h"
#include "flag.h"

static pthread_t mad_thread;
static pthread_mutex_t mad_stop_lock;
static pthread_mutex_t mad_loop_lock;


// if this flag was set true, we should go to next music
static flag_t * mad_flag;

//static const char* get_music_name() ;

static enum mad_flow input (void *data, struct mad_stream *stream);
static enum mad_flow output (void *data, struct mad_header const *header, struct mad_pcm *pcm);
static enum mad_flow error (void *data, struct mad_stream *stream, struct mad_frame *frame);
static inline signed int scale (mad_fixed_t sample);

static mad_t new_mad(file_t file);
static int destroy_mad(mad_t mad);

/*j
static const char* get_music_name() 
{
	static int i = 0;
	i++;
	if (i % 2 == 1) {
		return "blg.mp3";
	}
	else {
		return "77.mp3";
	}
}
*/
static void* mad_func(void *data) 
{
	struct mad_decoder decoder;

	file_t file = (file_t) data;

	mad_flag = new_flag();

	if (init_alsa(44100, 2) == -1) {
		fprintf(stderr, "init_alsa() error\n");
		return (void *)-1;
	}

	mad_t mad = new_mad(file);

	while (1) {

		pthread_mutex_lock(&mad_loop_lock);
sleep(1);
		mad->live_size = 0;
		mad->offset = 0;
		mad->stream = NULL;
		set_flag(mad_flag, '\0');

		//decode the data in mad buffer
		mad_decoder_init(&decoder, mad, input, 0, 0, output, error, 0);
		mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);
		mad_decoder_finish(&decoder);
		
		if (is_going_to_close(mad_flag)) {
			break;
		}

	}

	destroy_mad(mad);	
	pthread_mutex_destroy(&mad_stop_lock);
	pthread_mutex_destroy(&mad_loop_lock);
	
	return (void *)0;
}

static mad_t new_mad(file_t file)
{
	mad_t mad = (mad_t )malloc(sizeof(mad_strt));
	mad->file = file;
	mad->live_size = 0;
	mad->stream = NULL;
	mad->offset = 0;

	return mad;
}

static int destroy_mad(mad_t mad) 
{
/*	int ret;

	ret = file_close(mad->file);
	if (ret != 0) {
		fprintf(stderr, "%s : file cant close.\n", "destroy_mad");
	}
*/
	free(mad);
	return 0;
}

int mad_init(file_t share_file) 
{
	int ret;

	ret = pthread_mutex_init(&mad_stop_lock, NULL);	
	if (ret != 0) {
		fprintf(stderr, "%s : pthread_mutex_init error.\n", "new_mad");
	}

	ret = pthread_mutex_init(&mad_loop_lock, NULL);	
	if (ret != 0) {
		fprintf(stderr, "%s : pthread_mutex_init error.\n", "new_mad");
	}
	pthread_mutex_lock(&mad_loop_lock);

	ret = pthread_create(&mad_thread, NULL, mad_func, share_file);
	if (ret != 0) {
		fprintf(stderr, "pthread_create error.\n");
		return ret;
	}

	printf("pthread_create ok.\n");
	return 0;
}

int start_mad()
{
	pthread_mutex_unlock(&mad_loop_lock);
	return 0;
}

int next_mad()
{
	set_flag(mad_flag, 'n');			
	return 0;
}

int close_mad() 
{
	set_flag(mad_flag, 'q');
	return 0;
}

int stop_mad()
{
	printf("stop mad.\n");
	pthread_mutex_lock(&mad_stop_lock);
	return 0;
}

int continue_mad()
{
	printf("continue mad.\n");
	pthread_mutex_unlock(&mad_stop_lock);
	return 0;
}

int wait_mad()
{
	pthread_join(mad_thread, NULL);
	return 0;
}

static enum mad_flow output (void *data, struct mad_header const *header, struct mad_pcm *pcm)
{

	unsigned int nchannels, nsamples, n;
	mad_fixed_t const *left_ch, *right_ch;
	unsigned char Output[6912], *OutputPtr;
	int fmt, speed, err;

	static int i = 0;
	i++;

	nchannels = pcm->channels;
	n = nsamples = pcm->length;
	left_ch = pcm->samples[0];
	right_ch = pcm->samples[1];
	fmt = AFMT_S16_LE;
	speed = pcm->samplerate * 2;
	OutputPtr = Output;
	//          printf("output ...,i=%d, %x\n",i++, scale(*left_ch));
//	printf("sample rate = %u, num = %d\n", header->samplerate, i);
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
	while (n > 0) {
		err = write_to_alsa(OutputPtr, n);
		if (err == -EAGAIN) {
			continue;
		}
		if (err < 0) {

			if (recovery_alsa( err) < 0){
				printf("Write error:%s\n", snd_strerror(err));
				exit(EXIT_FAILURE);
			}
			break;
		}
		OutputPtr += err*nchannels;
		n -= err;
	}

	pthread_mutex_lock(&mad_stop_lock);
	pthread_mutex_unlock(&mad_stop_lock);
	if (is_going_to_next(mad_flag) || is_going_to_close(mad_flag)) {
		return MAD_FLOW_BREAK;
	}

	return MAD_FLOW_CONTINUE;
}

static enum mad_flow error (void *data, struct mad_stream *stream, struct mad_frame *frame)
{
	return MAD_FLOW_CONTINUE;
}


static enum mad_flow input (void *data, struct mad_stream *stream)
{
	mad_t mad = (mad_t )data;
	size_t remaining, read_size;
	unsigned char * read_start;

	/********************************************************/
	if (stream->this_frame != NULL) {
		remaining = stream->bufend - stream->this_frame;
		memmove(mad->mdata, stream->this_frame, remaining);
		read_start = mad->mdata + remaining;
		read_size = BUFFERSIZE - remaining;
//printf("the other in this..., remaining = %d\n", remaining);
	}	
	else {
		printf("first in this.\n");
		remaining = 0;	
		read_start = mad->mdata;
		read_size = BUFFERSIZE;
	}

//	printf("read_size = %d, stream.error = %d\n", read_size, stream->error);	
	/****************************************************/
//printf("mad offset =%ld\n", mad->offset);
	read_size = file_read(read_start, mad->offset, read_size, mad->file);
	mad->offset += read_size;
//printf("read_size = %d\n", read_size);
	if (read_size <= 0) {
		if (ferror(mad->file->fp)) {
			fprintf(stderr, "cant read data from file.\n");
		}
		if (mad->file->eof) {
			fprintf(stderr, "end of input stream.\n");
			return MAD_FLOW_STOP;	
		}
	}


	/****************************************************/
	mad_stream_buffer(stream, mad->mdata, read_size+remaining);
	mad->stream = stream;

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

