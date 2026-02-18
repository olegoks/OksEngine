// #pragma once
// #include <Render\Pipeline\auto_OksEngine.Render.Pipeline.hpp>


// #include <magic_enum/magic_enum.hpp>

// namespace OksEngine
// {

// namespace Render
// {
// namespace Pipeline
// {
// VertexShader ParseVertexShader(luabridge::LuaRef &vertexShaderRef) {
	
// 	luabridge::LuaRef nameRef = vertexShaderRef["name"];
// 	return VertexShader{ nameRef.cast<std::string>().value() };

// };

// FragmentShader ParseFragmentShader(luabridge::LuaRef &fragmentShaderRef) {

// 	luabridge::LuaRef nameRef = fragmentShaderRef["name"];
// 	return FragmentShader{ nameRef.cast<std::string>().value() };

// };

// GeometryShader ParseGeometryShader(luabridge::LuaRef &geometryShaderRef) {

// 	luabridge::LuaRef nameRef = geometryShaderRef["name"];
// 	return GeometryShader{ nameRef.cast<std::string>().value() };

// };

// Bindings ParseBindings(luabridge::LuaRef &bindingsRef) {

// 	luabridge::LuaRef bindingsRef = bindingsRef["bindings"];
// 	std::vector<Binding> bindings;
// 	if (bindingsRef.isTable()) {
// 		for (int i = 0; i <= bindingsRef.length(); ++i) {
// 			luabridge::LuaRef bindingTable = bindingsRef[i];
// 			if (bindingTable.isTable()) {
// 				Binding binding;
// 				{
// 					binding.binding_ = bindingTable["binding"].cast<uint32_t>().value();
// 					binding.resourceType_ = magic_enum::enum_cast<ResourceType>(bindingTable["resourceType"].cast<std::string>().value()).value();
// 					binding.stage_ = magic_enum::enum_cast<Stage>(bindingTable["stage"].cast<std::string>().value()).value();
// 				}
// 				bindings.emplace_back(binding);
// 			}
// 		}
// 	}
// 	return Bindings{ std::move(bindings) };
// };

// InputAssembler ParseInputAssembler(luabridge::LuaRef &inputAssemblerRef) {

// 	const VertexType vertexType = magic_enum::enum_cast<VertexType>(inputAssemblerRef["vertexType"].cast<std::string>().value()).value();
// 	const IndexType indexType = magic_enum::enum_cast<IndexType>(inputAssemblerRef["indexType"].cast<std::string>().value()).value();
// 	const FrontFace frontFace = magic_enum::enum_cast<FrontFace>(inputAssemblerRef["frontFace"].cast<std::string>().value()).value();
// 	const CullMode cullMode = magic_enum::enum_cast<CullMode>(inputAssemblerRef["cullMode"].cast<std::string>().value()).value();
	
// 	return InputAssembler{ vertexType, indexType, frontFace, cullMode };
// };

// Multisampling ParseMultisampling(luabridge::LuaRef &multisamplingRef) {

// 	NOT_IMPLEMENTED();

// 	return Multisampling{};
// };

// DepthTest ParseDepthTest(luabridge::LuaRef &depthTestRef) {
	

// 	const bool enable = depthTestRef["enable"].cast<bool>().value();
// 	const DepthBufferCompareOperation compareOperation = magic_enum::enum_cast<DepthBufferCompareOperation>(depthTestRef["compareOperation"].cast<std::string>().value()).value();

// 	return DepthTest{
// 		depthTestRef["enable"].cast<bool>().value(),
// 		compareOperation
// 	};

// };


// }

// } // namespace Render

// } // namespace OksEngine