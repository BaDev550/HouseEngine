#pragma once

#include <unordered_map>
#include "Shader.h"
#include "Utilities/Memory.h"

namespace House {
	class ShaderLibrary : public MEM::RefCounted
	{
	public:
		ShaderLibrary();
		~ShaderLibrary();

		MEM::Ref<Shader>& Load(const std::string& name, const std::string& vertexPath, const std::string& fragPath);
		const MEM::Ref<Shader>& GetShader(const std::string& name) const;
		const std::unordered_map<std::string, MEM::Ref<Shader>>& GetShaders() const { return _Shaders; }
		std::unordered_map<std::string, MEM::Ref<Shader>>& GetShaders() { return _Shaders; }
	private:
		std::unordered_map<std::string, MEM::Ref<Shader>> _Shaders;
	};
}