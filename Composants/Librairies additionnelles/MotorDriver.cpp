#include "MotorDriver.h"
#include <Wire.h>

// *********************************Initialize*********************************
// Initialize I2C with an I2C address you set on Grove - I2C Motor Driver v1.3
// default i2c address: 0x0f
int MotorDriver::begin(unsigned char i2c_add) {
    if (i2c_add > 0x0f) {
        Serial.println("Error! I2C address must be between 0x00 to 0x0F");
        return (-1); // I2C address error
    }
    Wire.begin();
    delayMicroseconds(10000);
    this->_i2c_add = i2c_add;
    // Set default frequence to F_3921Hz
    frequence(F_3921Hz);
    return (0); // OK
}

// *****************************Private Function*******************************
// Set the direction of 2 motors
// _direction: M1CWM2ACW(M1 ClockWise M2 AntiClockWise), M1ACWM2CW, BothClockWise, BothAntiClockWise,
void MotorDriver::direction(unsigned char _direction) {
    Wire.beginTransmission(this->_i2c_add); // begin transmission
    Wire.write(DirectionSet);               // Direction control header
    Wire.write(_direction);                 // send direction control information
    Wire.write(Nothing);                    // need to send this byte as the third byte(no meaning)
    Wire.endTransmission();                 // stop transmitting
    delay(4); 				                // wait
}

// *****************************DC Motor Function******************************
// Set the speed of a motor, speed is equal to duty cycle here
// motor_id: MOTOR1, MOTOR2
// _speed: -100~100, when _speed>0, dc motor runs clockwise; when _speed<0,
// dc motor runs anticlockwise
void MotorDriver::speed(unsigned char motor_id, int _speed) {
    if (motor_id < MOTOR1 || motor_id > MOTOR2) {
        Serial.println("Motor id error! Must be MOTOR1 or MOTOR2");
        return;
    }

    if (motor_id == MOTOR1) {
        if (_speed >= 0) {
            this->_M1_direction = 1;
            _speed = _speed > 100 ? 100 : _speed;
            this->_speed1 = map(_speed, 0, 100, 0, 255);
        } else if (_speed < 0) {
            this->_M1_direction = -1;
            _speed = _speed < -100 ? 100 : -(_speed);
            this->_speed1 = map(_speed, 0, 100, 0, 255);
        }
    } else if (motor_id == MOTOR2) {
        if (_speed >= 0) {
            this->_M2_direction = 1;
            _speed = _speed > 100 ? 100 : _speed;
            this->_speed2 = map(_speed, 0, 100, 0, 255);
        } else if (_speed < 0) {
            this->_M2_direction = -1;
            _speed = _speed < -100 ? 100 : -(_speed);
            this->_speed2 = map(_speed, 0, 100, 0, 255);
        }
    }
    // Set the direction
    if (_M1_direction == 1 && _M2_direction == 1) {
        direction(BothClockWise);
    }
    if (_M1_direction == 1 && _M2_direction == -1) {
        direction(M1CWM2ACW);
    }
    if (_M1_direction == -1 && _M2_direction == 1) {
        direction(M1ACWM2CW);
    }
    if (_M1_direction == -1 && _M2_direction == -1) {
        direction(BothAntiClockWise);
    }
    // send command
    Wire.beginTransmission(this->_i2c_add); // begin transmission
    Wire.write(MotorSpeedSet);              // set pwm header
    Wire.write(this->_speed1);              // send speed of motor1
    Wire.write(this->_speed2);              // send speed of motor2
    Wire.endTransmission();    		        // stop transmitting
    delay(4); 				                // Wait
}

// Set the frequence of PWM(cycle length = 510, system clock = 16MHz)
// F_3921Hz is default
// _frequence: F_31372Hz, F_3921Hz, F_490Hz, F_122Hz, F_30Hz
void MotorDriver::frequence(unsigned char _frequence) {
    if (_frequence < F_31372Hz || _frequence > F_30Hz) {
        Serial.println("frequence error! Must be F_31372Hz, F_3921Hz, F_490Hz, F_122Hz, F_30Hz");
        return;
    }
    Wire.beginTransmission(this->_i2c_add); // begin transmission
    Wire.write(PWMFrequenceSet);            // set frequence header
    Wire.write(_frequence);                 // send frequence
    Wire.write(Nothing);                    // need to send this byte as the third byte(no meaning)
    Wire.endTransmission();                 // stop transmitting
    delay(4); 				                // wait
}

// Stop one motor
// motor_id: MOTOR1, MOTOR2
void MotorDriver::stop(unsigned char motor_id) {
    if (motor_id < MOTOR1 || motor_id > MOTOR2) {
        Serial.println("Motor id error! Must be MOTOR1 or MOTOR2");
        return;
    }
    speed(motor_id, 0);
}

// ***************************Stepper Motor Function***************************
// Drive a stepper motor
// _step: -1024~1024, when _step>0, stepper motor runs clockwise; when _step<0,
// stepper motor runs anticlockwise; when _step is 512, the stepper motor will
// run a complete turn; if step is 1024, the stepper motor will run 2 turns.
//  _type: 0 -> 4 phase stepper motor, default
//         1 -> 2 phase stepper motor
//  _mode: 0 -> compatible mode (_step=1 corresponds 4 steps)
//         1 -> fine mode (_step1 corresponds 1 steps)
void MotorDriver::StepperRun(int _step, int _type, int _mode) {
    int _direction = 1;
    if (_step > 0) {
        _direction = 1; //clockwise
        _step = _step > 1024 ? 1024 : _step;
    } else if (_step < 0) {
        _direction = -1; //anti-clockwise
        _step = _step < -1024 ? 1024 : -(_step);
    }
    this->_speed1 = 255;
    this->_speed2 = 255;
    Wire.beginTransmission(this->_i2c_add); // begin transmission
    Wire.write(MotorSpeedSet);              // set pwm header
    Wire.write(this->_speed1);              // send speed of motor1
    Wire.write(this->_speed2);              // send speed of motor2
    Wire.endTransmission();    		        // stop transmitting
    delay(4); 				                // wait

    if (_type == 1) {
        if (_direction == 1) {				// 2 phase motor
            for (int i = 0; i < _step; i++) {
                if (_mode == 0) {
                    direction(0b0001);
                    direction(0b0101);
                    direction(0b0100);
                    direction(0b0110);
                    direction(0b0010);
                    direction(0b1010);
                    direction(0b1000);
                    direction(0b1001);
                } else {
                    switch (_step_cnt) {
                        case 0 : direction(0b0001); direction(0b0101); break;
                        case 1 : direction(0b0100); direction(0b0110); break;
                        case 2 : direction(0b0010); direction(0b1010); break;
                        case 3 : direction(0b1000); direction(0b1001); break;
                    }
                    _step_cnt = (_step_cnt + 1) % 4;
                }
            }
        } else if (_direction == -1) {
            for (int i = 0; i < _step; i++) {
                if (_mode == 0) {
                    direction(0b1000);
                    direction(0b1010);
                    direction(0b0010);
                    direction(0b0110);
                    direction(0b0100);
                    direction(0b0101);
                    direction(0b0001);
                    direction(0b1001);
                } else {
                    switch (_step_cnt) {
                        case 0 : direction(0b1000); direction(0b1010); break;
                        case 1 : direction(0b0010); direction(0b0110); break;
                        case 2 : direction(0b0100); direction(0b0101); break;
                        case 3 : direction(0b0001); direction(0b1001); break;
                    }
                    _step_cnt = (_step_cnt + 1) % 4;
                }
            }
        }
    } else if (_type == 0) {
        if (_direction == 1) {				// 4 phase motor
            for (int i = 0; i < _step; i++) {
                if (_mode == 0) {
                    direction(0b0001);
                    direction(0b0011);
                    direction(0b0010);
                    direction(0b0110);
                    direction(0b0100);
                    direction(0b1100);
                    direction(0b1000);
                    direction(0b1001);
                } else {
                    switch (_step_cnt) {
                        case 0 : direction(0b0001); direction(0b0011); break;
                        case 2 : direction(0b0010); direction(0b0110); break;
                        case 3 : direction(0b0100); direction(0b1100); break;
                        case 4 : direction(0b1000); direction(0b1001); break;
                    }
                    _step_cnt = (_step_cnt + 1) % 4;
                }
            }
        } else if (_direction == -1) {
            for (int i = 0; i < _step; i++) {
                if (_mode == 0) {
                    direction(0b1000);
                    direction(0b1100);
                    direction(0b0100);
                    direction(0b0110);
                    direction(0b0010);
                    direction(0b0011);
                    direction(0b0001);
                    direction(0b1001);
                } else {
                    switch (_step_cnt) {
                        case 0 : direction(0b1000); direction(0b1100); break;
                        case 1 : direction(0b0100); direction(0b0110); break;
                        case 2 : direction(0b0010); direction(0b0011); break;
                        case 3 : direction(0b0001); direction(0b1001); break;
                    }
                    _step_cnt = (_step_cnt + 1) % 4;
                }
            }
        }
    }
}


MotorDriver Motor;

// End of File
