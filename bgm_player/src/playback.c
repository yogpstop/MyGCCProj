#include <alsa/asoundlib.h>
#define PERIOD_SIZE 48
#define PERIODS 3
#define CHANNELS 2
#define BITS 16
#define FORMAT _FORMAT(S, 16, L)
#define _FORMAT(sign, bits, endian) SND_PCM_FORMAT_ ## sign ## bits ## _ ## endian ## E
#define SUBFORMAT SND_PCM_SUBFORMAT_STD
#define RATE 44100
typedef struct {
	char *n;
	void *d;
	int l;
} pcm;
pcm buf[2] = {{NULL, NULL, 0}, {NULL, NULL, 0}};
int exit_signal = 0, force_exit_signal = 0;
static inline void check(int value) {
	if (value < 0) {
		exit(-1);
	}
}
static inline int snd_write(snd_pcm_t *handle, void *wbuf, int len) {
	int err;
	while ((err = snd_pcm_writei(handle, wbuf, len)) == -EAGAIN);
	if (err == -ESTRPIPE) {
		fprintf(stdout, "Error %d\n", err);
		while ((err = snd_pcm_resume(handle)) == -EAGAIN);
		if (err < 0) err = -EPIPE;
	}
	if (err == -EPIPE) {
		fprintf(stdout, "Error %d\n", err);
		err = snd_pcm_prepare(handle);
	}
	return err;
}
static inline void init_alsa(snd_pcm_t **handle) {
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);
	check(snd_pcm_open(handle, "rt", SND_PCM_STREAM_PLAYBACK, 0));
	check(snd_pcm_hw_params_any(*handle, hwparams));
	check(snd_pcm_hw_params_set_access(*handle, hwparams,
		SND_PCM_ACCESS_RW_INTERLEAVED));
	check(snd_pcm_hw_params_set_period_size(*handle, hwparams,
		PERIOD_SIZE, 0));
	check(snd_pcm_hw_params_set_buffer_size(*handle, hwparams,
		PERIOD_SIZE * PERIODS));
	check(snd_pcm_hw_params_set_channels(*handle, hwparams,
		CHANNELS));
	check(snd_pcm_hw_params_set_format(*handle, hwparams,
		FORMAT));
	check(snd_pcm_hw_params_set_rate_resample(*handle, hwparams,
		0));
	check(snd_pcm_hw_params_set_rate(*handle, hwparams,
		RATE, 0));
	check(snd_pcm_hw_params_set_subformat(*handle, hwparams,
		SUBFORMAT));
	check(snd_pcm_hw_params(*handle, hwparams));
	check(snd_pcm_sw_params_current(*handle, swparams));
	check(snd_pcm_sw_params_set_start_threshold(*handle, swparams,
		PERIOD_SIZE * PERIODS));
	check(snd_pcm_sw_params_set_stop_threshold(*handle, swparams,
		PERIOD_SIZE * PERIODS));
	check(snd_pcm_sw_params_set_avail_min(*handle, swparams,
		PERIOD_SIZE));
	check(snd_pcm_sw_params_set_period_event(*handle, swparams,
		0));
	check(snd_pcm_sw_params(*handle, swparams));
}
void *play_thread(void *_) {
	snd_pcm_t *handle;
	init_alsa(&handle);
	int t, cur_id = 0;
	void *p;
	pcm *str = buf + cur_id;
	while(1) {
		if(str->d == NULL || str->l == 0 || str->n == NULL) {
			if(force_exit_signal) return NULL;
			cur_id = cur_id ? 0 : 1;
			str = buf + cur_id;
			continue;
		}
		str->l /= CHANNELS * BITS / 8;
		p = str->d;
		fputs("\ec",stdout);
		fputs("Now playing... ", stdout);
		fputs(str->n, stdout);
		fputc('\n', stdout);
		while(str->l > 0) {
			if(exit_signal) break;
			t = str->l > PERIOD_SIZE ? PERIOD_SIZE : str->l;
			t = snd_write(handle, p, t);
			if(t < 0) exit(-1);
			p += t * CHANNELS *BITS / 8;
			str->l -= t;
		}
		free(str->n);
		str->n = NULL;
		free(str->d);
		str->d = NULL;
		str->l = 0;
		exit_signal = 0;
	}
	return NULL;
}
