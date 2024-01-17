#ifndef RINGBUFFER_H
#define RINGBUFFER_H
typedef struct RingBufferState {
	int length;
	int fillLength;
	int currentPos;
	int *buffer;
} RingBufferState;

void ringBufferInit(RingBufferState *s, const int length, int *buffer);
void ringBufferAppend(RingBufferState *s, const int item);
void ringBufferClear(RingBufferState *s);
int ringBufferGet(RingBufferState *s, const int index);
int ringBufferAverage(RingBufferState *s);
int ringBufferAverageVariance(RingBufferState *s, int *variance);
void ringBufferPrint(RingBufferState *s, char *buf);

#endif
