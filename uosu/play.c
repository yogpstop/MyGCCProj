#include <alsa/asoundlib.h>
#include "osu.h"
#include "thread.h"

#define check(value); {if((value)<0)write(2, "ERROR\n", 6);}
static inline void init_alsa(snd_pcm_t **handle) {
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	check(snd_pcm_hw_params_malloc(&hwparams));
	check(snd_pcm_sw_params_malloc(&swparams));
	check(snd_pcm_open(handle, "rt", SND_PCM_STREAM_PLAYBACK, 0));
	check(snd_pcm_hw_params_any(*handle, hwparams));
	check(snd_pcm_hw_params_set_access(*handle, hwparams,
			SND_PCM_ACCESS_RW_INTERLEAVED));
	check(snd_pcm_hw_params_set_period_size(*handle, hwparams, 48, 0));
	check(snd_pcm_hw_params_set_buffer_size(*handle, hwparams, 48 * 3));
	check(snd_pcm_hw_params_set_channels(*handle, hwparams, 2));
	check(snd_pcm_hw_params_set_format(*handle, hwparams,
			SND_PCM_FORMAT_S16_LE));
	check(snd_pcm_hw_params_set_rate_resample(*handle, hwparams, 0));
	check(snd_pcm_hw_params_set_rate(*handle, hwparams, 44100, 0));
	check(snd_pcm_hw_params_set_subformat(*handle, hwparams,
			SND_PCM_SUBFORMAT_STD));
	check(snd_pcm_hw_params(*handle, hwparams));
	check(snd_pcm_sw_params_current(*handle, swparams));
	check(snd_pcm_sw_params_set_start_threshold(*handle, swparams, 48 * 3));
	check(snd_pcm_sw_params_set_stop_threshold(*handle, swparams, 48 * 3));
	check(snd_pcm_sw_params_set_avail_min(*handle, swparams, 48));
	check(snd_pcm_sw_params_set_period_event(*handle, swparams, 0));
	check(snd_pcm_sw_params(*handle, swparams));
	snd_pcm_hw_params_free(hwparams);
	snd_pcm_sw_params_free(swparams);
}
static inline int snd_write(snd_pcm_t *handle, void *rbuf, unsigned int len) {
	int err;
	while ((err = snd_pcm_writei(handle, rbuf, len)) == -EAGAIN);
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
void *play(pt_play *p) {
	snd_pcm_t *handle;
	init_alsa(&handle);
	size_t done, cur = 0;
	void *ptr = p->ptr;
	p->len /= 2 * 16 / 8;
	while (p->len > 0) {
		done = snd_write(handle, ptr, 48 > p->len ? p->len : 48);
		if (done < 0) continue;
		p->len -= done;
		ptr += done * 2 * 16 / 8;
		cur += done;
		snd_pcm_sframes_t delay;
		snd_pcm_delay(handle, &delay);
		*p->cur = cur - delay;
	}
	free(p->ptr);
	p->ptr = NULL;
	snd_pcm_close(handle);
	*p->done = 1;
	return NULL;
}
