#include "hepch.h"
#include <iostream>
#include <assert.h>
#include "Core/Application.h"
#include "ShaderLibrary.h"

namespace House {
	ShaderLibrary::ShaderLibrary() { }
	ShaderLibrary::~ShaderLibrary() { }

	MEM::Ref<Shader>& ShaderLibrary::Load(const std::string& name, const std::string& vertexPath, const std::string& fragPath)
	{
		_Shaders[name] = Shader::Create(vertexPath, fragPath);
		return _Shaders[name];
	}

	const MEM::Ref<Shader>& ShaderLibrary::GetShader(const std::string& name) const
	{
		return _Shaders.at(name);
	}
}