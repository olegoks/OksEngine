#pragma once

#include <tgbot/tgbot.h>

#include <Telegram\auto_OksEngine.Telegram.Bot.hpp>

#define CCAPI_ENABLE_SERVICE_MARKET_DATA      // Включаем сервис рыночных данных
#define CCAPI_ENABLE_EXCHANGE_BYBIT           // Включаем поддержку Bybit (или замените на нужную биржу)
#define CCAPI_ENABLE_SERVICE_EXECUTION_MANAGEMENT

//#include "ccapi_cpp/ccapi_session.h"

//namespace OksEngine {
//	std::unique_ptr<TgBot::Bot> bot = nullptr;
//	std::unique_ptr<TgBot::TgLongPoll> longPoll = nullptr;
//}

namespace OksEngine
{



	namespace Telegram
	{
		namespace Bot
		{
			void Create::Update() {

				std::string bot_token = "8653090832:AAFDSofDw2Xa_MFKnAtDGs0lPdNGH8oHAm4";

				auto bot = std::make_shared<TgBot::Bot>(bot_token);
				auto longPoll = std::make_shared<TgBot::TgLongPoll>(*bot, 1, 1);
				
				bot->getEvents().onCommand("start", [&](TgBot::Message::Ptr message) {
					bot->getApi().sendMessage(message->chat->id, "Hello!");
					
					});

				std::cout << "Бот @" << bot->getApi().getMe()->username << " успешно запущен!" << std::endl;

				const ECS2::Entity::Id tgBotEntity = CreateEntity();
				CreateComponent<Telegram::Bot::Tag>(tgBotEntity);
				CreateComponent<Telegram::Bot::Token>(tgBotEntity, bot_token);
				CreateComponent<Telegram::Bot::Instance>(tgBotEntity, bot, longPoll);
			};

			void Process::Update(
				ECS2::Entity::Id entity0id, 
				const OksEngine::Telegram::Bot::Tag* telegram__Bot__Tag0,
				const OksEngine::Telegram::Bot::Token* telegram__Bot__Token0,
				const OksEngine::Telegram::Bot::Instance* telegram__Bot__Instance0) {


				try {
					//telegram__Bot__Instance0->longPoll_->start();
				}
				catch (const TgBot::TgException& e) {
					// Ловим и выводим ошибки
					std::cerr << "Произошла ошибка: " << e.what() << std::endl;
				}

			}

		}

	} // namespace Telegram

} // namespace OksEngine