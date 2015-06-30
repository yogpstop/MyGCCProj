#include <windows.h>
#include <vfw.h>

#include "avcodec.h"
#include "internal.h"

const enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P,AV_PIX_FMT_NONE};

typedef struct LagarithEncContext {
	HIC hic;
	LPBITMAPINFOHEADER bmih;
	LPBITMAPINFOHEADER bmiho;
	BYTE *cdata;
	BYTE *odata;
	unsigned long y;
	unsigned long u;
	unsigned long v;
} LagarithEncContext;

static av_cold int lag_encode_init(AVCodecContext *);
static av_cold int lag_encode_end(AVCodecContext *);
static int lag_encode_frame(AVCodecContext *, AVPacket *,const AVFrame *,int *);

static av_cold int lag_encode_init(AVCodecContext *avctx) {
	LagarithEncContext *l = avctx->priv_data;
	l->hic = ICOpen(ICTYPE_VIDEO,mmioFOURCC('L','A','G','S'),ICMODE_COMPRESS);
	if (l->hic == 0)
		return -1;
	
	DWORD ssize = ICCompressGetFormat(l->hic,NULL,NULL);
	l->bmih = av_malloc(ssize);
	l->bmiho = av_malloc(ssize);
	memset(l->bmih,0,ssize);
	memset(l->bmiho,0,ssize);
	
	l->bmih->biSize = sizeof(BITMAPINFOHEADER);
	l->bmih->biWidth = avctx->width;
	l->bmih->biHeight = avctx->height;
	l->bmih->biPlanes = 1;
	
	switch(avctx->pix_fmt){
		case PIX_FMT_YUV420P:
			l->bmih->biBitCount = 12;
			l->bmih->biCompression = mmioFOURCC('Y','V','1','2');
			l->y = l->bmih->biWidth * l->bmih->biHeight;
			l->u = l->v = l->y / 4;
			break;
		default:
			return -1;
	}
	
	l->bmih->biSizeImage = l->bmih->biWidth * l->bmih->biHeight * l->bmih->biBitCount / 8;
	
	DWORD res = ICCompressGetFormat(l->hic,l->bmih,l->bmiho);
	if (res != ICERR_OK)
		return res;
	
	res = ICCompressBegin(l->hic,l->bmih,l->bmiho);
	if (res != ICERR_OK)
		return res;
	
	l->cdata = av_malloc(l->bmih->biSizeImage);
	l->odata = av_malloc(max(l->bmiho->biSizeImage,ICCompressGetSize(l->hic,l->bmih,l->bmiho)));
	
	return 0;
}

static av_cold int lag_encode_end(AVCodecContext *avctx) {
	LagarithEncContext *l = avctx->priv_data;
	ICCompressEnd(l->hic);
	ICClose(l->hic);
	av_freep(&l->bmih);
	av_freep(&l->bmiho);
	av_freep(&l->cdata);
	av_freep(&l->odata);
	return 0;
}

static int lag_encode_frame(AVCodecContext *avctx, AVPacket *avpkt, const AVFrame *frame, int *got_packet_ptr) {
	LagarithEncContext *l = avctx->priv_data;
	DWORD flags;
	int ret,i;
	
	switch(avctx->pix_fmt) {
		case PIX_FMT_YUV420P:
			for(i=0;i<avctx->height/2;i++) {
				memcpy(l->cdata+avctx->width*i,frame->data[0]+frame->linesize[0]*i,avctx->width);
				memcpy(l->cdata+l->y+avctx->width*i/2,frame->data[2]+frame->linesize[2]*i,avctx->width/2);
				memcpy(l->cdata+l->y+l->v+avctx->width*i/2,frame->data[1]+frame->linesize[1]*i,avctx->width/2);
			}
			for(;i<avctx->height;i++)
				memcpy(l->cdata+avctx->width*i,frame->data[0]+frame->linesize[0]*i,avctx->width);
			break;
		default:
			return -1;
	}
	
	//lagarith use only lpbiOutput(3),lpData(4),lpbiInput(5),lpBits(6),lpdwFlags(8),(lFrameNum(9))
	ret = ICCompress(l->hic,frame->key_frame?ICCOMPRESS_KEYFRAME:0,l->bmiho,l->odata,l->bmih,l->cdata,NULL,&flags,frame->coded_picture_number,0,0,NULL,NULL);
	if(ret != ICERR_OK)
		return ret;
	
	ret = ff_alloc_packet2(avctx,avpkt,l->bmiho->biSizeImage);
	if(ret < 0)
		return ret;
	
	memcpy(avpkt->data,l->odata,l->bmiho->biSizeImage);
	
	if(flags & AVIIF_KEYFRAME)
		avpkt->flags |= AV_PKT_FLAG_KEY;
	
	*got_packet_ptr = 1;
	
	return 0;
}

AVCodec ff_lagarith_encoder = {
    .name           = "lagarith",
    .type           = AVMEDIA_TYPE_VIDEO,
    .id             = AV_CODEC_ID_LAGARITH,
    .priv_data_size = sizeof(LagarithEncContext),
    .init           = lag_encode_init,
    .close          = lag_encode_end,
    .encode2        = lag_encode_frame,
    .long_name      = NULL_IF_CONFIG_SMALL("Lagarith lossless"),
    .pix_fmts       = pix_fmts,
};
