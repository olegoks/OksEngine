#include <filesystem>

#include <Lua.Context.hpp>

#include <OS.CommandLineParameters.hpp>
#include <Resource.hpp>
#include <ECS.Entity.hpp>

#include <ECSGenerator.Code.hpp>

enum class Type {
	Std_string,
	Float,
	Int,
	Common_UInt64,
	Size,
	Index,
	ECS_Entity_Id
};

Type TypeStringToEnum(const std::string typeString) {
	static std::map<std::string, Type> strToEnum{
		{ "std::string", Type::Std_string },
		{ "float", Type::Float },
		{ "Common::UInt64", Type::Common_UInt64 },
		{ "Size", Type::Size },
		{ "Index", Type::Index },
		{ "ECS::Entity::Id", Type::ECS_Entity_Id }
	};
	OS::AssertMessage(strToEnum.contains(typeString), "");
	return strToEnum[typeString];
}


bool IsTypeCanBeEnteredFromImGui(const std::string& typeName) {
	static std::unordered_set<std::string> bindableType{
		"std::string",
		"float",
		"int",
		"Common::UInt64",
		"Common::Size",
		"Common::Index",
		"ECS::Entity::Id"
	};
	return bindableType.contains(typeName);
}


bool IsBindableType(const std::string& typeName) {
	static std::unordered_set<std::string> bindableType{
		"std::string",
		"float",
		"int",
		"Common::UInt64",
		"Common::Size",
		"Common::Index",
		"ECS::Entity::Id"
	};
	return bindableType.contains(typeName);
}

struct SystemInfo {
	std::string name_;

	struct Include {
		std::string componentName_;
		std::string lowerComponentName_;
	};

	std::vector<Include> includes_;
	std::vector<std::string> excludes_;

	using ProcessInclude = std::function<bool(const Include& include, bool isLast)>;

	void ForEachInclude(ProcessInclude&& processInclude) const {
		for (Common::Index i = 0; i < includes_.size(); i++) {
			const Include& include = includes_[i];
			if (!processInclude(include, (i == includes_.size() - 1))) {
				break;
			}
		}
	}

	const std::string& GetName() const {
		return name_;
	}

};

struct ComponentInfo {

	std::string name_;
	std::string lowerName_; //Component name for component variables.
	struct FieldInfo {
		Type type_;
		std::string typeName_;
		std::string name_;

		const std::string& GetName() const {
			return name_;
		}
		const std::string& GetTypeName() const {
			return typeName_;
		}
	};
	std::vector<FieldInfo> fields_;

	const std::string& GetName() const {
		return name_;
	}

	const std::string& GetLowerName() const {
		return lowerName_;
	}

	bool AreThereFields() const {
		return !fields_.empty();
	}

	using ProcessField = std::function<bool(const FieldInfo& fieldInfo, bool isLast)>;

	void ForEachField(ProcessField&& processField) const {
		for (Common::Index i = 0; i < fields_.size(); i++) {
			const FieldInfo info = fields_[i];
			if (!processField(info, (i == fields_.size() - 1))) {
				break;
			}
		}
	}

	bool CanBeCreatedFromImGui() const {
		bool canCreateFromImgui = true;
		ForEachField([&canCreateFromImgui](const FieldInfo& fieldInfo, bool isLast) {
			if (!IsTypeCanBeEnteredFromImGui(fieldInfo.GetTypeName())) {
				canCreateFromImgui = false;
				return false;
			}
			return true;
			});
		return canCreateFromImgui;
	}

};


ComponentInfo CreateComponentInfo(luabridge::LuaRef component) {

	ComponentInfo info;
	info.name_ = component["name"].cast<std::string>().value();
	info.lowerName_ = std::string{ static_cast<char>(std::tolower(info.name_[0])) } + info.name_.substr(1);

	luabridge::LuaRef fields = component["fields"];

	OS::AssertMessage(std::isupper(info.name_[0]), "First component name symbol must be uppercase.");
	for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
		luabridge::LuaRef field = it.value();
		ComponentInfo::FieldInfo fieldInfo{
			.typeName_ = field["type"].cast<std::string>().value(),
			.name_ = field["name"].cast<std::string>().value()
		};
		info.fields_.push_back(fieldInfo);
	}
	return info;
}

SystemInfo CreateSystemInfo(luabridge::LuaRef system) {

	SystemInfo info;
	info.name_ = system["name"].cast<std::string>().value();
	luabridge::LuaRef includes = system["includes"];

	OS::AssertMessage(std::isupper(info.name_[0]), "First system name symbol must be uppercase.");
	for (luabridge::Iterator it(includes); !it.isNil(); ++it) {
		luabridge::LuaRef include = it.value();
		SystemInfo::Include includeInfo{  };
		std::string componentName = include.cast<std::string>().value();
		includeInfo.componentName_ = componentName;
		OS::AssertMessage(std::isupper(includeInfo.componentName_[0]), "");
		std::transform(componentName.begin(), componentName.end(), componentName.begin(), [](unsigned char c) {
			return std::tolower(c);
			});
		includeInfo.lowerComponentName_ = componentName;
		info.includes_.push_back(includeInfo);
	}
	return info;
}


std::string GetImGuiType(const std::string& fieldType) {
	if (fieldType == "float") {
		return "ImGuiDataType_Float";
	}
	else if (fieldType == "Common::UInt64") {
		return "ImGuiDataType_U64";
	}
	else if (fieldType == "Common::Size") {
		return "ImGuiDataType_U64";
	}
	else if (fieldType == "Common::Index") {
		return "ImGuiDataType_U64";
	}
	else if (fieldType == "ECS::Entity::Id") {
		return "ImGuiDataType_U64";
	}
	OS::AssertFail();
	return "";
}

//static char {variableName}[256] = "";
//static Common::UInt64 {variableName};
std::string GenerateTypeImGuiInputVariable(const ComponentInfo& componentInfo, const ComponentInfo::FieldInfo& fieldInfo, std::string variableName) {
	std::string code;
	const std::string& fieldTypeName = fieldInfo.GetTypeName();
	if (fieldTypeName == "std::string") {
		code += "static char " + variableName + "[256] = \"\";";
		return code;
	}
	else if (fieldTypeName == "float" || fieldTypeName == "double" || fieldTypeName == "Common::Size" || fieldTypeName == "Common::UInt64" || fieldTypeName == "Common::Index" || fieldTypeName == "ECS::Entity::Id") {
		code += "static " + fieldInfo.GetTypeName() + " " + variableName + ";";
		return code;
	}
	return "";
}

// ImGui::InputScalar("Value", ImGuiDataType_Float, {outVariable});\n
// ImGui::InputText("Value", {outVariable}, IM_ARRAYSIZE({outVariable}));\n
std::string GenerateImGuiInputTypeCode(const ComponentInfo& componentInfo, const ComponentInfo::FieldInfo& fieldInfo, std::string outVariable) {
	std::string code;
	const std::string& fieldTypeName = fieldInfo.GetTypeName();
	if (fieldTypeName == "std::string") {
		code += "ImGui::InputText(\"" + fieldInfo.GetName() + "\", " + outVariable + ", IM_ARRAYSIZE(" + outVariable + "));\n";
		return code;
	}
	else if (fieldTypeName == "float" || fieldTypeName == "double" || fieldTypeName == "Common::Size" || fieldTypeName == "Common::UInt64" || fieldTypeName == "Common::Index" || fieldTypeName == "ECS::Entity::Id") {
		code += "ImGui::InputScalar(\"" + fieldInfo.GetName() + "\", " + GetImGuiType(fieldTypeName) + ", " + outVariable + ");\n";
		return code;
	}
	return "";
}

std::string GenerateTypeImGuiEditCode(const ComponentInfo& componentInfo, const ComponentInfo::FieldInfo& fieldInfo) {
	std::string code;
	const std::string& fieldTypeName = fieldInfo.GetTypeName();
	if (fieldTypeName == "std::string") {
		code += "\t" + GenerateTypeImGuiInputVariable(componentInfo, fieldInfo, "buffer" + fieldInfo.GetName());
		code += "\n";
		code += "\tstd::memcpy(buffer" + fieldInfo.GetName() + ", " + componentInfo.GetLowerName() + "->" + fieldInfo.GetName() + "_.c_str(), " + componentInfo.GetLowerName() + "->" + fieldInfo.GetName() + "_.size());\n";
		code += "\t" + GenerateImGuiInputTypeCode(componentInfo, fieldInfo, "buffer" + fieldInfo.GetName());
		code += "\t" + componentInfo.GetLowerName() + "->" + fieldInfo.GetName() + "_ = std::string{ " + "buffer" + fieldInfo.GetName() + " };\n";
		return code;
	}
	else if (fieldTypeName == "float" || fieldTypeName == "double" || fieldTypeName == "Common::Size" || fieldTypeName == "Common::UInt64" || fieldTypeName == "Common::Index" || fieldTypeName == "ECS::Entity::Id") {
		code += GenerateImGuiInputTypeCode(componentInfo, fieldInfo, "&" + componentInfo.GetLowerName() + "->" + fieldInfo.GetName() + "_");
		return code;
	}
	return "";
}

std::string GenerateImGuiEditFunction(const ComponentInfo& component) {

	std::string code = "\n";
	code += "template<>\n";
	code += "inline void Edit<" + component.GetName() + ">(" + component.GetName() + "* " + component.GetLowerName() + ") {\n";
	component.ForEachField([&](const ComponentInfo::FieldInfo& field, bool isLast) {
		if (IsTypeCanBeEnteredFromImGui(field.GetTypeName())) {
			code += GenerateTypeImGuiEditCode(component, field);
		}
		return true;
		});

	code += "};\n\n";


	return code;
}


std::string GenerateComponentStructDefinition(const ComponentInfo& component) {
	std::string code = "\n";
	code += "struct " + component.GetName() + ": public ECSComponent<" + component.GetName() + "> " + "{\n";

	//Default constructor.
	code += "\t" + component.GetName() + "()\n\t : ECSComponent{ nullptr } {}\n";

	//Main constructor.
	if (component.AreThereFields()) {
		//Main constructor.
		code += "\t" + component.GetName() + "(";
		component.ForEachField([&code](const ComponentInfo::FieldInfo& field, bool isLast) {
			code += field.GetTypeName() + " " + field.GetName();
			if (!isLast) {
				code += ", ";
			}
			return true;
			});
		code += ") : \n";
		code += "\t\tECSComponent{ nullptr }, \n";

		component.ForEachField([&code](const ComponentInfo::FieldInfo& field, bool isLast) {
			code += "\t\t" + field.GetName() + "_{ " + field.GetName() + " }";
			if (!isLast) {
				code += ",\n";
			}
			return true;
			});
		code += "{ }\n";

		/*[[nodiscard]]
		static const char* GetName() noexcept {
			return Common::TypeInfo<Type>::GetTypeName();
		}*/

		code += "\n";

		code += "\t\tstatic const char* GetName() noexcept {\n"
			"\t\t\treturn Common::TypeInfo<" + component.GetName() + ">::GetTypeName();\n"
			"\t\t}\n";

		/*[[nodiscard]]
		static std::size_t GetSize() noexcept { return sizeof(Type); }*/

		code += "\t\t[[nodiscard]]\n"
			"\t\tstatic std::size_t GetSize() noexcept { return sizeof(" + component.GetName() + "); }\n";

	}

	component.ForEachField([&code](const ComponentInfo::FieldInfo& field, bool isLast) {
		code += "\t" + field.GetTypeName() + " " + field.GetName() + "_;\n";
		return true;
		});


	code += "};\n";
	return code;
}

//	std::string GetName() const noexcept override {
//		return "UpdateClock";
//	}
std::string GenerateGetNameFunction(const SystemInfo& system) {
	std::string code;

	code += "\t\tstd::string GetName() const noexcept override {\n";
	code += "\t\t\treturn \"" + system.GetName() + "\";\n";
	code += "\t\t};\n";

	return code;
}

//	virtual Common::TypeId GetTypeId() const noexcept override {
//		return Common::TypeInfo<UpdateClock>().GetId();
//	}
std::string GenerateGetTypeIdFunction(const SystemInfo& system) {
	std::string code;

	code += "\t\tvirtual Common::TypeId GetTypeId() const noexcept override {\n";
	code += "\t\t\treturn Common::TypeInfo<" + system.GetName() + ">().GetId();\n";
	code += "\t\t};";

	return code;
}

std::string GenerateSystemRunFunction(const SystemInfo& system) {
	std::string code;

	using namespace ECSGenerator;
	std::vector<Function::Parameter> parameters;
	parameters.push_back({ "ECS::World*", "world" });
	Function::CreateInfo fci{
		.name_ = "Run" + system.name_,
		.parameters_ = parameters,
		.returnType_ = "void"
	};
	Function function{ fci };
	//code += ;

	return code;
}


std::string GenerateUpdateFunctionPrototype(const SystemInfo& system) {
	std::string code;

	code += "\t\tvoid Update(";

	system.ForEachInclude([&](const SystemInfo::Include& include, bool isLast) {
		code += include.componentName_ + "* " + include.lowerComponentName_;
		if (!isLast) {
			code += ", ";
		}
		
		return true;
		});

	code += ");\n";
	return code;
}


//struct UpdateClock : public ECS::System2 {

	//	void Update(Clock* clock);

	//	[[nodiscard]]
	//	std::string GetName() const noexcept override {
	//		return "UpdateClock";
	//	}

	//	virtual Common::TypeId GetTypeId() const noexcept override {
	//		return Common::TypeInfo<UpdateClock>().GetId();
	//	}

	//};

std::string GenerateSystemClassDefinition(const SystemInfo& system) {
	std::string code = "\n";


	
	code += "\tclass " + system.GetName() + ": public ECS::System2" + "{\n";

	code += "\n";
	system.ForEachInclude([&](const SystemInfo::Include& include, bool isLast) {
		code += "\t\tclass " + include.componentName_ + ";\n";
		return true;
		});

	code += "\n";

	code += GenerateUpdateFunctionPrototype(system);
	code += "\n";
	code += GenerateGetNameFunction(system);
	code += "\n";
	code += GenerateGetTypeIdFunction(system);
	code += "\n";

	code += "\t};\n";

	return code;
}



std::string GenerateAddFunction(const ComponentInfo& component) {
	//Generate Add function. 
	std::string code;
	code += "template<>\n";
	code += "inline void Add<" + component.GetName() + ">(ECS::World * world, ECS::Entity::Id entityId) {\n";
	code += "\n";
	if (component.CanBeCreatedFromImGui()) {
		component.ForEachField([&](const ComponentInfo::FieldInfo& field, bool isLast) {
			code += "\t" + GenerateTypeImGuiInputVariable(component, field, field.GetName()) + "\n";
			return true;
			});
	}
	code += "\tif (ImGui::CollapsingHeader(\"Create info\")) {\n";
	
	if (component.CanBeCreatedFromImGui()) {
		component.ForEachField([&](const ComponentInfo::FieldInfo& field, bool isLast) {
			std::string variableName;
			if (!(field.GetTypeName() == "std::string")) {
				variableName += "&"; // If string we dont need &, because array id is pointer.
			}
			variableName += field.GetName();
			code += "\t\t" + GenerateImGuiInputTypeCode(component, field, variableName) + "\n";
			return true;
			});
	}
	code += "\t\tImGui::Spacing();\n";
	code += "\t}\n";
	code += "\tif (ImGui::Button(\"Add component\")) {\n";
	code += "\t\tif (!world->IsComponentExist<" + component.GetName() + ">(entityId)) {\n";
	code += "\t\t\tworld->CreateComponent<" + component.GetName() + ">(entityId";
	if (component.CanBeCreatedFromImGui()) {
		if (component.AreThereFields()) {
			code += ", ";
		}
		component.ForEachField([&](const ComponentInfo::FieldInfo& field, bool isLast) {
			code += field.GetName();
			if (!isLast) {
				code += ", ";
			}
			return true;
			});
	}
	code += ");\n";

	code += "\t\t}\n";
	code += "\t}\n";
	code += "};\n\n";

	return code;
}

class ECSFile {
	std::vector<ComponentInfo> components_;
	std::vector<SystemInfo> systems_;
};


std::string ParseEcsFile(Resources::ResourceData& ecsFileData) {

	::Lua::Script script{ std::string{ ecsFileData.GetData<Common::Byte>(), ecsFileData.GetSize()} };

	::Lua::Context context;
	context.LoadScript(script);

	if (!context.GetGlobalAsRef("component").isNil()) {
		std::string cxxECSCode = "//THIS FILE IS AUTOGENERATED!!!\n";
		cxxECSCode += "#pragma once\n";
		cxxECSCode += "\n";
		cxxECSCode += "#include <OksEngine.ECS.Component.hpp>\n";
		cxxECSCode += "#include <imgui.h>\n";
		cxxECSCode += "namespace OksEngine{\n\n";

		luabridge::LuaRef component = context.GetGlobalAsRef("component");
		luabridge::LuaRef fields = component["fields"];
		const ComponentInfo componentInfo = CreateComponentInfo(component);

		std::string name = componentInfo.GetName();
		std::string lowerName = componentInfo.GetLowerName();

		cxxECSCode += GenerateComponentStructDefinition(componentInfo);

		cxxECSCode += "\n";

		//Generate Edit function. 
		cxxECSCode += GenerateImGuiEditFunction(componentInfo);

		//Generate Bind function. 
		cxxECSCode += "template<>\n";
		cxxECSCode += "inline void Bind<" + name + ">(::Lua::Context & context) {\n";
		cxxECSCode += "\tcontext.GetGlobalNamespace()\n";
		cxxECSCode += "\t\t.beginClass<" + name + ">(\"" + name + "\")\n";
		for (luabridge::Iterator it(fields); !it.isNil(); ++it) {
			luabridge::LuaRef field = it.value();
			std::string type = field["type"].cast<std::string>().value();
			std::string fieldName = field["name"].cast<std::string>().value();
			if (IsBindableType(type)) {
				cxxECSCode += "\t\t.addProperty (\"" + fieldName + "\", &" + name + "::" + fieldName + "_)\n";
			}
		}
		cxxECSCode += "\t\t.endClass();\n";
		cxxECSCode += "};\n\n";

		cxxECSCode += GenerateAddFunction(componentInfo);



		//Namespace OksEngine end.
		cxxECSCode += "\n};\n";
		return cxxECSCode;
	}
	
	if (!context.GetGlobalAsRef("system").isNil()) {
		std::string cxxECSCode = "//THIS FILE IS AUTOGENERATED!!!\n";
		cxxECSCode += "#pragma once\n";
		cxxECSCode += "\n";
		cxxECSCode += "#include <OksEngine.ECS.System.hpp>\n";
		cxxECSCode += "#include <imgui.h>\n";
		cxxECSCode += "namespace OksEngine{\n\n";

		luabridge::LuaRef system = context.GetGlobalAsRef("system");
		const SystemInfo systemInfo = CreateSystemInfo(system);
		cxxECSCode += "\n";
		cxxECSCode += GenerateSystemClassDefinition(systemInfo);
		cxxECSCode += "\n";


		//Namespace OksEngine end.
		cxxECSCode += "\n};\n";
		return cxxECSCode;
	}
}


#include <graphviz/gvc.h>

int main(int argc, char** argv) {

	// Создаём новый графический контекст и граф
	GVC_t* gvc = gvContext();
	Agraph_t* graph = agopen((char*)"G", Agdirected, 0);

	// Добавляем узлы
	Agnode_t* node1 = agnode(graph, (char*)"Node1", 1);
	Agnode_t* node2 = agnode(graph, (char*)"Node2", 1);

	// Добавляем рёбра
	agedge(graph, node1, node2, 0, 1);

	// Настраиваем параметрыализации
	gvLayout(gvc, graph, "dot");

	// Генерируем и сохраняем изображение в формате PNG
	gvRenderFilename(gvc, graph, "png", "graph.png");

	// Освобождаем ресурсы
	gvFreeLayout(gvc, graph);
	agclose(graph);
	gvFreeContext(gvc);


	

	OS::CommandLineParameters parameters{ argc, argv };


	OS::InitializeLogger(
		parameters.GetArgc(),
		parameters.GetArgv());

	OS::AssertMessage(argc > 1, "");
	const std::string_view workDir = parameters.GetValue("-workDir");
	std::vector<std::filesystem::path> workDirs{ workDir };
	Resources::ResourceSystem resourceSystem;
	resourceSystem.SetRoots(workDirs);

	struct ECSFileInfo {
		std::filesystem::path resourceSystemPath_;
		std::filesystem::path filesystemPath_;
	};

	std::vector<ECSFileInfo> ecsFileInfos;

	const std::filesystem::path root = "Root";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(workDir)) {
		if (std::filesystem::is_regular_file(entry)) {
			const bool ecsFile = entry.path().extension() == ".ecs";
			if (ecsFile) {
				const std::filesystem::path name = root / entry.path().filename();
				resourceSystem.LoadResource(name);
				ecsFileInfos.push_back(ECSFileInfo{
						.resourceSystemPath_ = name,
						.filesystemPath_ = entry.path()
					});
			}
		}
	}

	for (const ECSFileInfo& ecsFileInfo : ecsFileInfos) {
		Resources::ResourceData resourceData = resourceSystem.GetResourceData(ecsFileInfo.resourceSystemPath_);
		const std::string cxxCode = ParseEcsFile(resourceData);
		const std::string autoPrefix = "auto_";
		const std::filesystem::path cxxCodeFileName = autoPrefix + ecsFileInfo.filesystemPath_.filename().replace_extension(".hpp").string();
		const std::filesystem::path cxxCodeFileFullPath = ecsFileInfo.filesystemPath_.parent_path() / cxxCodeFileName;
		OS::TextFile cxxECSCodeFile{ cxxCodeFileFullPath };
		cxxECSCodeFile.Create();
		cxxECSCodeFile << cxxCode;
	}

	return 0;
}

