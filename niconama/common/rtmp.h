#pragma once

#include "data_exchange.h"

#ifdef __cplusplus
extern "C" {
#endif

THREAD_RET_TYPE rtmp_loop(void*);
int rtmp_init(void);
int rtmp_write_header(int(*v)(RTMP*),int(*a)(RTMP*));

#ifdef __cplusplus
}
#endif

#define WRITE_1(ptr,a) *ptr = (a)
#define CHG_PTR(ptr,a, b) WRITE_1(ptr,*(((uint8_t*)&(a))+(b)))

#define INV_WRITE_8(ptr,a) CHG_PTR(ptr,a,7);CHG_PTR(ptr,a,6); \
	CHG_PTR(ptr,a,5);CHG_PTR(ptr,a,4); \
	CHG_PTR(ptr,a,3);CHG_PTR(ptr,a,2); \
	CHG_PTR(ptr,a,1);WRITE_1(ptr,*((uint8_t*)&(a)))
#define INV_WRITE_4(ptr,a) CHG_PTR(ptr,a,3);CHG_PTR(ptr,a,2); \
	CHG_PTR(ptr,a,1);WRITE_1(ptr,*((uint8_t*)&(a)))
#define INV_WRITE_2(ptr,a) CHG_PTR(ptr,a,1);WRITE_1(ptr,*((uint8_t*)&(a)))

#define WRITE_8(ptr,a) (*((uint64_t*)ptr)=(a))
#define WRITE_4(ptr,a) (*((uint32_t*)ptr)=(a))
#define WRITE_2(ptr,a) (*((uint16_t*)ptr)=(a))

#define CAST_SHIFT8(a, b) (((uint64_t)(a))<<((b)*8))
#define CAST_SHIFT4(a, b) (((uint32_t)(a))<<((b)*8))
#define CAST_SHIFT2(a, b) (((uint16_t)(a))<<((b)*8))

#define MAKE_8(a,b,c,d,e,f,g,h) ((a)|CAST_SHIFT8(b,1)| \
	CAST_SHIFT8(c,2)|CAST_SHIFT8(d,3)|CAST_SHIFT8(e,4)|CAST_SHIFT8(f,5)| \
	CAST_SHIFT8(g,6)|CAST_SHIFT8(h,7))
#define MAKE_4(a,b,c,d) ((a)|CAST_SHIFT4(b,1)| \
		CAST_SHIFT4(c,2)|CAST_SHIFT4(d,3))
#define MAKE_2(a,b) ((a)|CAST_SHIFT2(b,1))

#define S_WRITE_8(ptr,a,b,c,d,e,f,g,h) WRITE_8(ptr,MAKE_8(a,b,c,d,e,f,g,h))
#define S_WRITE_4(ptr,a,b,c,d) WRITE_4(ptr,MAKE_4(a,b,c,d))
#define S_WRITE_2(ptr,a,b) WRITE_2(ptr,MAKE_2(a,b))

#define SETDATAFRAME_WRITE(ptr) \
	S_WRITE_8(ptr,2,0,13,'@','s','e','t','D'); ptr+=8; \
	S_WRITE_8(ptr,'a','t','a','F','r','a','m','e'); ptr+=8
#define ONMETADATA_WRITE(ptr) \
	S_WRITE_8(ptr,2,0,10,'o','n','M','e','t'); ptr+=8; \
	S_WRITE_8(ptr,'a','d','a','t','a',8,0,0); ptr+=6
#define ENDOFECMA_WRITE(ptr) S_WRITE_4(ptr,0,0,9,0);ptr+=3
#define DURATION_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,8,'d','u','r','a','t','i'); ptr+=8; \
	S_WRITE_4(ptr,'o','n',0,0); ptr+=3; \
	INV_WRITE_8(ptr,d); ptr+=8
#define FILESIZE_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,8,'f','i','l','e','s','i'); ptr+=8; \
	S_WRITE_4(ptr,'z','e',0,0); ptr+=3; \
	INV_WRITE_8(ptr,d); ptr+=8
#define VIDEOCODECID_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,12,'v','i','d','e','o','c'); ptr+=8; \
	S_WRITE_8(ptr,'o','d','e','c','i','d',0,0); ptr+=7; \
	INV_WRITE_8(ptr,d); ptr+=8
#define VIDEODATARATE_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,13,'v','i','d','e','o','d'); ptr+=8; \
	S_WRITE_8(ptr,'a','t','a','r','a','t','e',0); ptr+=8; \
	INV_WRITE_8(ptr,d); ptr+=8
#define WIDTH_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,5,'w','i','d','t','h',0); ptr+=8; \
	INV_WRITE_8(ptr,d); ptr+=8
#define HEIGHT_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,6,'h','e','i','g','h','t'); ptr+=8; \
	*ptr++=0; \
	INV_WRITE_8(ptr,d); ptr+=8
#define FRAMERATE_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,9,'f','r','a','m','e','r'); ptr+=8; \
	S_WRITE_4(ptr,'a','t','e',0); ptr+=4; \
	INV_WRITE_8(ptr,d); ptr+=8
#define AUDIOCODECID_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,12,'a','u','d','i','o','c'); ptr+=8; \
	S_WRITE_8(ptr,'o','d','e','c','i','d',0,0); ptr+=7; \
	INV_WRITE_8(ptr,d); ptr+=8
#define AUDIODATARATE_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,13,'a','u','d','i','o','d'); ptr+=8; \
	S_WRITE_8(ptr,'a','t','a','r','a','t','e',0); ptr+=8; \
	INV_WRITE_8(ptr,d); ptr+=8
#define AUDIOSAMPLERATE_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,15,'a','u','d','i','o','s'); ptr+=8; \
	S_WRITE_8(ptr,'a','m','p','l','e','r','a','t'); ptr+=8; \
	S_WRITE_2(ptr,'e',0); ptr+=2; \
	INV_WRITE_8(ptr,d); ptr+=8
#define AUDIOSAMPLESIZE_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,15,'a','u','d','i','o','s'); ptr+=8; \
	S_WRITE_8(ptr,'a','m','p','l','e','s','i','z'); ptr+=8; \
	S_WRITE_2(ptr,'e',0); ptr+=2; \
	INV_WRITE_8(ptr,d); ptr+=8
#define STEREO_WRITE(ptr,b) \
	S_WRITE_8(ptr,0,6,'s','t','e','r','e','o'); ptr+=8; \
	*ptr++=1; \
	*ptr++=b
#define AUDIODELAY_WRITE(ptr,d) \
	S_WRITE_8(ptr,0,10,'a','u','d','i','o','d'); ptr+=8; \
	S_WRITE_8(ptr,'e','l','a','y',0,0,0,0); ptr+=5; \
	INV_WRITE_8(ptr,d); ptr+=8
#define CREATIONDATE_WRITE(ptr) \
	S_WRITE_8(ptr,0,12,'c','r','e','a','r','i'); ptr+=8; \
	S_WRITE_8(ptr,'o','n','d','a','t','e',2,0); ptr+=7
#define CANSEEKTOEND_WRITE(ptr,b) \
	S_WRITE_8(ptr,0,12,'c','a','n','S','e','e'); ptr+=8; \
	S_WRITE_8(ptr,'k','T','o','E','n','d',1,b); ptr+=8

#define RTMP_SET_SIZE(p, size) p->m_nBodySize=size
#define RTMP_SET_INT_SIZE(r, size) r->m_write.m_nBodySize=size

#define RTMP_SEND(r, p, ret) \
		p->m_nInfoField2 = r->m_stream_id; \
		ret=!RTMP_SendPacket(r, p, FALSE); \
		free(p->m_body - RTMP_MAX_HEADER_SIZE); \
		free(p); p = NULL
#define RTMP_INT_SEND(r, ret) \
		ret=!RTMP_SendPacket(r, &r->m_write, FALSE); \
		free(r->m_write.m_body - RTMP_MAX_HEADER_SIZE)

#define RTMP_GET_INT_BUFFER(r,size,time,hdrType,pktType) \
		r->m_write.m_headerType = hdrType; \
		r->m_write.m_packetType = pktType; \
		r->m_write.m_hasAbsTimestamp = TRUE; \
		r->m_write.m_nChannel = 4; \
		r->m_write.m_nTimeStamp = time; \
		r->m_write.m_nInfoField2 = r->m_stream_id; \
		r->m_write.m_body = ((char*)calloc(1, size + RTMP_MAX_HEADER_SIZE)) \
							+ RTMP_MAX_HEADER_SIZE; \
		r->m_write.m_nBytesRead = 0
#define RTMP_GET_BUFFER(p,size,time,hdrType,pktType) \
		p = (RTMPPacket*)malloc(sizeof(RTMPPacket)); \
		p->m_headerType = hdrType; \
		p->m_packetType = pktType; \
		p->m_hasAbsTimestamp = TRUE; \
		p->m_nChannel = 4; \
		p->m_nTimeStamp = time; \
		p->m_body = ((char*)calloc(1, size + RTMP_MAX_HEADER_SIZE)) \
							+ RTMP_MAX_HEADER_SIZE; \
		p->m_nBytesRead = 0

#define RTMP_GET_AAC_BUFFER(p, size, frame) \
		RTMP_GET_BUFFER(p,size,frame*10/441,frame?RTMP_PACKET_SIZE_MEDIUM: \
							RTMP_PACKET_SIZE_LARGE,RTMP_PACKET_TYPE_AUDIO); \
		p->m_body[0] = 0xAF; p->m_body[1] = 0x01

