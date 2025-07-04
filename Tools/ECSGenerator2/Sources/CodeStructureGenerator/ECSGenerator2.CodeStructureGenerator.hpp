#pragma once 

#include <System/ECSGenerator2.SystemCodeStructureGenerator.hpp>

namespace ECSGenerator2 {

    class CodeStructureGenerator {

        public:

        std::vector<std::shared_ptr<OS::TextFile>> Generate(
            const std::filesystem::path& includeDirectory,
            const std::filesystem::path& ecsFilePath, 
            const std::shared_ptr<ParsedECSFile> parsedECSFile) {


            SystemStructureGenerator::CreateInfo ssgci{
                .includeDirectory_ = includeDirectory
            };

            SystemStructureGenerator systemGenerator{ ssgci };

            parsedECSFile->ForEachSystem(
                [&](std::shared_ptr<ParsedSystem> parsedSystem) {
                    systemGenerator.GenerateSystemStructCode(parsedSystem);
                });

            auto dotfile = std::make_shared<OS::TextFile>(includeDirectory / "auto_ECSSystemsGraph.dot");

            return { dotfile };
        }

    };

}