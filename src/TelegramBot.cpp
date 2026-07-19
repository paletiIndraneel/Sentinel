#include "TelegramBot.h"
#include "Config.h"

TelegramBot::TelegramBot()
    : bot_(nullptr),
      ready_(false)
{
}

bool TelegramBot::begin()
{
    secureClient_.setInsecure();
    bot_ = new UniversalTelegramBot(sentinel::config::TELEGRAM_BOT_TOKEN,secureClient_);
    ready_ = (bot_ != nullptr);
    return ready_;
}

void TelegramBot::update()
{
}

bool TelegramBot::sendMessage(const char* message)
{
    if (!ready_ || bot_ == nullptr)
    {
        return false;
    }
    return bot_->sendMessage(sentinel::config::TELEGRAM_CHAT_ID,message,"");
}

bool TelegramBot::isReady() const
{
    return ready_;
}