#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <OS.hpp>
#include <Math.Scalar.hpp>
#include <Geometry.Vertex.hpp>
#include <Geometry.Index.hpp>
#include <Geometry.Shape.hpp>
#include <Geometry.VertexCloud.hpp>
#include <Geometry.IndexBuffer.hpp>

namespace Geometry {

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
        std::vector<Shape> shapes_;
    };

    template<class VertexType>
    VertexType GetVertex(tinyobj::index_t index, tinyobj::attrib_t& atrributes) noexcept {
        OS::NotImplemented();
        return VertexType{};
    }


    template<>
    Vertex3fnct GetVertex<Vertex3fnct>(tinyobj::index_t index, tinyobj::attrib_t& atrributes) noexcept {
        Vertex3fnct vertex{};

        if (index.vertex_index >= 0) {
            vertex.position_ = {
                atrributes.vertices[3 * index.vertex_index + 0],
                atrributes.vertices[3 * index.vertex_index + 1],
                atrributes.vertices[3 * index.vertex_index + 2],
            };
            vertex.color_ = {
                atrributes.colors[3 * index.vertex_index + 0],
                atrributes.colors[3 * index.vertex_index + 1],
                atrributes.colors[3 * index.vertex_index + 2],
            };
            vertex.texel_ = {
                atrributes.texcoords[2 * index.texcoord_index + 0],
                1.0f - atrributes.texcoords[2 * index.texcoord_index + 1]
            };
        }

        if (index.normal_index >= 0) {
            vertex.normal_ = {
                atrributes.normals[3 * index.normal_index + 0],
                atrributes.normals[3 * index.normal_index + 1],
                atrributes.normals[3 * index.normal_index + 2],
            };
            vertex.normal_.Normalize();
        }
        return vertex;
    }

    template<>
    Vertex3fnt GetVertex<Vertex3fnt>(tinyobj::index_t index, tinyobj::attrib_t& atrributes) noexcept {
        Vertex3fnt vertex{};

        if (index.vertex_index >= 0) {
            vertex.position_ = {
                atrributes.vertices[3 * index.vertex_index + 0],
                atrributes.vertices[3 * index.vertex_index + 1],
                atrributes.vertices[3 * index.vertex_index + 2],
            };

            vertex.texel_ = {
                atrributes.texcoords[2 * index.texcoord_index + 0],
                1.0f - atrributes.texcoords[2 * index.texcoord_index + 1]
            };
        }

        if (index.normal_index >= 0) {
            vertex.normal_ = {
                atrributes.normals[3 * index.normal_index + 0],
                atrributes.normals[3 * index.normal_index + 1],
                atrributes.normals[3 * index.normal_index + 2],
            };
            vertex.normal_.Normalize();
        }
        return vertex;
    }

    template<>
    Vertex3fnc GetVertex<Vertex3fnc>(tinyobj::index_t index, tinyobj::attrib_t& atrributes) noexcept {
        Vertex3fnc vertex{};

        if (index.vertex_index >= 0) {
            vertex.position_ = {
                atrributes.vertices[3 * index.vertex_index + 0],
                atrributes.vertices[3 * index.vertex_index + 1],
                atrributes.vertices[3 * index.vertex_index + 2],
            };

            vertex.color_ = {
                atrributes.colors[3 * index.vertex_index + 0],
                atrributes.colors[3 * index.vertex_index + 1],
                atrributes.colors[3 * index.vertex_index + 2],
            };
        }

        if (index.normal_index >= 0) {
            vertex.normal_ = {
                atrributes.normals[3 * index.normal_index + 0],
                atrributes.normals[3 * index.normal_index + 1],
                atrributes.normals[3 * index.normal_index + 2],
            };
            vertex.normal_.Normalize();
        }
        return vertex;
    }

    template<class VertexType = Vertex3fnc, class IndexType = Index16>
    Model<VertexType, IndexType> LoadObj(const std::filesystem::path& objPath) noexcept {

        tinyobj::attrib_t objAttrib;
        std::vector<tinyobj::shape_t> objShapes;
        std::vector<tinyobj::material_t> objMaterials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&objAttrib, &objShapes, &objMaterials, &warn, &err, objPath.string().c_str())) {
            OS::AssertFailMessage({ "Error while loading model %s.", objPath.string().c_str() });
        }

        OS::LogInfo("geometry/loading", { "Obj %s was loaded. Number of shapes %d.", objPath.string().c_str(), objShapes.size()});
        

        Model<VertexType, IndexType> model;

        std::unordered_map<VertexType, IndexType, VertexType::Hash> uniqueVertices{};
        for (const auto& objShape : objShapes) {
            VertexCloud<VertexType> vertices;
            IndexBuffer<IndexType> indices;

            for (const auto& index : objShape.mesh.indices) {
                /*Vertex3fnc vertex{};

                if (index.vertex_index >= 0) {
                    vertex.position_ = {
                        objAttrib.vertices[3 * index.vertex_index + 0],
                        objAttrib.vertices[3 * index.vertex_index + 1],
                        objAttrib.vertices[3 * index.vertex_index + 2],
                    };

                    vertex.color_ = {
                        objAttrib.colors[3 * index.vertex_index + 0],
                        objAttrib.colors[3 * index.vertex_index + 1],
                        objAttrib.colors[3 * index.vertex_index + 2],
                    };
                }

                if (index.normal_index >= 0) {
                    vertex.normal_ = {
                        objAttrib.normals[3 * index.normal_index + 0],
                        objAttrib.normals[3 * index.normal_index + 1],
                        objAttrib.normals[3 * index.normal_index + 2],
                    };
                    vertex.normal_.Normalize();
                }*/

                auto vertex = GetVertex<VertexType>(index, objAttrib);
                bool deleteDuplicateVertices = true;
                if (deleteDuplicateVertices) {
                    if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.GetVerticesNumber());
                    vertices.Add(vertex);
                    }
                    indices.Add(uniqueVertices[vertex]);
                }
                else {
                    vertices.Add(vertex);
                    indices.Add(vertices.GetVerticesNumber() - 1);
                }

                //if (index.texcoord_index >= 0) {
                //    vertex.uv = {
                //        attrib.texcoords[2 * index.texcoord_index + 0],
                //        attrib.texcoords[2 * index.texcoord_index + 1],
                //    };
                //}
                 
                //if (uniqueVertices.count(vertex) == 0) {
                    //uniqueVertices[vertex] = static_cast<uint32_t>(vertices.GetVerticesNumber());
                    
                //}
                
            }
            Shape<VertexType, IndexType> shape{ vertices, indices };
            model.AddShape(shape);
        }
        OS::AssertMessage(model.GetShapesNumber() > 0, { "Empty model %s was loaded.", objPath.string().c_str() });
        return model;
    }

}