#pragma once

#include <Geometry.Shape.hpp>
#include <Geometry.Storage.hpp>

namespace Geometry {

    //class Object {
    //public:
    //private:
    //    DS::Vector<  >

    //};

    //template<class VertexType = Vertex3fnc, class IndexType = Index16>
    //class Model {
    //public:

    //    Model() noexcept = default;

    //    Model(Model&& modelMove) noexcept : shapes_{ std::move(modelMove.shapes_) } {};

    //    void AddShape(const Shape& shape) noexcept {
    //        shapes_.push_back(shape);
    //    }

    //    using iterator = std::vector<Shape>::iterator;
    //    using const_iterator = const iterator;

    //    iterator begin() noexcept { return shapes_.begin(); }
    //    iterator end() noexcept { return shapes_.end(); }

    //    const_iterator begin() const noexcept { return const_cast<Model*>(this)->begin(); }
    //    const_iterator end() const noexcept { return const_cast<Model*>(this)->end(); }

    //    [[nodiscard]]
    //    Common::Size GetShapesNumber() const noexcept { return shapes_.size(); }

    //private:
    //    std::vector<Shape> shapes_;
    //};

    //Model<Vertex3fnc, Index16> ParseObjVertex3fncIndex16(const std::string& obj, const std::string& mtl);
    //Model<Vertex3fnt, Index16> ParseObjVertex3fntIndex16(const std::string& obj, const std::string& mtl, const std::string& image);
    //Model<Vertex3f,   Index16> ParseObjVertex3fIndex16(const std::string& obj);
    //Model<Vertex3fnt, Index16> ParseObjVertex3fntIndex16Textures(const std::string& obj, const std::string& mtl);

    bool ParseModelFbx(const char* memory, Common::Size size);
    [[nodiscard]]
    Geom::Model2 ParseFbxModelBaked(const std::string& fbxName, const std::string& fbx);
    [[nodiscard]]
    Geom::Model2 ParseObjMtlModelBaked(const std::string& objName, const std::string& obj, const std::string& mtlName, const std::string& mtl);
    Geom::Model2 ParseObjMtlModel(const std::string& objName, const std::string& obj, const std::string& mtlName, const std::string& mtl);
    bool ParseModelFile(const std::filesystem::path& filePath);
}
