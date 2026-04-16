#pragma once

#include <tgbot/tgbot.h>

#include <Telegram\auto_OksEngine.Telegram.Bot.hpp>

#define CCAPI_ENABLE_SERVICE_MARKET_DATA      // Включаем сервис рыночных данных
#define CCAPI_ENABLE_EXCHANGE_BYBIT           // Включаем поддержку Bybit (или замените на нужную биржу)

#include "ccapi_cpp/ccapi_session.h"

namespace OksEngine {
	std::unique_ptr<TgBot::Bot> bot = nullptr;
	std::unique_ptr<TgBot::TgLongPoll> longPoll = nullptr;
}

namespace ccapi {

	Logger* Logger::logger = nullptr;  // This line is needed.

	class MyEventHandler : public EventHandler {
	public:

		std::int64_t chat_id_ = - 1;

		bool processEvent(const Event& event, Session* sessionPtr) override { 

			std::cout << "Received an event:\n" + event.toStringPretty(2, 2) << std::endl;

			OksEngine::bot->getApi().sendMessage(chat_id_, event.toStringPretty(2, 2));
			
			return true; }
	};

} /* namespace ccapi */

using ::ccapi::MyEventHandler;
using ::ccapi::Request;
using ::ccapi::Session;
using ::ccapi::SessionConfigs;
using ::ccapi::SessionOptions;

namespace OksEngine
{



	namespace Telegram
	{
		namespace Bot
		{
			void Create::Update() {

				std::string bot_token = "8653090832:AAFDSofDw2Xa_MFKnAtDGs0lPdNGH8oHAm4";

				bot = std::make_unique<TgBot::Bot>(bot_token);

				bot->getEvents().onCommand("start", [&](TgBot::Message::Ptr message) {
					bot->getApi().sendMessage(message->chat->id, "Hello!");
					SessionOptions sessionOptions;
					SessionConfigs sessionConfigs;
					MyEventHandler eventHandler;
					eventHandler.chat_id_ = message->chat->id;
					Session session(sessionOptions, sessionConfigs, &eventHandler);
					Request request(Request::Operation::GET_RECENT_TRADES, "bybit", "BTCUSDT");
					request.appendParam({
						{"LIMIT", "1"},
						});
					session.sendRequest(request);
					
					std::this_thread::sleep_for(std::chrono::seconds(10));
					session.stop();
					});

				std::cout << "Бот @" << bot->getApi().getMe()->username << " успешно запущен!" << std::endl;

				const ECS2::Entity::Id tgBotEntity = CreateEntity();
				CreateComponent<Telegram::Bot::Tag>(tgBotEntity);
				CreateComponent<Telegram::Bot::Token>(tgBotEntity, bot_token);
			};

			void Process::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Telegram::Bot::Tag* telegram__Bot__Tag0,
				const OksEngine::Telegram::Bot::Token* telegram__Bot__Token0) {
				try {
					if (longPoll == nullptr) {
						longPoll = std::make_unique<TgBot::TgLongPoll>(*bot);
					}

					longPoll->start();
				}
				catch (const TgBot::TgException& e) {
					// Ловим и выводим ошибки
					std::cerr << "Произошла ошибка: " << e.what() << std::endl;
				}

			}

		}

	} // namespace Telegram

} // namespace OksEngine