#ifndef MOTORCONTROL_H_
#define MOTORCONTROL_H_

#include "BoardConnection.h"
#include "PositionMotionMatrices.h"

#define MOTOR_CNT 4

const MotorAddr motorAddrs[] = { motor1Addr, motor2Addr, motor3Addr, motor4Addr };

void moveMotor(uint8_t motor, bool clockwise, uint32_t rate, uint32_t duration);
void moveMotors(const IntVector4 *motor_steps);
bool switchMotor(uint8_t motor);
void initMotors();

#endif
