#pragma once

#define CCAPI_ENABLE_SERVICE_MARKET_DATA      // ¬ключаем сервис рыночных данных
#define CCAPI_ENABLE_EXCHANGE_BYBIT           // ¬ключаем поддержку Bybit (или замените на нужную биржу)
#define CCAPI_ENABLE_SERVICE_EXECUTION_MANAGEMENT
#include <Crypto\auto_OksEngine.Crypto.hpp>


namespace ccapi {

	Logger* Logger::logger = nullptr;  // This line is needed.

	class MyEventHandler : public EventHandler {
	public:
		void processEvent(const Event& event, Session* sessionPtr) override {

			std::cout << "Received an event:\n" + event.toPrettyString(2, 2) << std::endl;

		}
	};

} /* namespace ccapi */


namespace OksEngine
{

	namespace Crypto
	{
		namespace Session
		{
			void Create::Update() {

				::ccapi::SessionOptions sessionOptions;
				::ccapi::SessionConfigs sessionConfigs;
				sessionConfigs.setUrlRestBase({ { "bybit", "https://api.bybit.com" } });
				sessionConfigs.setUrlWebsocketBase({ { "bybit", "wss://stream.bybit.com" } });

				auto session = std::make_shared<ccapi::Session>(sessionOptions, sessionConfigs, nullptr);

				auto eventQueue = std::make_shared<ccapi::Queue<ccapi::Event>>();// eventQueue;
				//::ccapi::Request request(::ccapi::Request::Operation::CREATE_ORDER, CCAPI_EXCHANGE_NAME_BYBIT, "BTCUSDT");
				//request.appendParam({
				//	{"SIDE", "BUY"},
				//	{"QUANTITY", "0.000133"},
				//	{"LIMIT_PRICE", "20000"},
				//	 {"ORDER_TYPE", "LIMIT"},         // явно указываем, что это лимитный ордер
				//	{"TIME_IN_FORCE", "GTC"},        // ќрдер активен до отмены
				//	{"INSTRUMENT_TYPE", "SPOT"},
				//	{"RECV_WINDOW", "10000"}
				//	});
				//session.sendRequest(request, &eventQueue);
				//ccapi::Event event = eventQueue.popBack();

				//session.stop();

				ccapi::Subscription min15CandlesSubscription("bybit", "BTCUSDT", CCAPI_CANDLESTICK, "CANDLESTICK_INTERVAL_SECONDS=900");
				session->subscribe(min15CandlesSubscription);
				/*session->*/

				ccapi::Request request(ccapi::Request::Operation::GET_RECENT_CANDLESTICKS, "bybit", "BTCUSDT");
				request.appendParam({
					{"INTERVAL", "15"},    // »нтервал в минутах
					{"LIMIT", "13"}        //  оличество свечей
					});

				session->sendRequest(request);

				Common::Size candlesReceived = 0;
				std::vector<double> closingPrices;
				while (candlesReceived != 13) {
					
					if (!session->getEventQueue().empty()) {
						const auto& event = session->getEventQueue().popBack();
						for (const auto& message : event.getMessageList()) {
							if (message.getType() == ccapi::Message::Type::MARKET_DATA_EVENTS_CANDLESTICK) {

								for (const auto& element : message.getElementList()) {
									auto& nameToValue = element.getNameValueMap();
									ASSERT(nameToValue.contains("CLOSE_PRICE"));
									double closePrice = std::stod(nameToValue.find("CLOSE_PRICE")->second);
									closingPrices.push_back(closePrice);
									std::cout << "Close price: " << closePrice << std::endl;

								}
								// «десь closingPrices.size() будет равно 13
								// ѕередайте этот вектор в ваш класс RSICalculator дл€ инициализации
								break;
							}
						}
					}
				}

				

				

				ccapi::Subscription currentPriceSubsription("bybit", "BTCUSDT", CCAPI_TRADE);
				session->subscribe(currentPriceSubsription);


				const ECS2::Entity::Id sessionEntityId = CreateEntity();
				CreateComponent<Session::Tag>(sessionEntityId);
				CreateComponent<Session::Instance>(sessionEntityId, session);
				CreateComponent<Session::EventsQueue>(sessionEntityId, eventQueue);
			};

		}

	} // namespace Crypto

	namespace Crypto
	{

		namespace State {

			void Create::Update() {
				ECS2::Entity::Id stateEntityId = CreateEntity();
				CreateComponent<State::Tag>(stateEntityId);
				CreateComponent<State::CurrentPrice>(stateEntityId);
			}
		}
		namespace Session
		{
			void ProcessRequests::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Crypto::Session::Tag* session__Tag0,
				OksEngine::Crypto::Session::Instance* session__Instance0,
				OksEngine::Crypto::Session::EventsQueue* session__EventsQueue0,

				ECS2::Entity::Id entity1id,
				const OksEngine::Crypto::State::Tag* state__Tag1,
				OksEngine::Crypto::State::CurrentPrice* state__CurrentPrice1) {

				while (!session__Instance0->session_->getEventQueue().empty()) {

					const auto& event = session__Instance0->session_->getEventQueue().popBack();
					if (!event.getMessageList().empty()) {
						const auto& message = event.getMessageList().front();
						//std::cout << event.toPrettyString() << std::endl;
						if (message.getType() == ccapi::Message::Type::MARKET_DATA_EVENTS_TRADE) {
							for (const auto& element : message.getElementList()) {
								auto& nameToValue = element.getNameValueMap();
								if (nameToValue.contains("LAST_PRICE")) {
									std::string_view lastPriceKey = "LAST_PRICE";
									state__CurrentPrice1->price_ = std::stod(nameToValue.find(lastPriceKey)->second);
								}
							}
						}
						else {
							Common::BreakPointLine();
						}

					}
				}

			}

			void CreatePriceRequest::Update(
				ECS2::Entity::Id entity0id,
				const OksEngine::Crypto::Session::Tag* session__Tag0,
				OksEngine::Crypto::Session::Instance* session__Instance0,
				OksEngine::Crypto::Session::EventsQueue* session__EventsQueue0) {

				//BEGIN_PROFILE("Creating request");
				//::ccapi::Request request(ccapi::Request::Operation::GET_RECENT_TRADES, CCAPI_EXCHANGE_NAME_BYBIT, "BTCUSDT");
				//END_PROFILE();

				//BEGIN_PROFILE("Sending request")
				//session__Instance0->session_->sendRequest(request, session__EventsQueue0->queue_.get());
				//END_PROFILE();

			};

		}

	} // namespace Crypto

} // namespace OksEngine