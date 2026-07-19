#pragma once

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

class TelegramBot
{
public:
    TelegramBot();

    bool begin();

    void update();

    bool sendMessage(const char* message);

    bool isReady() const;

private:

    WiFiClientSecure secureClient_;

    UniversalTelegramBot* bot_;

    bool ready_;
};