
#include <Engine.hpp>

int main(int argc, char** argv){

    using namespace OksEngine;

    Engine engine;
    Entity entity = engine.CreateEntity();
    entity.AddPosition(0, 0, 0);
    entity.AddBehaviour("../../Sources/Scripts/", "TestObject.lua", "TestObject");

    engine.Run();

    return 0;
}