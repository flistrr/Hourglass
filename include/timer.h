#include <Arduino.h>
#include "config.h"

class Timer
{
private:
    Config cfg;
    unsigned long interval;
    unsigned long lastMillis;
    int seconds;
    int minutes;

public:
    Timer(Config *inputCfg);
    void tickClock();
    void updateTimer();
    void printTime();

    void reverseTime();
};

Timer::Timer(Config *inputCfg)
{
    if (inputCfg != NULL)
    {
        this->cfg = *inputCfg;
    }
    else
    {
        Serial.println("Error: Config is NULL. Using default.");
        this->cfg = Config();
    }

    this->interval = (cfg.interval > 0) ? cfg.interval : 1000;
    this->seconds = cfg.secondsFixed;
    this->minutes = cfg.minutesFixed;

    // Initialize the timer tracker
    this->lastMillis = millis();
}

void Timer::tickClock()
{
    if (minutes <= 0 && seconds <= 0)
    {
        return;
    }

    seconds--;

    if (seconds < 0)
    {
        seconds = 59;
        minutes--;
    }
}

void Timer::printTime()
{
    if (minutes < 10)
        Serial.print("0");
    Serial.print(minutes);
    Serial.print(":");

    if (seconds < 10)
        Serial.print("0");
    Serial.println(seconds);
}

void Timer::updateTimer()
{
    if (millis() - lastMillis >= interval)
    {
        lastMillis = millis();
        tickClock();
        printTime();
    }
}

void Timer::reverseTime()
{
    if (seconds > 0)
    {
        minutes = (cfg.minutesFixed - minutes) - 1;
        seconds = cfg.secondsFixed - seconds;
    }
    else
    {
        minutes = cfg.minutesFixed - minutes;
        seconds = 0;
    }
}