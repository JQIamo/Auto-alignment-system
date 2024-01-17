#include "Arduino.h"
#include "math.h"
#include "arduino_freertos.h"
#include "avr/pgmspace.h"

#include "BeamDetection.h"

::QueueHandle_t psdReadoutQueue;

PSDAvgData psd_avg_data[PSD_COUNT];

bool psd_log_enabled = true;

uint32_t buf_x1[PSD_COUNT*PSD_READ_AVERAGE_COUNT];
uint32_t buf_x2[PSD_COUNT*PSD_READ_AVERAGE_COUNT];
uint32_t buf_y1[PSD_COUNT*PSD_READ_AVERAGE_COUNT];
uint32_t buf_y2[PSD_COUNT*PSD_READ_AVERAGE_COUNT];

uint32_t calculate_average_variance(uint32_t *buf, size_t len, uint32_t *variance){
	int sum, avg, var_sum;
	if (!len) return 0;

	sum = 0;
	for (size_t i=0; i < len; ++i) {
		sum += buf[i];
	}

	avg = ceil(sum / len);

	var_sum = 0;
	for (size_t i=0; i < len; ++i) {
		int v = buf[i] - avg;
		var_sum += v*v;
	}

	*variance = ceil((float)var_sum / len);

	return avg;
}

bool readPSDData(const uint8_t detector, PSDData *d){
	if (detector >= PSD_COUNT) return false;

 	const PSDPin pins = psds[detector].pins;

 	d->y1 = ::analogRead(pins.y1_pin);
 	d->y2 = ::analogRead(pins.y2_pin);
 	d->x1 = ::analogRead(pins.x1_pin);
 	d->x2 = ::analogRead(pins.x2_pin);

	return true;
}

// For some reason, the internal multiplexing of ADC of Teensy 4.1 introduces
// large noise to the data. This workaround changes the pin mode and reduces
// this noise by an order of magnitude.

inline void _pinModeWorkaround(uint8_t active_pin) {
	for (uint8_t j=0; j<PSD_COUNT; j++){
		const PSDPin pins = psds[j].pins;
		uint8_t mode = (j == active_pin) ? arduino::INPUT : arduino::OUTPUT_OPENDRAIN;
		::pinMode(pins.x1_pin, mode);
		::pinMode(pins.x2_pin, mode);
		::pinMode(pins.y1_pin, mode);
		::pinMode(pins.y2_pin, mode);
	}
}

static void acquirePSDDataTask(void*) {
	PSDData d;

	while (true) {
		for (uint8_t j=0; j<PSD_COUNT; j++) {
			_pinModeWorkaround(j);

			for (size_t i=0; i<PSD_READ_AVERAGE_COUNT; ++i) {
				if (readPSDData(j, &d)) {
					buf_x1[i+j*PSD_READ_AVERAGE_COUNT] = d.x1;
					buf_x2[i+j*PSD_READ_AVERAGE_COUNT] = d.x2;
					buf_y1[i+j*PSD_READ_AVERAGE_COUNT] = d.y1;
					buf_y2[i+j*PSD_READ_AVERAGE_COUNT] = d.y2;
				}
			}
		}

#ifdef PSD_READ_DELAY
			::vTaskDelay(pdMS_TO_TICKS(PSD_READ_DELAY));
#endif

		for (uint8_t j=0; j<PSD_COUNT; j++) {
			psd_avg_data[j].x1 = calculate_average_variance(buf_x1 + j*PSD_READ_AVERAGE_COUNT, PSD_READ_AVERAGE_COUNT, &psd_avg_data[j].x1_var);
			psd_avg_data[j].x2 = calculate_average_variance(buf_x2 + j*PSD_READ_AVERAGE_COUNT, PSD_READ_AVERAGE_COUNT, &psd_avg_data[j].x2_var);
			psd_avg_data[j].y1 = calculate_average_variance(buf_y1 + j*PSD_READ_AVERAGE_COUNT, PSD_READ_AVERAGE_COUNT, &psd_avg_data[j].y1_var);
			psd_avg_data[j].y2 = calculate_average_variance(buf_y2 + j*PSD_READ_AVERAGE_COUNT, PSD_READ_AVERAGE_COUNT, &psd_avg_data[j].y2_var);

			if (psd_log_enabled) {
				::Serial.printf("r%d: %d, %d, %d, %d, var%d: %d, %d, %d, %d\r\n",
						j+1,
						psd_avg_data[j].x1,
						psd_avg_data[j].x2,
						psd_avg_data[j].y1,
						psd_avg_data[j].y2,
						j+1,
						psd_avg_data[j].x1_var,
						psd_avg_data[j].x2_var,
						psd_avg_data[j].y1_var,
						psd_avg_data[j].y2_var);
			}
		}

		::xQueueOverwrite(psdReadoutQueue, (void *) &psd_avg_data);
		taskYIELD();
	}
}

inline PSDConversionError checkPSDReadingRange(const uint8_t detector, const PSDData *d) {
	const PSDReliableRange r = psds[detector].range;

	if ( d->x1 > r.x1_max || d->x2 > r.x2_max || d->y1 < r.y1_min || d->y2 < r.y2_min ) { // y1 and y2 go low when intensity go high
		return PSDConversionError::INTENSITY_TOO_HIGH;
	}

	if ( d->x1 < r.x1_min || d->x2 < r.x2_min || d->y1 > r.y1_max || d->y2 > r.y2_max ) {
		return PSDConversionError::INTENSITY_TOO_LOW;
	}

	return PSDConversionError::NO_ERROR;
}

inline PSDConversionError checkPSDReadingRange(const uint8_t detector, const PSDAvgData *ad) {
	return checkPSDReadingRange(detector, (const PSDData*) ad);
}

PSDConversionError convertPSDReadingtoPosition(const uint8_t detector, const PSDData *d, Point *p){
	const PSDOffset offset = psds[detector].offset;

	PSDConversionError err;
	err = checkPSDReadingRange(detector, d);

	if (err != PSDConversionError::NO_ERROR) return err;

	p->x = (float)((d->x1 - offset.x1_offset) - (d->x2 - offset.x2_offset)) / (float)((d->x1 - offset.x1_offset) + (d->x2 - offset.x2_offset));
	p->y = (float)((d->y1 - offset.y1_offset) - (d->y2 - offset.y2_offset)) / (float)((d->y1 - offset.y1_offset) + (d->y2 - offset.y2_offset));

	return PSDConversionError::NO_ERROR;
}

inline float quotient_err(int32_t x1, int32_t x2, uint32_t var1, uint32_t var2, float *err) {
	int32_t _sum = x1 + x2; // uint32_t BAD!
	int32_t _sub = x1 - x2;

	float x = (float) _sub / _sum;

	if (err) {
		*err = fabs(x * sqrt((float)(var1 + var2) * (1.0 / (_sum*_sum) + 1.0 / (_sub*_sub))));
	}

	return x;
}

PSDConversionError convertPSDReadingtoPosition(const uint8_t detector, const PSDAvgData *ad, Point *p, Point *var){
	const PSDOffset offset = psds[detector].offset;
	PSDConversionError err;
	err = checkPSDReadingRange(detector, ad);

	if (err != PSDConversionError::NO_ERROR) return err;

	p->x = quotient_err(ad->x1 - offset.x1_offset,
			ad->x2 - offset.x2_offset,
			ad->x1_var,
			ad->x2_var,
			var ? &(var->x) : NULL);

	p->y = quotient_err(ad->y1 - offset.y1_offset,
			ad->y2 - offset.y2_offset,
			ad->y1_var,
			ad->y2_var,
			var ? &(var->y) : NULL);

	return PSDConversionError::NO_ERROR;
}

void printLastPSDReading() {
	for (uint8_t j=0; j<PSD_COUNT; j++) {
		PSDAvgData d = psd_avg_data[j];

		::Serial.printf("] r%d: %d, %d, %d, %d, var1: %d, %d, %d, %d \r\n",
				j+1, d.x1, d.x2, d.y1, d.y2, d.x1_var, d.x2_var, d.y1_var, d.y2_var);
	}
}

void printLastBeamPosition() {
	for (uint8_t j=0; j<PSD_COUNT; j++) {
		PSDAvgData d = psd_avg_data[j];

		Point p, p_var;
		PSDConversionError err = convertPSDReadingtoPosition(j, &d, &p, &p_var);
		if (err == PSDConversionError::NO_ERROR) {
			::Serial.printf("] p%d: (%.4f, %.4f), p1_var: (%.4f, %.4f) \r\n",
					j+1, p.x, p.y, p_var.x, p_var.y);
		} else {
			::Serial.printf("] p%d: CONVERSION ERROR \r\n", j+1);
		}
	}
}

void setupPSDLogging(bool enabled) {
	psd_log_enabled = enabled;
}

FLASHMEM __attribute__((noinline)) void initBeamLocator() {
	psdReadoutQueue = ::xQueueCreate(1, PSD_COUNT*sizeof(PSDAvgData));

	for (uint8_t j=0; j<PSD_COUNT; j++) {
		const PSDPin pins = psds[j].pins;

		::pinMode(pins.y1_pin, arduino::INPUT);
		::pinMode(pins.y2_pin, arduino::INPUT);
		::pinMode(pins.x1_pin, arduino::INPUT);
		::pinMode(pins.x2_pin, arduino::INPUT);
	}

	::xTaskCreate(acquirePSDDataTask, "acquirePSDDataTask", 512, nullptr, 2, nullptr);
}
