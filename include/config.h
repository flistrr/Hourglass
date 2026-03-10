#pragma once

class Config
{
private:
public:
    int minutesFixed;
    int secondsFixed;
    unsigned long interval;
    Config(int minutesFixed, int secondsFixed, unsigned long interval);
    Config();
};

Config::Config()
{
    this->minutesFixed = 2;
    this->secondsFixed = 0;
    this->interval = 1000;
}

Config::Config(int minutesFixed, int secondsFixed, unsigned long interval)
{
    this->minutesFixed = minutesFixed;
    this->secondsFixed = secondsFixed;
    this->interval = interval;
}
