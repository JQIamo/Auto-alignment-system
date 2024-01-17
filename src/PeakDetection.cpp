#include <Arduino.h>
#include "arduino_freertos.h"
#include "avr/pgmspace.h"
#include "semphr.h"
#include "queue.h"

#include "BoardConnection.h"
#include "PeakDetection.h"
#include "RingBuffer.h"

// #define PEAK_ADC_USE_COMPARISON
// #define PEAK_ADC_DEBUG

::QueueHandle_t peakCountQueue;

uint32_t peak_height_thres_1;
uint32_t peak_height_thres_2;

int _adc_data_buf_arr[5];
RingBufferState adc_data_buf;

volatile uint32_t peak_ctr = 0;
volatile bool comp_triggered = false;

volatile uint32_t _adc_data;

bool before_peak = true;

#ifdef PEAK_ADC_DEBUG
volatile bool led;
::QueueHandle_t _peak_adc_queue;

static void _serialDebugOutput(void*)
{
	uint32_t data;

	for (;;) {
		if(::xQueueReceive(_peak_adc_queue, (void *) &data, portMAX_DELAY) == pdTRUE) {
			::Serial.printf("[peak %u] ADC reading: %u\r\n", peak_ctr, data);
		}
	}
}
#endif

static void isrTrigHandler()
{
	::BaseType_t higherProprityTaskWoken = pdFALSE;

	::xQueueOverwriteFromISR(peakCountQueue, (void *) &peak_ctr, &higherProprityTaskWoken);
	peak_ctr = 0;
	before_peak = true;
	ringBufferClear(&adc_data_buf);

#ifdef PEAK_ADC_DEBUG
	led = !led;
	::digitalWriteFast(arduino::LED_BUILTIN, led);
#endif

	portYIELD_FROM_ISR(higherProprityTaskWoken);
}

static void isrADCHandler() {
	_adc_data = (!(PEAK_DET_ADC_CH & 0x80)) ? ADC1_R0 : ADC2_R0;
	ringBufferAppend(&adc_data_buf, _adc_data);

#ifdef PEAK_ADC_DEBUG
	::BaseType_t higherProprityTaskWoken = pdFALSE;
	::xQueueOverwriteFromISR(_peak_adc_queue, (void *) &_adc_data, &higherProprityTaskWoken);
#endif

	if (adc_data_buf.fillLength == 5) {
		uint32_t yn2 = ringBufferGet(&adc_data_buf, 0);
		uint32_t yn1 = ringBufferGet(&adc_data_buf, 1);
		uint32_t yp1 = ringBufferGet(&adc_data_buf, 3);
		uint32_t yp2 = ringBufferGet(&adc_data_buf, 4);

		int32_t derv = ((yp2 << 1) + yp1 - yn1 - (yn2 << 1));

		if (before_peak && derv <= 0) {
			before_peak = false;
			peak_ctr++;
		} else if (!before_peak && derv >= 0) {
			before_peak = true;
		}
	}

	if (!comp_triggered && _adc_data > peak_height_thres_1) {
		comp_triggered = true;

#ifdef PEAD_ADC_USE_COMPARISON
		if (!(PEAK_DET_ADC_CH & 0x80)) {
			// Disable comparison mode once triggered
			ADC1_GC &= ~ADC_GC_ACFE;
			ADC1_HC0 = PEAK_DET_ADC_CH;
			ADC1_HC0 |= ADC_HC_AIEN;
		} else {
			ADC2_GC &= ~ADC_GC_ACFE;
			ADC2_HC0 = PEAK_DET_ADC_CH & 0x7f;
			ADC2_HC0 |= ADC_HC_AIEN;
		}
#endif
	}


	if (_adc_data < peak_height_thres_2) {
		comp_triggered = false;

#ifdef PEAD_ADC_USE_COMPARISON
		if (!(PEAK_DET_ADC_CH & 0x80)) {
			// Enable comparison mode once triggered
			ADC1_CV = ADC_CV_CV1(peak_height_thres_1);
			ADC1_GC |= ADC_GC_ACFE;
			ADC1_HC0 = PEAK_DET_ADC_CH;
			ADC1_HC0 |= ADC_HC_AIEN;
		} else {
			ADC2_CV = ADC_CV_CV2(10);
			ADC2_GC |= ADC_GC_ACFE;
			ADC2_HC0 = PEAK_DET_ADC_CH & 0x7f;
			ADC2_HC0 |= ADC_HC_AIEN;
		}
#endif
	}

#ifdef PEAK_ADC_DEBUG
	portYIELD_FROM_ISR(higherProprityTaskWoken);
#endif
}

inline void setPeakDetectionThres(uint32_t peakHeightThres1, uint32_t peakHeightThres2)
{
	peak_height_thres_1 = peakHeightThres1;
	peak_height_thres_2 = peakHeightThres2;
}

FLASHMEM __attribute__((noinline)) void initPeakDetection(uint32_t peakHeightThres1, uint32_t peakHeightThres2)
{
	setPeakDetectionThres(peakHeightThres1, peakHeightThres2);
	peakCountQueue = ::xQueueCreate(1, sizeof(uint32_t));
	ringBufferInit(&adc_data_buf, 5, _adc_data_buf_arr);


#ifdef PEAK_ADC_DEBUG
	_peak_adc_queue = ::xQueueCreate(1, sizeof(uint32_t));
#endif

	if (!(PEAK_DET_ADC_CH & 0x80)) {
#ifdef PEAK_ADC_USE_COMPARISON
		// Init ADC comparison
		ADC1_GC |= ADC_GC_ACFE;
		ADC1_GC |= ADC_GC_ACFGT;
		ADC1_CV = ADC_CV_CV1(peakHeightThres1);
#endif

		// Init ADC IRQ
		attachInterruptVector(IRQ_ADC1, &isrADCHandler);
		NVIC_SET_PRIORITY(IRQ_ADC1, 255);
		NVIC_ENABLE_IRQ(IRQ_ADC1);

		ADC1_HC0 = PEAK_DET_ADC_CH;

		// Start Continuous Mode
		ADC1_GC |= ADC_GC_ADCO;

		__disable_irq();
		ADC1_HC0 |= ADC_HC_AIEN;
		__enable_irq();
	} else {
#ifdef PEAD_ADC_USE_COMPARISON
		// Init ADC comparison
		ADC2_GC |= ADC_GC_ACFE;
		ADC2_GC |= ADC_GC_ACFGT;
		ADC2_CV = ADC_CV_CV2(peakHeightThres1);
#endif

		// Init ADC IRQ
		attachInterruptVector(IRQ_ADC2, &isrADCHandler);
		NVIC_SET_PRIORITY(IRQ_ADC2, 255);
		NVIC_ENABLE_IRQ(IRQ_ADC2);

		ADC2_HC0 = PEAK_DET_ADC_CH & 0x7f;

		// Start Continuous Mode
		ADC2_GC |= ADC_GC_ADCO;

		__disable_irq();
		ADC2_HC0 |= ADC_HC_AIEN;
		__enable_irq();
	}

	attachInterrupt(digitalPinToInterrupt(PEAK_DET_TRIG_PIN), isrTrigHandler, arduino::RISING);

#ifdef PEAK_ADC_DEBUG
	::xTaskCreate(_serialDebugOutput, "_serialDebugOutput", 128, nullptr, 2, nullptr);
#endif
}
