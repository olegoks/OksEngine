#pragma once

#include <memory>
#include <string>
#include <vector>
#include <any>

#include <OS.Assert.hpp>
#include <Common.Types.hpp>
#include <Math.Vector.hpp>
#include <Math.Matrix.hpp>
#include <RAL.Camera.hpp>
#include <RAL.Light.hpp>
#include <RAL.Common.hpp>
#include <Geometry.Vertex.hpp>
#include <DataStructures.Vector.hpp>
#include <GLFW\glfw3.h>
#include <RAL.Model.hpp>

namespace RAL {

	template<class VertexType, class IndexType>
	class Vertices {
	public:

		void Add(const VertexType& vertex) noexcept {
			Add(&vertex, 1);
		}

		void Add(const VertexType* vertices, Common::Size verticesNumber) noexcept {

			OS::Assert(vertices != nullptr);

			const Common::Size offset = GetVerticesNumber();
			const Common::Size newSize = GetVerticesNumber() + verticesNumber;
			vertices_.resize(newSize);
			std::memcpy(vertices_.data() + offset, vertices, verticesNumber * sizeof(VertexType));
			verticesNumber_ += verticesNumber;
		}

		[[nodiscard]]
		Common::Size GetVerticesNumber() const noexcept { return verticesNumber_; }

		[[nodiscard]]
		Common::Size GetSizeInBytes() const noexcept { return GetVerticesNumber() * sizeof(VertexType); }


		void Clear() noexcept {
			verticesNumber_ = 0;
		}

		[[nodiscard]]
		const VertexType* GetData() const noexcept { return vertices_.data(); }

		using iterator = std::vector<VertexType>::iterator;
		using const_iterator = const iterator;

		iterator begin() noexcept { return vertices_.begin(); }
		iterator end() noexcept { return vertices_.end(); };

		const_iterator begin() const noexcept { return const_cast<Vertices*>(this)->begin(); }
		const_iterator end() const noexcept { return const_cast<Vertices*>(this)->end(); }

	private:
		Common::Size verticesNumber_ = 0;
		std::vector<VertexType> vertices_;
	};

	template<class IndexType = Index16>
	class Indices {
	public:

		void Add(const IndexType* indices, Common::Size indicesNumber, Common::Size verticesNumber) noexcept {

			OS::Assert(indices != nullptr);

			const Common::Size offset = GetIndicesNumber();
			const Common::Size newSize = GetIndicesNumber() + indicesNumber;
			indices_.resize(newSize);
			std::memcpy(indices_.data() + offset, indices, indicesNumber * sizeof(IndexType));

			OS::AssertMessage(verticesNumber <= Common::Limits<IndexType>::Max(), 
				"Number of vertices is more than Index type can contain.");
			for (Common::Index i = offset; i < newSize; i++) {
				indices_[i] += static_cast<IndexType>(GetVerticesNumber());
			}
			verticesNumber_ += verticesNumber;
			indicesNumber_ += indicesNumber;
		}

		void Clear() noexcept {
			verticesNumber_ = 0;
			indicesNumber_ = 0;
		}

		[[nodiscard]]
		Common::Size GetVerticesNumber() const noexcept { return verticesNumber_; }

		[[nodiscard]]
		Common::Size GetIndicesNumber() const noexcept { return indicesNumber_; }

		[[nodiscard]]
		Common::Size GetSizeInBytes() const noexcept { return GetIndicesNumber() * sizeof(IndexType); }

		[[nodiscard]]
		const IndexType* GetData() const noexcept { return indices_.data(); }

	private:
		Common::Size verticesNumber_ = 0;
		Common::Size indicesNumber_ = 0;
		std::vector<IndexType> indices_;
	};

}

namespace RAL {
	
	enum class UISubsystem {
		WinAPI,
		GLFW
	};

	class Shader {
	public:

		Shader() noexcept = default;

		Shader(const Common::Byte* text, Common::Size size) {
			spirv_.resize(size);
			std::memcpy(spirv_.data(), text, size);
		}

		Shader(const Shader& copyShader) {
			spirv_ = copyShader.spirv_;
		}

		[[nodiscard]]
		std::vector<Common::Byte> GetCode() const noexcept {
			return spirv_;
		}

		~Shader() noexcept = default;
	private:
		std::vector<Common::Byte> spirv_;
	};

	struct RenderSurface {
		std::any param1_;
		std::any param2_;
		std::any param3_;
		std::any param4_;
		Math::Vector2u32 size_{ 0, 0 };
		UISubsystem uiSubsystem_;
		//If WINApi
		// param1_ == HWND
		// param2_ == HINSTANCE
		//If GLFW
		// param1_ == GLFWwindow*
		// param2_ == requiredExtensionsCound uint32
		// param3_ == requiredExtensions const char**
	};

	class Driver {
	public:

		struct Pipeline {
			std::string name_ = "No name";
			std::shared_ptr<Shader> vertexShader_ = nullptr;
			std::shared_ptr<Shader> fragmentShader_ = nullptr;
			bool enableDepthTest_ = true;
		};

		struct CreateInfo {
			std::shared_ptr<Pipeline> imguiNativePipeline_ = nullptr;
			std::shared_ptr<Pipeline> imguiPipeline_ = nullptr;
			std::shared_ptr<Pipeline> linesPipeline_ = nullptr;
			std::shared_ptr<Pipeline> texturedPipeline_ = nullptr;
			std::shared_ptr<Pipeline> flatShadedPipeline_ = nullptr;
			RenderSurface surface_;
		};

		Driver(const CreateInfo& createInfo) noexcept : createInfo_{ createInfo } { }

		virtual void StartRender() = 0;
		virtual void Render() = 0;
		virtual void EndRender() = 0;


		void SetCamera(std::shared_ptr<Camera> camera) noexcept {
			camera_ = camera;
		}

		void AddLight(std::shared_ptr<Light> light) noexcept {
			light_ = light;
		}

		std::shared_ptr<Camera> GetCamera() noexcept {
			return camera_;
		}

		virtual Common::UInt64 DrawIndexed(
			glm::vec2 scale,
			glm::vec2 translate,
			const Vertex2ftc* vertices,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indicesNumber,
			std::shared_ptr<RAL::Texture> texture) = 0;

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const Vertex3fnt* vertices,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indicesNumber,
			std::shared_ptr<RAL::Texture> texture) = 0;

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const Vertex3fnc* vertices,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indicesNumber) = 0;

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const Vertex3fc* vertex,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indeciesNumber) = 0;

		virtual Common::UInt64 DrawIndexed(
			const glm::mat4& model_,
			const Vertex3f* vertex,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indeciesNumber,
			const Color3f& color) = 0;


		//virtual void SetPosition(
		//	Common::Index shapeIndex,
		//	const Vector3f& position) = 0;

		virtual void SetModelMatrix(
			Common::Index shapeIndex,
			const glm::mat4& modelMatrix) = 0;

		//virtual void Rotate(
		//	Common::Index shapeIndex,
		//	const Vector3f& aroundVector,
		//	Math::Angle angle) = 0;

		virtual ~Driver() = default;

	protected:
		CreateInfo createInfo_;
		std::shared_ptr<Light> light_ = nullptr;
		std::shared_ptr<Camera> camera_ = nullptr;
	};




}