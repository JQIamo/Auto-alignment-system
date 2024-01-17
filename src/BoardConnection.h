#ifndef BOARDCONNECTION_H_
#define BOARDCONNECTION_H_

// #define DETECTOR1 1
// #define DETECTOR2 2
//
#define DETECTOR1 0
#define DETECTOR2 1

#define D1_Y1_PIN 16
#define D1_X1_PIN 17
#define D1_Y2_PIN 18
#define D1_X2_PIN 19

#define D2_Y1_PIN 20
#define D2_X1_PIN 21
#define D2_Y2_PIN 22
#define D2_X2_PIN 23

#define MOTOR_DIRECTION_PIN 10

//pulse rate can be between 0 and 2kHz
#define MOTOR_PULSE_PIN 11
#define MOTOR_IDLE_PIN 12
#define RELAY1_PIN 24
#define RELAY2_PIN 26
#define RELAY3_PIN 25

#define PEAK_DET_TRIG_PIN 37
#define PEAK_DET_ADC_PIN 38 // A14
#define PEAK_DET_ADC_CH (128+1) // A14, on ADC2

typedef struct {
	uint8_t y1_pin;
	uint8_t x1_pin;
	uint8_t y2_pin;
	uint8_t x2_pin;
} PSDPin;

typedef struct {
	uint32_t y1_offset;
	uint32_t x1_offset;
	uint32_t y2_offset;
	uint32_t x2_offset;
} PSDOffset;

typedef struct {
	uint32_t x1_min;
	uint32_t x1_max;
	uint32_t x2_min;
	uint32_t x2_max;
	uint32_t y1_min;
	uint32_t y1_max;
	uint32_t y2_min;
	uint32_t y2_max;
} PSDReliableRange;

const PSDPin PSD1_PIN = {
	.y1_pin = 16, 
	.x1_pin = 17, 
	.y2_pin = 18, 
	.x2_pin = 19 
};

const PSDPin PSD2_PIN = {
	.y1_pin = 20, 
	.x1_pin = 21, 
	.y2_pin = 22, 
	.x2_pin = 23 
};

const PSDOffset PSD1_OFFSET = {
	.y1_offset = 3102, 
	.x1_offset = 0, 
	.y2_offset = 3085, 
	.x2_offset = 64 
};

const PSDOffset PSD2_OFFSET = {
	.y1_offset = 3126, 
	.x1_offset = 0, 
	.y2_offset = 3179, 
	.x2_offset = 0 
};

const PSDReliableRange PSD1_RANGE = {
	.x1_min = 500,
	.x1_max = 4000,
	.x2_min = 500,
	.x2_max = 4000,
	.y1_min = 100,
	.y1_max = 2600,
	.y2_min = 100,
	.y2_max = 2600,
};

const PSDReliableRange PSD2_RANGE = {
	.x1_min = 500,
	.x1_max = 4000,
	.x2_min = 500,
	.x2_max = 4000,
	.y1_min = 100,
	.y1_max = 2600,
	.y2_min = 100,
	.y2_max = 2600,
};

#define MIRROR1_X_MOTOR 0
#define MIRROR1_Y_MOTOR 1
#define MIRROR2_X_MOTOR 2
#define MIRROR2_Y_MOTOR 3

typedef struct MotorAddr {
	bool relay1;
	bool relay2;
	bool relay3;
} MotorAddr;

const MotorAddr motor1Addr = {
	.relay1 = false,
	.relay2 = false,
	.relay3 = false
};

const MotorAddr motor2Addr = {
	.relay1 = true,
	.relay2 = false,
	.relay3 = false
};

const MotorAddr motor3Addr = {
	.relay1 = false,
	.relay2 = true,
	.relay3 = false
};

const MotorAddr motor4Addr = {
	.relay1 = true,
	.relay2 = false,
	.relay3 = true
};

#endif
