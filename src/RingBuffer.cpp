#include "stdio.h"
#include "RingBuffer.h"

inline int __square(int num) {
    // handle negative input
    if (num < 0)
        num = -num;
 
    // Initialize power of 2 and result
    int power = 0, result = 0;
    int temp = num;
 
    while (temp) {
        if (temp & 1) {
            // result=result+(num*(2^power))
            result += (num << power);
        }
        power++;
 
        // temp=temp/2
        temp = temp >> 1;
    }
 
    return result;
}

void ringBufferInit(RingBufferState *s, const int length, int *buffer) {
	s->fillLength = 0;
	s->currentPos = 0;
	s->length = length;
	s->buffer = buffer;
}

void ringBufferAppend(RingBufferState *s, int item){
	if (s->fillLength < s->length) {
		s->buffer[s->currentPos] = item;
		s->fillLength++;
	} else if (s->currentPos < s->length) {
		s->buffer[s->currentPos] = item;
	} else {
		// s->currentPos == s->length
		s->currentPos = 0;
		s->buffer[0] = item;
	}

	s->currentPos++;
}

void ringBufferClear(RingBufferState *s){
	s->fillLength = 0;
	s->currentPos = 0;
}

int ringBufferGet(RingBufferState *s, const int index){
	if (!s->fillLength) return 0;

	return s->buffer[(s->currentPos + 1 + (index % s->fillLength)) % s->fillLength];
}

int ringBufferAverage(RingBufferState *s){
	if (!s->fillLength) return 0;

	int sum = 0;
	for (int i=0; i < s->fillLength; ++i) {
		sum += s->buffer[i];
	}

	return sum / s->fillLength;
}

int ringBufferAverageVariance(RingBufferState *s, int *variance){
	int sum, avg, var_sum;
	if (!s->fillLength) return 0;

	sum = 0;
	for (int i=0; i < s->fillLength; ++i) {
		sum += s->buffer[i];
	}

	avg = sum / s->fillLength;

	var_sum = 0;
	for (int i=0; i < s->fillLength; ++i) {
		var_sum += __square(s->buffer[i] - avg);
	}

	*variance = var_sum / s->fillLength;

	return avg;
}

void ringBufferPrint(RingBufferState *s, char *buf){
	int pos = 0;

	for (int i = s->currentPos; i < s->currentPos + s->fillLength; ++i) {
		pos += sprintf(buf + pos, "%d, ", s->buffer[(i < s->fillLength) ? i : i - s->fillLength]);
	}

	buf[pos - 2] = '\0';
}
