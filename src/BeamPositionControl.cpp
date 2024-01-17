#include "Arduino.h"
#include "math.h"
#include "arduino_freertos.h"
#include "queue.h"

#include "BeamPositionControl.h"
#include "BeamDetection.h"
#include "MotorControl.h"

bool running;
bool can_run;

::QueueHandle_t *psd_queue;

Point p1, p2, p1_var, p2_var;

void receivePSDDataTask(void*)
{
	PSDAvgData psd_readouts[2];

	PSDConversionError err1, err2;

	for (;;) {
		if (::xQueueReceive(*psd_queue, (void *) &psd_readouts, portMAX_DELAY) == pdTRUE) {
			const PSDAvgData d1 = psd_readouts[0];
			const PSDAvgData d2 = psd_readouts[1];

			err1 = convertPSDReadingtoPosition(0, &d1, &p1, &p1_var);
			err2 = convertPSDReadingtoPosition(1, &d2, &p2, &p2_var);

			can_run = (err1 == PSDConversionError::NO_ERROR && err2 == PSDConversionError::NO_ERROR);

                        // if (err1 == PSDConversionError::NO_ERROR) {
                        //         ::Serial.printf("> p1: (%.2f, %.2f), p1_var: (%.2f, %.2f) \r\n",
                        //                         p1.x, p1.y, p1_var.x, p1_var.y);
                        // } else {
                        //         ::Serial.printf("> p1: CONVERSION ERROR \r\n");
                        // }
 
                        // if (err2 == PSDConversionError::NO_ERROR) {
                        //         ::Serial.printf("> p2: (%.2f, %.2f), p2_var: (%.2f, %.2f) \r\n",
                        //                         p2.x, p2.y, p2_var.x, p2_var.y);
                        // } else {
                        //         ::Serial.printf("> p2: CONVERSION ERROR \r\n");
                        // }
		}
	}
}

void moveBeamToPosition(const FloatVector4 *dest_p) {
	FloatVector4 current_p = {{ p1.x, p1.y, p2.x, p2.y }};
	FloatVector4 delta_p;
	vectorDelta(dest_p, &current_p, &delta_p);
	::Serial.printf("] Delta p (%.5f, %.5f, %.5f, %.5f) \r\n", delta_p.v[0], delta_p.v[1], delta_p.v[2], delta_p.v[3]);

	IntVector4 delta_m = {{ 0, 0, 0, 0 }};
	matrixMul(&pMMatrices[0], &delta_p, &delta_m);
	matrixMul(&pMMatricesSelect(delta_m), &delta_p, &delta_m);

	::Serial.printf("] Motor move step (%d, %d, %d, %d) \r\n", delta_m.v[0], delta_m.v[1], delta_m.v[2], delta_m.v[3]);
	moveMotors(&delta_m);
}


void initBeamPositionControl(::QueueHandle_t *_psd_queue) {
	psd_queue = _psd_queue;
	::xTaskCreate(receivePSDDataTask, "receivePSDDataTask", 512, nullptr, 2, nullptr);
}

