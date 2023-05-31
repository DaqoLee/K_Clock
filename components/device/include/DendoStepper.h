#pragma once

/*  ESP-IDF library for bipolar stepper motor drivers with STEP/DIR interface
    Copyright (C) 2022 Denis Voltmann

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DENDOSTEPPER_H
#define DENDOSTEPPER_H

#include "stdint.h"
#include "stdio.h"
#include <cstring>
#include "driver/timer.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "math.h"

//#define STEP_DEBUG

#define NS_TO_T_TICKS(x) (x)
#define TIMER_F 1000000ULL
#define TICK_PER_S TIMER_F

enum motor_status
{
    DISABLED,
    IDLE,
    ACC,
    COAST,
    DEC
};

enum dir
{
    CW,
    CCW
};

enum microStepping_t
{
    MICROSTEP_1 = 0x1,
    MICROSTEP_2,
    MICROSTEP_4 = 0x4,
    MICROSTEP_8 = 0x8,
    MICROSTEP_16 = 0x10,
    MICROSTEP_32 = 0x20,
    MICROSTEP_64 = 0x40,
    MICROSTEP_128 = 0x80,
    MICROSTEP_256 = 0x100,
};

/**
 * @brief Configuration structure
 */
typedef struct
{
    uint8_t stepPin;           /** step signal pin */
    uint8_t dirPin;            /** dir signal pin */
    uint8_t enPin;             /** enable signal pin */
    uint8_t ms1Pin;            /** ms1 signal pin */
    uint8_t ms2Pin;             /** ms2 signal pin */
    timer_group_t timer_group; /** timer group, useful if we are controlling more than 2 steppers */
    timer_idx_t timer_idx;     /** timer index, useful if we are controlling 2steppers */
    microStepping_t miStep;    /** microstepping configured on driver - used in distance calculation */
    float stepAngle;           /** one step angle in degrees (usually 1.8deg), used in steps per rotation calculation */
} DendoStepper_config_t;

typedef struct
{
    uint32_t stepInterval = 40000; // step interval in ns/25
    double targetSpeed = 0;        // target speed in steps/s
    double currentSpeed = 0;
    double accInc = 0;
    double decInc = 0;
    uint32_t stepCnt = 0;   // step counter
    uint32_t accEnd;        // when to end acc and start coast
    uint32_t coastEnd;      // when to end coast and start decel
    uint32_t stepsToGo = 0; // steps we need to take
    float speed = 100;      // speed in rad/s
    float acc = 100;        // acceleration in rad*second^-2
    float dec = 100;        // decceleration in rad*second^-2
    uint32_t accSteps = 0;
    uint32_t decSteps = 0;
    uint8_t Resolution = 8;
    uint8_t status = DISABLED;
    bool dir = CW;
    bool runInfinite = false;
    uint16_t stepsPerRot;    // steps per one revolution, 360/stepAngle * microstep
    uint16_t stepsPerMm = 0; /** Steps per one milimiter, if the motor is performing linear movement */
} ctrl_var_t;

class DendoStepper
{
private:
    DendoStepper_config_t conf;
    ctrl_var_t ctrl;
    esp_timer_handle_t dyingTimer;
    TaskHandle_t enTask;
    uint64_t currentPos = 0; // absolute position
    bool timerStarted = 0;

    /** @brief PRIVATE: Step interval calculation
     *  @param speed maximum movement speed
     *  @param accTimeMs acceleration time in ms
     *  @param target target position
     */
    void calc(uint32_t);

    /** @brief sets En GPIO
     *  @param state 0-LOW,1-HIGH
     *  @return void
     */
    void setEn(bool);

    /** @brief sets Dir GPIO
     *  @param state 0-CW 1-CCW
     */
    void setDir(bool);

    
    /** @brief sets Microstep resolution
     *  @param value 8 16 32 64
     */
    void setResolution(uint8_t value);

    /** @brief static wrapper for ISR function
     *  @param _this DendoStepper* this pointer
     *  @return bool
     */
    static bool xISRwrap(void *_this)
    {
        return static_cast<DendoStepper *>(_this)->xISR();
    }

    /** @brief enableMotor wrapper
     */
    static void _disableMotor(void *_this)
    {
        static_cast<DendoStepper *>(_this)->disableMotor();
    }

    bool xISR();

public:
    /** @brief Costructor - conf variables to be passed later
     */
    DendoStepper();

    /** @brief Configuration of library, used with constructor w/o params
     *  @param config DendoStepper_config_t structure pointer - can be freed after this call
     */
    void config(DendoStepper_config_t *config);

    /** @brief initialize GPIO and Timer peripherals
     *  @param stepP step pulse pin
     *  @param dirP direction signal pin
     *  @param enP enable signal Pin
     *  @param group timer group to use (0 or 1)
     *  @param index which timer to use (0 or 1)
     *  @param microstepping microstepping performed by the driver, used for more accuracy
     *  @param stepsPerRot how many steps it takes for the motor to move 2Pi rads. this can be also used instead of microstepping parameter
     */
    void init(uint8_t, uint8_t, uint8_t, timer_group_t, timer_idx_t, microStepping_t microstep, uint16_t stepsPerRot);

    /** @brief initialize GPIO and Timer peripherals, class must be configured beforehand with @attention config()
     */
    void init();

    /** @brief runs motor to relative position in steps
     *  @param relative number of steps to run, negative is reverse
     */
    esp_err_t runPos(int32_t relative);

    /** @brief runs motor to relative position in mm
     *  @param relative number of mm to run, negative is reverse
     */
    esp_err_t runPosMm(int32_t relative);

    /** @brief run motor to position in absolute coordinates (steps)
     *  @param postition absolute position in steps from home position (must be positive);
     *  @return ESP_OK if motor can run immediately, ESP_ERR if it is currently moving
     */
    esp_err_t runAbs(uint32_t position);

    /** @brief run motor to position in absolute coordinates (millimiters)
     *  @param postition absolute position in mm from home position (must be positive);
     *  @return ESP_OK if motor can run immediately, ESP_ERR if it is currently moving
     */
    esp_err_t runAbsMm(uint32_t position);

    /** @brief sets motor speed
     *  @param speed speed in steps per second
     *  @param accT acceleration time in ms
     *  @param decT deceleration time in ms
     */
    void setSpeed(uint32_t speed, uint16_t accT, uint16_t decT);

    /** @brief sets motor speed and accel in millimeters/second
     *  @param speed speed mm*s^-1
     *  @param accT acceleration time in ms
     *  @param accT deceleration time in ms
     */
    void setSpeedMm(uint32_t speed, uint16_t accT, uint16_t decT);

    /**
     * @brief Set steps per 1 mm of linear movement
     *
     * @param steps steps needed to move one millimiter
     */
    void setStepsPerMm(uint16_t steps);

    /**
     * @brief get steps per 1mm settings
     *
     */
    uint16_t getStepsPerMm();

    /** @brief set EN pin 1, stops movement
     */
    void disableMotor();

    /** @brief set EN pin to 0, enables movement
     */
    void enableMotor();

    /** @brief returns current state
     *  @return motor_status enum
     */
    uint8_t getState();

    /** @brief run motor to position in absolute coordinates (millimiters)
     *  @param postition absolute position in steps from home position (must be positive);
     *  @return ESP_OK if motor can run immediately, ESP_ERR if it is currently moving
     */
    esp_err_t runAbsoluteMm(uint32_t position);

    /** @brief returns current absolute position
     *  @return current absolute postion in steps
     */
    uint64_t getPosition();

    /** @brief returns current absolute position
     *  @return current absolute postion in steps
     */
    uint64_t getPositionMm();

    /** @brief resets absolute pos to 0
     */
    void resetAbsolute();

    /** @brief
     *
     */
    void runInf(bool direction);

    /** @brief returns current speed in steps per second
     */
    uint16_t getSpeed();

    /** @brief returns current acceleration time in ms
     */
    float getAcc();

    /** @brief stops the motor dead, but stays enabled
     */
    void stop();
};

#endif