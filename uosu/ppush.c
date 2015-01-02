#include <time.h>
#include <stdint.h>
#include "thread.h"

void *ppush(pt_ppush *p) {
	const struct timespec ms = {0, 1000000};
	hito *hop, *hox = p->buf + p->buf_len - 1, *hol = p->buf;
	size_t cms;
	double /*c300t = 78 - p->hard->od * 6,*/
				c100t = 138 - p->hard->od * 8,
				c50t = 198 - p->hard->od * 10;
	double arms = p->hard->ar > 5
				? 1200 - (p->hard->ar - 5) * 150
				: 1200 + (5 - p->hard->ar) * 120;
	while (1) {
		if (*p->done) break;
		cms = (double) *p->cur / 44.100;
		pthread_mutex_lock(p->mutex);
		for (hop = hol; hop <= hox; hop++) {
			if (hop->time - arms > cms) break;
			hop->cshow = 1;
			if (hop->clicked != SIZE_MAX
					? hop->clicked + 200 < cms : hop->endtime + c50t < cms) {
				hop->alpha = -1; if (hop == hol) hol++;
			} else if (hop->clicked != SIZE_MAX) {
				hop->alpha = 1 - (double) (cms - hop->clicked) / 200;
				hop->rgb = 1;
			} else if (hop->endtime + c100t < cms) {
				hop->alpha = 1 - (double) (cms - (hop->endtime + c100t))
						 / (c50t - c100t);
				hop->rgb = 1;
			} else if (hop->time - 300 < cms) {
				hop->rgb = 1; hop->alpha = 1;
			} else {
				if (hop->time - 400 < cms)
					hop->rgb = 0.75 + (double) (cms - (hop->time - 400)) / 400;
				else hop->rgb = 0.75;
				if (hop->time - arms + 400 > cms) {
					if (hop->type & OSU_OBJ_TYPE_SLIDER)
						hop->cshow = (double) (cms - (hop->time - arms)) / 400;
					hop->alpha = (double) (cms - (hop->time - arms)) / 400;
				} else hop->alpha = 1;
			}
			if (hop->time > cms) {
				hop->acsize = 4 - 3 * (cms - (hop->time - arms)) / arms;
			} else {
				hop->acsize = -1;
			}
			if (hop->endtime <= cms && hop->clicked == SIZE_MAX)
					hop->clicked = cms; // TODO auto
		}
		pthread_mutex_unlock(p->mutex);
		if (hol > hox) break;
		nanosleep(&ms, NULL);
	}
	return NULL;
}
