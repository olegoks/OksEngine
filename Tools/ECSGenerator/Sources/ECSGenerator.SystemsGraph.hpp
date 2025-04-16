#pragma once

#include <ECSGenerator.ProjectContext.hpp>

#include <ECSGenerator.ParsedSystemEcsFile.hpp>
#include <ECSGenerator.ParsedComponentEcsFile.hpp>
#include <graphviz/gvc.h>


namespace ECSGenerator {


	void dfs(Agnode_t* node, std::unordered_map<Agnode_t*, bool>& visited, std::vector<Agnode_t*>& component, Agraph_t* g) {
		visited[node] = true;
		component.push_back(node);

		for (Agedge_t* edge = agfstout(g, node); edge; edge = agnxtout(g, edge)) {
			Agnode_t* neighbor = aghead(edge);
			if (!visited[neighbor]) {
				dfs(neighbor, visited, component, g);
			}
		}

		for (Agedge_t* edge = agfstin(g, node); edge; edge = agnxtin(g, edge)) {
			Agnode_t* neighbor = agtail(edge);
			if (!visited[neighbor]) {
				dfs(neighbor, visited, component, g);
			}
		}
	}

	std::vector<std::vector<Agnode_t*>> findConnectedComponents(Agraph_t* g) {
		std::unordered_map<Agnode_t*, bool> visited;
		std::vector<std::vector<Agnode_t*>> clusters;

		for (Agnode_t* node = agfstnode(g); node; node = agnxtnode(g, node)) {
			if (!visited[node]) {
				std::vector<Agnode_t*> component;
				dfs(node, visited, component, g);
				clusters.push_back(component);
			}
		}

		return clusters;
	}

	//Adds new systems to .dot file if needed.
	class SystemCallsGraphDescriptionGenerator {
	public:

		Agraph_t* CreateSystemsGraph(std::shared_ptr<ProjectContext> projectContext) {

			// �������� ������ �����
			Agraph_t* g = agopen((char*)"G", Agstrictundirected, nullptr);

			projectContext->ForEachSystemEcsFile([&](std::shared_ptr<ParsedECSFile> systemEcs) {

				if ("OksEngine.ProcessInput.ecs" == systemEcs->GetPath().filename()) {
					//__debugbreak();
				}

				auto system = std::dynamic_pointer_cast<ParsedSystemECSFile>(systemEcs);
				//Do not add system that will be call manualy.
				if (system->ci_.type_ == ParsedSystemECSFile::SystemType::Initialize/* || system->GetThread() == ParsedSystemECSFile::Thread::Main*/) {
					return;
				}
				Agnode_t* systemNode = agnode(g, (char*)system->GetName().c_str(), 1);
				agsafeset(systemNode, (char*)"shape", (char*)"rect", (char*)"");
				for (auto& entity : system->ci_.processesEntities_) {
					entity.ForEachInclude([&](const ParsedSystemECSFile::Include& include, bool isLast) {

						projectContext->ForEachSystemEcsFile([&](std::shared_ptr<ParsedECSFile> systemEcsFile) {
							auto maybeDependenceSystem = std::dynamic_pointer_cast<ParsedSystemECSFile>(system);
							//Do not add system that will be call manualy.
							if (system->ci_.type_ == ParsedSystemECSFile::SystemType::Initialize/* || system->GetThread() == ParsedSystemECSFile::Thread::Main*/) {
								return;
							}
							if (system == maybeDependenceSystem) { return; }
							if (system->GetName() == "UpdateDriverIndexBuffer" && maybeDependenceSystem->GetName() == "CreateUniformBuffer") {
								systemNode = systemNode;
							}
							Agnode_t* maybeDependenceSystemNode = agnode(g, (char*)maybeDependenceSystem->GetName().c_str(), 1);
							agsafeset(maybeDependenceSystemNode, (char*)"shape", (char*)"rect", (char*)"");
							for (auto& entity : maybeDependenceSystem->ci_.processesEntities_) {
								entity.ForEachInclude([&](const ParsedSystemECSFile::Include& dependenceSystemInclude, bool isLast) {
									if (include.name_ == dependenceSystemInclude.name_/* && dependenceSystemInclude.name_ == "RenderDriver"*/) {
										if (include.readonly_ != dependenceSystemInclude.readonly_ || !(include.readonly_ && dependenceSystemInclude.readonly_)) {
											Agedge_t* edge = agedge(g, systemNode, maybeDependenceSystemNode, nullptr, 0);
											if (edge == nullptr) {
												agedge(g, systemNode, maybeDependenceSystemNode, nullptr, 1);
												//	//agsafeset(e, (char*)"dir", (char*)"both", (char*)""); 
											}
										}
									}
									return true;
									});
							}

							});

						return true;
						});
				}

				});

			return g;

		}

		std::vector<std::vector<Agnode_t*>> FindClusters(Agraph_t* g) {

			std::vector<std::string> graphvizColors = {
	"white", "red", "green", "blue", "yellow",
	"cyan", "magenta", "lightgray", "darkgray", "lightred",
	"lightgreen", "lightblue", "darkred", "darkgreen", "darkblue",
	"pink", "orange", "brown", "purple", "violet",
	"turquoise", "goldenrod ", "salmon", "khaki", "plum",
	"coral", "forestgreen", "olive", "navy", "indigo"
			};

			std::vector<std::vector<Agnode_t*>> clusters = findConnectedComponents(g);

			// ������ ���������
			for (size_t i = 0; i < clusters.size(); ++i) {


				std::cout << "Cluster " << i + 1 << ": ";
				for (Agnode_t* node : clusters[i]) {
					char* color = (i < graphvizColors.size()) ? ((char*)graphvizColors[i].c_str()) : ((char*)"red");
					agsafeset(node, (char*)"style", (char*)"filled", (char*)"");
					agsafeset(node, (char*)"fillcolor", (char*)color, "");
					std::cout << agnameof(node) << " ";
				}
				std::cout << std::endl;
			}

			return clusters;
		}

		std::shared_ptr<OS::TextFile> GenerateGraphText(Agraph_t* g, std::shared_ptr<ProjectContext> projectContext) {


			//Parse .dot
			GVC_t* gvc = gvContext();


			//Get path
			auto randomEcsFilePath = projectContext->nameEcsFile_.begin()->second->GetPath();

			std::filesystem::path includeDirFullPath;

			std::filesystem::path::iterator includeDirIt;
			for (auto it = randomEcsFilePath.end(); it != randomEcsFilePath.begin(); --it) {
				auto folder = *it;
				if (folder == projectContext->includeDirectory_) {
					includeDirIt = it;
					break;
				}
			}

			for (auto it = randomEcsFilePath.begin(); it != includeDirIt; it++) {
				includeDirFullPath /= *it;
			}

			includeDirFullPath /= *includeDirIt;
			//Get path


			auto dotfile = std::make_shared<OS::TextFile>(includeDirFullPath / "auto_ECSSystemsGraph.dot");

			char* dotData = nullptr;
			unsigned int length = 0;

			//agsafeset(g, (char*)"rankdir", (char*)"LR", (char*)"");
			//agsafeset(g, (char*)"splines", (char*)"ortho", (char*)"");
			//agsafeset(g, (char*)"ep", (char*)"0.5", (char*)"");  // ���������� ����� ������
			//agsafeset(g, (char*)"ranksep", (char*)"10 equally", (char*)""); // ���������� ����� �������


			gvLayout(gvc, g, "dot");
			gvRenderData(gvc, g, "dot", &dotData, &length);

			agclose(g);
			gvFreeContext(gvc);
			dotfile->Create();
			std::string dotText{ dotData };
			(*dotfile) << dotText;
			dotfile->Close();

			return dotfile;
		}
	};

}