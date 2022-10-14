#ifndef _TS_MUXER_H_
#define _TS_MUXER_H_

void* tsmuxer_init (char *file);
void  tsmuxer_exit (void *ctx);
void  tsmuxer_video(void *ctx, unsigned char *buf1, int len1, unsigned char *buf2, int len2, int key, unsigned pts);
void  tsmuxer_audio(void *ctx, unsigned char *buf1, int len1, unsigned char *buf2, int len2, int key, unsigned pts);

#endif
