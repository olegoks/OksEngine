#pragma once 

#include <Struct/ECSGenerator2.ParsedStruct.hpp>

namespace ECSGenerator2 {


	class StructCodeStructureGenerator {
	public:

		struct CreateInfo {
			std::filesystem::path includeDirectory_;
		};

		StructCodeStructureGenerator(const CreateInfo& createInfo) : ci_{ createInfo } {}

		std::shared_ptr<Struct> GenerateStructCodeStructure(std::shared_ptr<ParsedStruct> parsedStruct) {

			if (parsedStruct->GetName() == "ECSMenu") {
				Common::BreakPointLine();
			}
			std::vector<Struct::Field> fields;

			parsedStruct->ForEachField([&](const ParsedStruct::FieldInfo& fieldInfo, bool isLast)->bool {

				Struct::Field field{
					.type_ = fieldInfo.GetTypeName(),
					.name_ = fieldInfo.GetName()
				};

				fields.push_back(field);

				return true;
				});

			Struct::CreateInfo sci{
				.name_ = parsedStruct->GetName(),
				.fields_ = fields,
				.methods_ = {  }
			};
			auto structObject = std::make_shared<Struct>(sci);

			return structObject;
		}

		std::shared_ptr<Struct> GenerateStructForwardDeclaration(std::shared_ptr<ParsedStruct> parsedStruct) {

			Struct::CreateInfo sci{
				.name_ = parsedStruct->GetName(),
				.forwardDeclaration_ = true
			};
			auto structObject = std::make_shared<Struct>(sci);

			return structObject;
		}

		File::Includes GenerateIncludes(std::shared_ptr<ParsedStruct> parsedStruct) {

			File::Includes includes{
				
			};

			parsedStruct->ForEachField([&](const ParsedStruct::FieldInfo& fieldInfo, bool isLast) {

				if (fieldInfo.GetTypeName() == "std::string") {
					includes.paths_.insert("string");
				}
				if (fieldInfo.GetTypeName().find("Common") != std::string::npos) {
					includes.paths_.insert("Common.hpp");
				}
				if (fieldInfo.GetTypeName().find("std::chrono") != std::string::npos) {
					includes.paths_.insert("chrono");
				}
				if (fieldInfo.GetTypeName().find("PAL") != std::string::npos) {
					includes.paths_.insert("PAL.hpp");
				}
				if (fieldInfo.GetTypeName().find("glm") != std::string::npos) {
					includes.paths_.insert("glm/glm.hpp");
				}
				if (fieldInfo.GetTypeName().find("std::queue") != std::string::npos) {
					includes.paths_.insert("queue");
				}
				if (fieldInfo.GetTypeName().find("std::vector") != std::string::npos) {
					includes.paths_.insert("vector");
				}
				if (fieldInfo.GetTypeName().find("std::unordered_set") != std::string::npos) {
					includes.paths_.insert("unordered_set");
				}
				if (fieldInfo.GetTypeName().find("RAL::") != std::string::npos) {
					includes.paths_.insert("RAL.hpp");
				}
				if (fieldInfo.GetTypeName().find("Geom:") != std::string::npos) {
					includes.paths_.insert("Geometry.hpp");
				}
				if (fieldInfo.GetTypeName().find("UIAL::Window") != std::string::npos) {
					includes.paths_.insert("UIAL.Window.hpp");
				}
				if (fieldInfo.GetTypeName().find("UI::Window") != std::string::npos) {
					includes.paths_.insert("UI.Window.hpp");
				}
				if (fieldInfo.GetTypeName().find("sf::") != std::string::npos) {
					includes.paths_.insert("SFML/Audio.hpp");
				}
				if (fieldInfo.GetTypeName().find("glm::quat") != std::string::npos) {
					includes.paths_.insert("glm/gtc/quaternion.hpp");
				}
				if (fieldInfo.GetTypeName().find("PE") != std::string::npos) {
					includes.paths_.insert("PE.PhysicsEngine.hpp");
				}
				if (fieldInfo.GetTypeName().find("AsyncResourceSubsystem") != std::string::npos) {
					includes.paths_.insert("Resources/OksEngine.Resource.Subsystem.hpp");
				}
				if (fieldInfo.GetTypeName().find("ax::NodeEditor::") != std::string::npos) {
					includes.paths_.insert("imgui.h");
					includes.paths_.insert("imgui-node-editor/imgui_node_editor.h");
				}

				return true;
				});

			return includes;
		}

		CreateInfo ci_;
	};

}