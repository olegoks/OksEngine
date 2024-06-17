#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>

#include <OS.hpp>
#include <Math.Scalar.hpp>
#include <Geometry.Vertex.hpp>
#include <Geometry.Index.hpp>
#include <Geometry.Shape.hpp>
#include <Geometry.VertexCloud.hpp>
#include <Geometry.IndexBuffer.hpp>

namespace Geometry {

    //class Object {
    //public:
    //private:
    //    DS::Vector<  >

    //};

    template<class VertexType = Vertex3fnc, class IndexType = Index16>
    class Model {
    public:

        using Shape = Shape<VertexType, IndexType>;

        Model() noexcept = default;

        Model(Model&& modelMove) noexcept : shapes_{ std::move(modelMove.shapes_) } {};

        void AddShape(const Shape& shape) noexcept {
            shapes_.push_back(shape);
        }

        using iterator = std::vector<Shape>::iterator;
        using const_iterator = const iterator;

        iterator begin() noexcept { return shapes_.begin(); }
        iterator end() noexcept { return shapes_.end(); }

        const_iterator begin() const noexcept { return const_cast<Model*>(this)->begin(); }
        const_iterator end() const noexcept { return const_cast<Model*>(this)->end(); }

        [[nodiscard]]
        Common::Size GetShapesNumber() const noexcept { return shapes_.size(); }

    private:
       // DS::Vector<Shape>
        std::vector<Shape> shapes_;
    };


    //class Model {
    //public:
    //private:
    //    DS::Vector<Shape3f> shapes3f_;
    //};

    Model<Vertex3fnc, Index16> ParseObj(const std::string& obj, const std::string& mtl);

}