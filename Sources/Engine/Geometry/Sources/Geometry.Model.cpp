
#include <Geometry.Model.hpp>

#include <unordered_map>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace Geometry {




    template<class VertexType>
    VertexType GetVertex(tinyobj::index_t index, const tinyobj::attrib_t& atrributes) noexcept {
        OS::NotImplemented();
        return VertexType{};
    }

    //template<>
    //inline Vertex3fnct GetVertex<Vertex3fnct>(tinyobj::index_t index, tinyobj::attrib_t& atrributes) noexcept {
    //    Vertex3fnct vertex{};

    //    if (index.vertex_index >= 0) {
    //        vertex.position_ = {
    //            atrributes.vertices[3 * index.vertex_index + 0],
    //            atrributes.vertices[3 * index.vertex_index + 1],
    //            atrributes.vertices[3 * index.vertex_index + 2],
    //        };
    //        vertex.color_ = {
    //            atrributes.colors[3 * index.vertex_index + 0],
    //            atrributes.colors[3 * index.vertex_index + 1],
    //            atrributes.colors[3 * index.vertex_index + 2],
    //        };
    //        vertex.texel_ = {
    //            atrributes.texcoords[2 * index.texcoord_index + 0],
    //            1.0f - atrributes.texcoords[2 * index.texcoord_index + 1]
    //        };
    //    }

    //    if (index.normal_index >= 0) {
    //        vertex.normal_ = {
    //            atrributes.normals[3 * index.normal_index + 0],
    //            atrributes.normals[3 * index.normal_index + 1],
    //            atrributes.normals[3 * index.normal_index + 2],
    //        };
    //        vertex.normal_.Normalize();
    //    }
    //    return vertex;
    //}

    template<>
    Vertex3fnt GetVertex<Vertex3fnt>(tinyobj::index_t index, const tinyobj::attrib_t& atrributes) noexcept {
        Vertex3fnt vertex{};

        if (index.vertex_index >= 0) {
            vertex.position_ = {
                atrributes.vertices[3 * index.vertex_index + 0],
                atrributes.vertices[3 * index.vertex_index + 1],
                atrributes.vertices[3 * index.vertex_index + 2],
            };

            vertex.uv_ = {
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
    inline Vertex3fnc GetVertex<Vertex3fnc>(tinyobj::index_t index, const tinyobj::attrib_t& atrributes) noexcept {
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


    //template<>
    //inline Vertex3fnct GetVertex<Vertex3fnct>(tinyobj::index_t index, tinyobj::attrib_t& atrributes) noexcept;

    //template<>
    //Vertex3fnt GetVertex<Vertex3fnt>(tinyobj::index_t index, tinyobj::attrib_t& atrributes) noexcept;

    //template<>
    //inline Vertex3fnc GetVertex<Vertex3fnc>(tinyobj::index_t index, tinyobj::attrib_t& atrributes) noexcept;

    //template<class VertexType = Vertex3fnc, class IndexType = Index16>
    //inline Model<VertexType, IndexType> LoadObj(const std::filesystem::path& objPath) noexcept {

    //    tinyobj::attrib_t objAttrib;
    //    std::vector<tinyobj::shape_t> objShapes;
    //    std::vector<tinyobj::material_t> objMaterials;
    //    std::string warn, err;

    //    if (!tinyobj::LoadObj(&objAttrib, &objShapes, &objMaterials, &warn, &err, objPath.string().c_str())) {
    //        OS::AssertFailMessage({ "Error while loading model %s.", objPath.string().c_str() });
    //    }

    //    OS::LogInfo("geometry/loading", { "Obj %s was loaded. Number of shapes %d.", objPath.string().c_str(), objShapes.size() });


    //    Model<VertexType, IndexType> model;

    //    std::unordered_map<VertexType, IndexType, VertexType::Hash> uniqueVertices{};
    //    for (const auto& objShape : objShapes) {
    //        VertexCloud<VertexType> vertices;
    //        IndexBuffer<IndexType> indices;

    //        for (const auto& index : objShape.mesh.indices) {
    //            auto vertex = GetVertex<VertexType>(index, objAttrib);
    //            bool deleteDuplicateVertices = true;
    //            if (deleteDuplicateVertices) {
    //                if (uniqueVertices.count(vertex) == 0) {
    //                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.GetVerticesNumber());
    //                    vertices.Add(vertex);
    //                }
    //                indices.Add(uniqueVertices[vertex]);
    //            }
    //            else {
    //                vertices.Add(vertex);
    //                indices.Add(vertices.GetVerticesNumber() - 1);
    //            }
    //        }
    //        Shape<VertexType, IndexType> shape{ vertices, indices };
    //        model.AddShape(shape);
    //    }
    //    OS::AssertMessage(model.GetShapesNumber() > 0, { "Empty model %s was loaded.", objPath.string().c_str() });
    //    return model;
    //}

    Model<Vertex3fnc, Index16> ParseObj(const std::string& obj, const std::string& mtl) noexcept {

        tinyobj::ObjReader objReader;
        objReader.ParseFromString(obj, mtl);
        OS::AssertMessage(objReader.Valid(),
            "Attempt to use incorrect .obj/mtl model file.");
        const auto& attributes = objReader.GetAttrib();
        auto shapes = objReader.GetShapes();
        auto materials = objReader.GetMaterials();


        Model<Vertex3fnc, Index16> model;
 

        std::unordered_map<Vertex3fnc, Index16, Vertex3fnc::Hash> uniqueVertices{};
        for (const auto& objShape : shapes) {
            VertexCloud<Vertex3fnc> vertices;
            IndexBuffer<Index16> indices;
            Common::Index vertexIndex = 0;
            for (const auto& index : objShape.mesh.indices) {
                auto vertex = GetVertex<Vertex3fnc>(index, attributes);
                auto& material = materials[objShape.mesh.material_ids[vertexIndex/ 3]];
                vertex.color_ = Geom::Color3f{ material.diffuse[0], material.diffuse[1], material.diffuse[2] };
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
                vertexIndex++;
            }
            Shape<Vertex3fnc, Index16> shape{ vertices, indices };
            model.AddShape(shape);
        }
        OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");
        return model;
    }

    Model<Vertex3fnt, Index16> ParseObjVertex3fnt(const std::string& obj, const std::string& mtl) noexcept {

        tinyobj::ObjReader objReader;
        objReader.ParseFromString(obj, mtl);
        OS::AssertMessage(objReader.Valid(),
            "Attempt to use incorrect .obj/mtl model file.");
        const auto& attributes = objReader.GetAttrib();
        auto shapes = objReader.GetShapes();
        auto materials = objReader.GetMaterials();


        Model<Vertex3fnt, Index16> model;


        std::unordered_map<Vertex3fnt, Index16, Vertex3fnt::Hash> uniqueVertices{};
        for (const auto& objShape : shapes) {
            VertexCloud<Vertex3fnt> vertices;
            IndexBuffer<Index16> indices;
            Common::Index vertexIndex = 0;
            for (const auto& index : objShape.mesh.indices) {
                auto vertex = GetVertex<Vertex3fnt>(index, attributes);
                //auto& material = materials[objShape.mesh.material_ids[vertexIndex / 3]];
                //vertex.color_ = Geom::Color3f{ material.diffuse[0], material.diffuse[1], material.diffuse[2] };
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
                vertexIndex++;
            }
            Shape<Vertex3fnt, Index16> shape{ vertices, indices };
            model.AddShape(shape);
        }
        OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");
        return model;
    }

    Model<Vertex3fnc, Index16> ParseObjVertex3fncIndex16(const std::string& obj, const std::string& mtl)
    {
        return ParseObj(obj, mtl);
    }

    Model<Vertex3fnt, Index16> ParseObjVertex3fntIndex16(const std::string& obj, const std::string& mtl)
    {
        return ParseObjVertex3fnt(obj, mtl);
    }

    
}