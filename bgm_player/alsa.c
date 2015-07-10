#ifdef SNDalsa
#include <alsa/asoundlib.h>
#define RATE 44100
#define PERIOD_SIZE 48
#define PERIODS 3
#define CHANNELS 2
#define BITS 16
#define FORMAT _FORMAT(S, 16, L)
#define _FORMAT(sign, bits, endian) SND_PCM_FORMAT_ ## sign ## bits ## _ ## endian ## E
#define SUBFORMAT SND_PCM_SUBFORMAT_STD
#define check(value); {if((value)<0)force_exit_signal=1;}
#include "main.h"
void snd_alsa_init(void **handle) {
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);
	check(snd_pcm_open((snd_pcm_t**)handle, "rt", SND_PCM_STREAM_PLAYBACK, 0));
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
	//snd_pcm_hw_params_free(hwparams);
	//snd_pcm_sw_params_free(swparams);
}
int snd_alsa_write(void *handle, void *wbuf, int len) {
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
void snd_alsa_close(void *handle) {
	snd_pcm_close(handle);
}
#endif
