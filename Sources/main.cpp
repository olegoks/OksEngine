
#include <Engine.hpp>

#include <filesystem>

int main(int argc, char** argv){

    using namespace std::filesystem;

    path test = "Root/Images/";

    auto root = *test.begin();
    std::string rootStr = root.string();
    using namespace OksEngine;

    Engine engine;
    Entity entity = engine.CreateEntity();
    entity.AddPosition(0, 0, 0);
    entity.AddBehaviour("../../Sources/Scripts/", "TestObject.lua", "TestObject");

    engine.Run();

    return 0;
}