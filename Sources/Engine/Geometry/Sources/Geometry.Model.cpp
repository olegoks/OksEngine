
#include <Geometry.Model.hpp>

#include <unordered_map>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

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
    Vertex3f GetVertex<Vertex3f>(tinyobj::index_t index, const tinyobj::attrib_t& atrributes) noexcept {
        Vertex3f vertex{};

        if (index.vertex_index >= 0) {
            vertex.position_ = {
                atrributes.vertices[3 * index.vertex_index + 0],
                atrributes.vertices[3 * index.vertex_index + 1],
                atrributes.vertices[3 * index.vertex_index + 2],
            };
        }
        return vertex;
    }


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

    Model<Vertex3fnt, Index16> ParseObjVertex3fnt(const std::string& obj, const std::string& mtl, const std::string& texture) noexcept {

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
        
            auto textureObject = std::make_shared<Geom::Texture<Geom::Color4b>>(std::move(Geom::CreateTexture(texture.data(), texture.size())));
            Shape<Vertex3fnt, Index16> shape{ vertices, indices, textureObject };
            model.AddShape(shape);
        }
        OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");
        return model;
    }

    Model<Vertex3f, Index16> ParseObjVertex3fIndex16(const std::string& obj) {

        tinyobj::ObjReader objReader;
        objReader.ParseFromString(obj, "");

        OS::AssertMessage(objReader.Valid(),
            "Attempt to use incorrect .obj/mtl model file.");

        const auto& attributes = objReader.GetAttrib();
        auto shapes = objReader.GetShapes();
        //auto materials = objReader.GetMaterials();


        Model<Vertex3f, Index16> model;

        for (const auto& objShape : shapes) {
            std::unordered_map<Vertex3f, Index16, Vertex3f::Hash> uniqueVertices{};
            VertexCloud<Vertex3f> vertices;
            IndexBuffer<Index16> indices;
            Common::Index vertexIndex = 0;
            for (const auto& index : objShape.mesh.indices) {
                auto vertex = GetVertex<Vertex3f>(index, attributes);

                glm::vec3 axis(1.0f, 0.0f, 0.0f);

                float angle = glm::radians(-90.0f);

                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

                {
                    glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex.position_.GetX(), vertex.position_.GetY(), vertex.position_.GetZ(), 1.0f);
                    vertex.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
                }

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

            //auto textureObject = std::make_shared<Geom::Texture<Geom::Color4b>>(std::move(Geom::CreateTexture(texture.data(), texture.size())));
            Shape<Vertex3f, Index16> shape{ vertices, indices ,""};
            model.AddShape(shape);
        }
        OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");

        OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");
        return model;
    }


    Model<Vertex3fnt, Index16> ParseObjVertex3fntIndex16Textures(const std::string& obj, const std::string& mtl) {

        tinyobj::ObjReader objReader;
        objReader.ParseFromString(obj, mtl);

        OS::AssertMessage(objReader.Valid(),
            "Attempt to use incorrect .obj/mtl model file.");

        const auto& attributes = objReader.GetAttrib();
        auto shapes = objReader.GetShapes();
        auto materials = objReader.GetMaterials();


        Model<Vertex3fnt, Index16> model;

        for (int id = 0; id < materials.size(); id++) {
            const tinyobj::material_t& material = materials[id];
            VertexCloud<Vertex3fnt> vertices;
            IndexBuffer<Index16> indices;
            for (const auto& objShape : shapes) {
                for (const auto verticesPerFace : objShape.mesh.num_face_vertices) {
                    OS::Assert(verticesPerFace == 3);
                }
                for (std::size_t i = 0; i < objShape.mesh.material_ids.size(); i++) {
                    const int materialId = objShape.mesh.material_ids[i];
                    if (materialId == id) {
                        const std::size_t faceIndex = i;
                        const tinyobj::index_t Vertex1Index = objShape.mesh.indices[faceIndex * 3 + 0];
                        const tinyobj::index_t Vertex2Index = objShape.mesh.indices[faceIndex * 3 + 1];
                        const tinyobj::index_t Vertex3Index = objShape.mesh.indices[faceIndex * 3 + 2];
                        auto vertex1 = GetVertex<Vertex3fnt>(Vertex1Index, attributes);
                        auto vertex2 = GetVertex<Vertex3fnt>(Vertex2Index, attributes);
                        auto vertex3 = GetVertex<Vertex3fnt>(Vertex3Index, attributes);

                        glm::vec3 axis(1.0f, 0.0f, 0.0f);

                        float angle = glm::radians(-90.0f);

                        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);

                        {
                            glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex1.position_.GetX(), vertex1.position_.GetY(), vertex1.position_.GetZ(), 1.0f);
                            vertex1.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
                        }
                        {
                            glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex2.position_.GetX(), vertex2.position_.GetY(), vertex2.position_.GetZ(), 1.0f);
                            vertex2.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
                        }
                        {
                            glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertex3.position_.GetX(), vertex3.position_.GetY(), vertex3.position_.GetZ(), 1.0f);
                            vertex3.position_ = { rotatedVertex.x, rotatedVertex.y, rotatedVertex.z };
                        }
                        vertices.Add(vertex1);
                        const Index16 firstIndex = vertices.GetVerticesNumber() - 1;
                        vertices.Add(vertex2);
                        vertices.Add(vertex3);

                        indices.Add(firstIndex);
                        indices.Add(firstIndex + 1);
                        indices.Add(firstIndex + 2);
                    }
                }

            }
            Shape<Vertex3fnt, Index16> shape{ vertices, indices,material.diffuse_texname };
            shape.textureName_ = material.diffuse_texname;
            model.AddShape(shape);
        }

        OS::AssertMessage(model.GetShapesNumber() > 0, "Attempt to parse .obj file with no geometry.");
        return model;
    }

    Model<Vertex3fnc, Index16> ParseObjVertex3fncIndex16(const std::string& obj, const std::string& mtl)
    {
        return ParseObj(obj, mtl);
    }

    Model<Vertex3fnt, Index16> ParseObjVertex3fntIndex16(const std::string& obj, const std::string& mtl, const std::string& image)
    {
        return ParseObjVertex3fnt(obj, mtl, image);
    }

    

    bool ParseModel(const char* memory, Common::Size size) {

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFileFromMemory(memory, size,
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);
        if (scene == nullptr) {
            OS::AssertFailMessage(importer.GetErrorString());
        }
        // If the import failed, report it
        if (nullptr == scene) {
            //DoTheErrorLogging(importer.GetErrorString());
            return false;
        }

        // Now we can access the file's contents.
        //DoTheSceneProcessing(scene);

        // We're done. Everything will be cleaned up by the importer destructor
        return true;
    }


    bool ParseModelFile(const std::filesystem::path& filePath) {

        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(filePath.string().c_str(),
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType);
        if (scene == nullptr) {
            OS::AssertFailMessage(importer.GetErrorString());
        }
        // If the import failed, report it
        if (nullptr == scene) {
            //DoTheErrorLogging(importer.GetErrorString());
            return false;
        }

        // Now we can access the file's contents.
        //DoTheSceneProcessing(scene);

        // We're done. Everything will be cleaned up by the importer destructor
        return true;
    }

}