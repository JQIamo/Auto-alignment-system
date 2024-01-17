#include "Arduino.h"
#include "arduino_freertos.h"

#include "MotorControl.h"

uint8_t current_motor;
bool motor_idle, motor_direction;


void moveMotors(const IntVector4 *motor_steps) {
	moveMotor(MIRROR1_X_MOTOR, (motor_steps->v[0] > 0), 800, abs(motor_steps->v[0]));
	moveMotor(MIRROR1_Y_MOTOR, (motor_steps->v[1] > 0), 800, abs(motor_steps->v[1]));
	moveMotor(MIRROR2_X_MOTOR, (motor_steps->v[2] > 0), 800, abs(motor_steps->v[2]));
	moveMotor(MIRROR2_Y_MOTOR, (motor_steps->v[3] > 0), 800, abs(motor_steps->v[3]));
}

void moveMotor(uint8_t motor, bool clockwise, uint32_t frequency, uint32_t step)
{
	::noTone(MOTOR_PULSE_PIN);

	if (motor != current_motor){
		switchMotor(motor);
	}

	if (motor_idle) {
		::digitalWriteFast(MOTOR_IDLE_PIN, arduino::HIGH);
		motor_idle = false;
		::vTaskDelay(pdMS_TO_TICKS(500));
	}

	if (motor_direction != clockwise){
		::digitalWriteFast(MOTOR_DIRECTION_PIN, (clockwise ? arduino::HIGH : arduino::LOW));
		motor_direction = clockwise;
		::vTaskDelay(pdMS_TO_TICKS(5));
	}

	uint32_t duration = 1000 * step / frequency;

	::Serial.printf("] Move m%d, direction %d, rate %d, step %d, duration %d\r\n", motor, clockwise, frequency, step, duration);

	if (duration > 0) {
		::tone(MOTOR_PULSE_PIN, frequency, duration); 
		::vTaskDelay(pdMS_TO_TICKS(duration));
	}
}

bool switchMotor(uint8_t motor)
{
	if (!motor_idle){
		::digitalWriteFast(MOTOR_IDLE_PIN, arduino::LOW);
		motor_idle = true;
		::vTaskDelay(pdMS_TO_TICKS(500));  // 500 ms is required by the datasheet to safely discharge picomotor.
	}

	if (motor >= MOTOR_CNT) return false;

	current_motor = motor;

	MotorAddr ma = motorAddrs[motor];
	::digitalWriteFast(RELAY1_PIN, ma.relay1);
	::digitalWriteFast(RELAY2_PIN, ma.relay2);
	::digitalWriteFast(RELAY3_PIN, ma.relay3);

	return true;
}

FLASHMEM __attribute__((noinline)) void initMotors() 
{
	::pinMode(RELAY1_PIN, arduino::OUTPUT);
	::pinMode(RELAY2_PIN, arduino::OUTPUT);
	::pinMode(RELAY3_PIN, arduino::OUTPUT);

	::pinMode(MOTOR_IDLE_PIN, arduino::OUTPUT);
	::pinMode(MOTOR_DIRECTION_PIN, arduino::OUTPUT);
	::pinMode(MOTOR_PULSE_PIN, arduino::OUTPUT);

	::digitalWriteFast(MOTOR_IDLE_PIN, arduino::LOW);
	motor_idle = true;
	::digitalWriteFast(MOTOR_DIRECTION_PIN, arduino::LOW);
	motor_direction = false;
}
