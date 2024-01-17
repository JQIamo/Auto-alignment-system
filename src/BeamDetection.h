#ifndef BEAMDETECTION_H_
#define BEAMDETECTION_H_

#include "queue.h"

#include "BoardConnection.h"

#define PSD_READ_AVERAGE_COUNT 800
#define PSD_READ_DELAY 2
#define PSD_COUNT 2

extern ::QueueHandle_t psdReadoutQueue;

typedef struct {
	float x;
	float y;
} Point;

typedef struct {
	uint32_t y1;
	uint32_t x1;
	uint32_t y2;
	uint32_t x2;
} PSDData;

typedef struct {
	uint32_t y1;
	uint32_t x1;
	uint32_t y2;
	uint32_t x2;
	uint32_t y1_var;
	uint32_t x1_var;
	uint32_t y2_var;
	uint32_t x2_var;
} PSDAvgData;

typedef struct {
	PSDPin pins;
	PSDOffset offset;
	PSDReliableRange range;
} PSD;

enum PSDConversionError {
	NO_ERROR = 0,
	INTENSITY_TOO_LOW = 1,
	INTENSITY_TOO_HIGH = 2
};

const PSD psds[] = {
	{.pins = PSD1_PIN, .offset = PSD1_OFFSET, .range = PSD1_RANGE},
	{.pins = PSD2_PIN, .offset = PSD2_OFFSET, .range = PSD2_RANGE}
};

void initBeamLocator();
PSDConversionError convertPSDReadingtoPosition(const uint8_t detector, const PSDData *d, Point *p);
PSDConversionError convertPSDReadingtoPosition(const uint8_t detector, const PSDAvgData *ad, Point *p, Point *var);
void printLastPSDReading();
void printLastBeamPosition();
void setupPSDLogging(bool enabled);

#endif
