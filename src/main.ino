#include "Arduino.h"
#include "arduino_freertos.h"
#include "avr/pgmspace.h"

#include "BoardConnection.h"
#include "BeamDetection.h"
#include "PeakDetection.h"
#include "MotorControl.h"
#include "BeamPositionControl.h"

#include "PositionMotionMatrices.h"

char inputBuffer[200];
int inputLen;

bool mute = false;

void peakCountTask(void*)
{
	uint32_t peak_cnt;
	for (;;) {
		if (::xQueueReceive(peakCountQueue, (void *) &peak_cnt, portMAX_DELAY) == pdTRUE)
		if (!mute) ::Serial.printf("peak_count %d\r\n", peak_cnt);
	}
}

FLASHMEM __attribute__((noinline)) void setup()
{
	::analogReadResolution(12);

	::pinMode(arduino::LED_BUILTIN, arduino::OUTPUT);

	inputLen = 0;

	::Serial.begin(115200);

	delay(200);

	initBeamLocator();
	setupPSDLogging(true);
	initPeakDetection(20, 10);  // this is optional
	initMotors();
	initBeamPositionControl(&psdReadoutQueue);

	::xTaskCreate(serialInputDispatchTask, "serialInputDispatchTask", 1024, nullptr, 2, nullptr);
	::xTaskCreate(peakCountTask, "peakCountTask", 128, nullptr, 2, nullptr);

	::vTaskStartScheduler();
}

void loop() { }

static void serialInputDispatchTask(void*)
{
        char c;
        int arg1, arg2, arg3, arg4;
        float arg5, arg6, arg7, arg8;
        while (true) {
                if(!::Serial.available()) {
                        taskYIELD();
                        continue;
                }

                c = Serial.read();
                if (c != '\n' && c != '\r' && inputLen < 200 - 2) {
                        inputBuffer[inputLen] = c;
                        inputBuffer[inputLen + 1] = '\0';
                        ++inputLen;
                        continue;
                } else {
                        inputBuffer[inputLen] = '\0';
                        inputLen = 0;
                }

                if (strcmp(inputBuffer, "mute") == 0) {
			setupPSDLogging(false);
			mute = true;
		} else if (strcmp(inputBuffer, "unmute") == 0) {
			mute = false;
			setupPSDLogging(true);
		} else if (strcmp(inputBuffer, "pp") == 0) {
			printLastBeamPosition();
		} else if (sscanf(inputBuffer, "m %d %d", &arg1, &arg2) == 2) {
                        ::Serial.printf("] Motor %d move step %d\r\n", arg1, arg2);

                        if (arg2 == 0) continue;

                        switch (arg1) {
                                case 1: moveMotor(MIRROR1_X_MOTOR, (arg2 > 0), 800, abs(arg2)); break;
                                case 2: moveMotor(MIRROR1_Y_MOTOR, (arg2 > 0), 800, abs(arg2)); break;
                                case 3: moveMotor(MIRROR2_X_MOTOR, (arg2 > 0), 800, abs(arg2)); break;
                                case 4: moveMotor(MIRROR2_Y_MOTOR, (arg2 > 0), 800, abs(arg2)); break;
                                default: break;
                        }
                } else if (sscanf(inputBuffer, "mm %d %d %d %d", &arg1, &arg2, &arg3, &arg4) == 4) {
                        ::Serial.printf("] Motor move step (%d, %d, %d, %d) \r\n", arg1, arg2, arg3, arg4);

                        IntVector4 delta_m = {{ arg1, arg2, arg3, arg4 }};
			moveMotors(&delta_m);
                } else if (sscanf(inputBuffer, "mp %f %f %f %f", &arg5, &arg6, &arg7, &arg8) == 4) {
                        ::Serial.printf("] Motor move to position (%.4f, %.4f, %.4f, %.4f) \r\n", arg5, arg6, arg7, arg8);

                        FloatVector4 dest_p = {{ arg5, arg6, arg7, arg8 }};
			moveBeamToPosition(&dest_p);
                } else {
			::Serial.println("?");
		}
        }
}
