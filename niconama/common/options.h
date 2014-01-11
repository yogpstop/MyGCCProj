#define AUDIO_ENABLE 1
#define AUDIO_BITRATE 96000
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNELS 2
#define AUDIO_BIT_DEPTH 32
#define AUDIO_BYTE_PER_FULL_SAMPLE (AUDIO_CHANNELS*AUDIO_BIT_DEPTH/8)
#define VIDEO_ENABLE 0
#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 360
#define VIDEO_FRAMERATE 20
#define VIDEO_THREADS 3
#define VIDEO_PRESET "slower"
#define VIDEO_TUNE
#define VIDEO_PROFILE "main"
#define FDK_AAC_AFTERBURNER 1
#define FDK_AAC_SBR 1
#define FDK_AAC_PS 1
#define FDK_AAC_VBR 0

/*"ultrafast""superfast""veryfast""faster""fast""medium""slow""slower""veryslow""placebo"
"film""animation""grain""stillimage""psnr""ssim""fastdecode""zerolatency""touhou"
"baseline""main""high""high10""high422""high444"*/