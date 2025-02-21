#include "stb_image.h"
#include <iostream>
#include "vk_loader.h"

#include "vk_engine.h"
#include "vk_initializers.h"
#include "vk_types.h"
#include <glm/gtx/quaternion.hpp>
//#include <fmt/format.h>

//#include <fastgltf/glm_element_traits.hpp>
//#include <fastgltf/core.hpp>
//#include <fastgltf/tools.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>


//std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshes(VulkanEngine* engine, std::filesystem::path filePath)
//{
//
//    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
//    if (!file) {
//        throw std::runtime_error("Failed to open GLB file.");
//    }
//
//    std::streamsize fileSize = file.tellg();
//    file.seekg(0, std::ios::beg);
//
//    std::vector<std::byte> buffer(fileSize);
//    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
//        throw std::runtime_error("Failed to read GLB file.");
//    }
//
//    auto data = fastgltf::GltfDataBuffer::FromBytes(buffer.data(),buffer.size());
//    if (data.error() != fastgltf::Error::None)
//    {
//        //fmt::println("Failed to load GLTF data from {}", filePath.string());
//        return std::nullopt;
//    }
//    constexpr auto gltfOptions = fastgltf::Options::LoadGLBBuffers | fastgltf::Options::LoadExternalBuffers;
//    fastgltf::Parser parser{};
//    fastgltf::Asset gltfAsset;
//    auto load = parser.loadGltfBinary(data.get(), filePath.parent_path(), gltfOptions);
//    if (load.error() == fastgltf::Error::None)
//    {
//        gltfAsset = std::move(load.get());
//    }
//    else
//    {
//        //fmt::println("Failed to parse GLTF binary from {}", filePath.string());
//        return std::nullopt;
//    }
//    std::vector<std::shared_ptr<MeshAsset>> meshes;
//
//    // Process the GLTF asset to extract mesh data
//    std::vector<uint32_t> indices;
//    std::vector<Vertex> vertices;
//
//    for (const auto& mesh : gltfAsset.meshes)
//    {
//        std::shared_ptr<MeshAsset> newMesh = std::make_shared<MeshAsset>();
//        // Populate meshAsset with data from mesh
//        newMesh->name = mesh.name;
//        indices.clear();
//        vertices.clear();
//        for (auto&& prim : mesh.primitives)
//        {
//            GeoSurface newSurface;
//            newSurface.startIndex = (uint32_t)indices.size();
//            newSurface.count = (uint32_t)gltfAsset.accessors[prim.indicesAccessor.value()].count;
//            size_t initialVerticesSize = vertices.size();
//            // load indices
//            {
//                fastgltf::Accessor& indexAccessor = gltfAsset.accessors[prim.indicesAccessor.value()];
//                indices.reserve(indices.size() + indexAccessor.count);
//                fastgltf::iterateAccessor<std::uint32_t>(gltfAsset, indexAccessor, [&](std::uint32_t index) {
//                    indices.push_back(index + initialVerticesSize);
//                    });
//            }
//            // load vertices
//            {
//                fastgltf::Accessor& positionAccessor =
//                    gltfAsset.accessors[prim.findAttribute("POSITION")->accessorIndex];
//
//                vertices.resize(vertices.size() + positionAccessor.count);
//                fastgltf::iterateAccessorWithIndex<glm::vec3>(
//                    gltfAsset, positionAccessor, [&](glm::vec3 v, size_t index) {
//                        auto& curVertex = vertices[initialVerticesSize + index];
//                        curVertex.position = v;
//                        curVertex.normal = { 1, 0, 0 };
//                        curVertex.color = glm::vec4(1.0f);
//                        curVertex.uv_x = 0;
//                        curVertex.uv_y = 0;
//                    });
//            }
//            // load normal
//            if (auto normalAccessor = prim.findAttribute("NORMAL"))
//            {
//                fastgltf::Accessor& normalAcc = gltfAsset.accessors[normalAccessor->accessorIndex];
//                fastgltf::iterateAccessorWithIndex<glm::vec3>(
//                    gltfAsset, normalAcc,
//                    [&](glm::vec3 n, std::size_t index) { vertices[initialVerticesSize + index].normal = n; });
//            }
//            // load UVs
//            if (auto uvAccessor = prim.findAttribute("TEXCOORD_0"))
//            {
//                fastgltf::Accessor& uvAcc = gltfAsset.accessors[uvAccessor->accessorIndex];
//                fastgltf::iterateAccessorWithIndex<glm::vec2>(gltfAsset, uvAcc, [&](glm::vec2 uv, std::size_t index) {
//                    vertices[initialVerticesSize + index].uv_x = uv.x;
//                    vertices[initialVerticesSize + index].uv_y = uv.y;
//                    });
//            }
//            // load vertex color
//            if (auto colorAccessor = prim.findAttribute("COLOR_0"))
//            {
//                fastgltf::Accessor& colorAcc = gltfAsset.accessors[colorAccessor->accessorIndex];
//                fastgltf::iterateAccessorWithIndex<glm::vec4>(
//                    gltfAsset, colorAcc,
//                    [&](glm::vec4 color, std::size_t index) { vertices[initialVerticesSize + index].color = color; });
//            }
//            newMesh->surfaces.push_back(newSurface);
//        }
//
//        /**
//         * @brief Display the vertex normal
//         */
//        constexpr bool overrideColors = true;
//        if (overrideColors)
//        {
//            for (auto& vertex : vertices)
//            {
//                vertex.color = glm::vec4(vertex.normal * 0.5f + 0.5f, 1.0f);
//            }
//        }
//        newMesh->meshBuffers = engine->uploadMesh(indices, vertices);
//        meshes.push_back(newMesh);
//    }
//
//    return meshes;
//}

std::optional<std::vector<std::shared_ptr<MeshAsset>>> loadGltfMeshesTinyGLTF(VulkanEngine* engine, const std::filesystem::path& filePath) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filePath.string());

    if (!ret) {
        return std::nullopt;
    }

    std::vector<std::shared_ptr<MeshAsset>> meshes;

    for (const auto& mesh : model.meshes) {
        std::shared_ptr<MeshAsset> newMesh = std::make_shared<MeshAsset>();
        newMesh->name = mesh.name;

        std::vector<uint32_t> indices;
        std::vector<Vertex> vertices;

        for (const auto& primitive : mesh.primitives) {
            GeoSurface newSurface;
            newSurface.startIndex = static_cast<uint32_t>(indices.size());

            // Load indices
            if (primitive.indices >= 0) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                indices.reserve(indices.size() + accessor.count);
                const uint16_t* data = reinterpret_cast<const uint16_t*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; i++) {
                    indices.push_back(static_cast<uint32_t>(data[i]));
                }
                newSurface.count = static_cast<uint32_t>(accessor.count);
            }

            size_t initialVerticesSize = vertices.size();

            // Load vertex positions
            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("POSITION")];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                vertices.resize(vertices.size() + accessor.count);
                const float* data = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; i++) {
                    vertices[initialVerticesSize + i].position = glm::vec3(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
                    vertices[initialVerticesSize + i].normal = glm::vec3(1, 0, 0);
                    vertices[initialVerticesSize + i].color = glm::vec4(1.0f);
                    vertices[initialVerticesSize + i].uv_x = 0;
                    vertices[initialVerticesSize + i].uv_y = 0;
                }
            }

            // Load normals
            if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("NORMAL")];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const float* data = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; i++) {
                    vertices[initialVerticesSize + i].normal = glm::vec3(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
                }
            }

            // Load UVs
            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const float* data = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; i++) {
                    vertices[initialVerticesSize + i].uv_x = data[i * 2];
                    vertices[initialVerticesSize + i].uv_y = data[i * 2 + 1];
                }
            }

            // Load colors
            if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("COLOR_0")];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                const float* data = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                for (size_t i = 0; i < accessor.count; i++) {
                    vertices[initialVerticesSize + i].color = glm::vec4(data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]);
                }
            }

            newMesh->surfaces.push_back(newSurface);
        }

        constexpr bool overrideColors = true;
        if (overrideColors) {
            for (auto& vertex : vertices) {
                vertex.color = glm::vec4(vertex.normal * 0.5f + 0.5f, 1.0f);
            }
        }

        newMesh->meshBuffers = engine->uploadMesh(indices, vertices);
        meshes.push_back(newMesh);
    }

    return meshes;
}
