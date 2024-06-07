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

			for (Common::Index i = offset; i < newSize; i++) {
				indices_[i] += GetVerticesNumber();
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
			text_.Resize(size);
			std::memcpy(text_.GetData(), text, size);
		}

		Shader(const Shader& copyShader) {
			text_ = copyShader.text_;
		}

		const Common::Byte* GetCode() const {
			return text_.GetData();
		}

		Common::Size GetSize() const {
			return text_.GetSize();
		}

		~Shader() noexcept = default;
	private:
		DS::Vector<Common::Byte> text_;
	};

	struct RenderSurface {
		std::any param1_;
		std::any param2_;
		std::any param3_;
		UISubsystem uiSubsystem_;
		//If WINApi
		// param1_ == HWND
		// param2_ == HINSTANCE
		//If GLFW
		// param1_ == GLFWwindow*
	};

	class Driver {
	public:

		struct CreateInfo {
			Shader vertexShader_;
			Shader fragmentShader_;
			RenderSurface surface_;
			bool enableDepthBuffer_ = false;
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

		virtual void DrawIndexed(
			const RAL::Vertex3fnñt* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber,
			const Texture& texture) = 0;

		virtual void DrawIndexed(
			const RAL::Vertex3fnt* vertices,
			Common::Size verticesNumber,
			const RAL::Index16* indices,
			Common::Size indicesNumber,
			const Texture& texture) = 0;

		virtual void DrawIndexed(
			const Vertex3fnc* vertices,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indicesNumber) = 0;

		virtual void DrawIndexed(
			const Vertex3fc* vertex,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indeciesNumber) = 0;

		virtual void DrawIndexed(
			const Vertex3f* vertex,
			Common::Size verticesNumber,
			const Index16* indices,
			Common::Size indeciesNumber,
			const Color& color) = 0;

		virtual ~Driver() = default;

	protected:
		CreateInfo createInfo_;
		std::shared_ptr<Light> light_ = nullptr;
		std::shared_ptr<Camera> camera_ = nullptr;
		Vertices<Geometry::Vertex3fnct, Index16> vertices_;
		Indices<Index16> indices_;
	};

}