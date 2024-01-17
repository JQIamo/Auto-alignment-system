#ifndef PEAKDETECTION_H_
#define PEAKDETECTION_H_

#include "queue.h"

extern ::QueueHandle_t peakCountQueue;

void initPeakDetection(uint32_t peakHeightThres1, uint32_t peakHeightThres2);
void setPeakDetectionThres(uint32_t peakHeightThres1, uint32_t peakHeightThres2);

#endif
