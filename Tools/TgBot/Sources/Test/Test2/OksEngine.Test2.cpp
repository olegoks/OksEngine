#include <Test\Test2\auto_OksEngine.Test2.hpp>

#include <chrono>

namespace OksEngine {
    namespace Test2 {
        using namespace std::chrono_literals;
        void TestMain1::Update(ECS2::Entity::Id entity0Id, const OksEngine::Test2::Test2* oKSENGINE__TEST2__TEST2) {
            Common::UInt64 j = 0;
            for (Common::Index i = 0; i < 900'000'000; i++) {
                j++;
            }
        }
        void TestChild1::Update(ECS2::Entity::Id entity0Id, const OksEngine::Test2::Test1* oKSENGINE__TEST2__TEST2) {
            Common::UInt64 j = 0;
            for (Common::Index i = 0; i < 100'000'000; i++) {
                j++;
            }
        }
        void TestMain2::Update(ECS2::Entity::Id entity0Id, OksEngine::Test2::Test2* oKSENGINE__TEST2__TEST2) {
            Common::UInt64 j = 0;
            for (Common::Index i = 0; i < 700'000'000; i++) {
                j++;
            }
        }

        void TestChild2::Update(ECS2::Entity::Id entity0Id, const OksEngine::Test2::Test2* oKSENGINE__TEST2__TEST2) {
            Common::UInt64 j = 0;
            for (Common::Index i = 0; i < 300'000'000; i++) {
                j++;
            }
        }

        void InitTest::Update(ECS2::Entity::Id entity0Id, const OksEngine::Test2::Test2* oKSENGINE__TEST2__TEST2) {
            Common::UInt64 j = 0;
            for (Common::Index i = 0; i < 600'000'000; i++) {
                j++;
            }
        }

    }
}