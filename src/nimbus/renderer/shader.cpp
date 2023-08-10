#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/renderer/shader.hpp"

#include "platform/gl/glShader.hpp"

namespace nimbus
{

ref<Shader> Shader::s_create(const std::string& name,
                             const std::string& vertexSource,
                             const std::string& fragmentSource) noexcept
{
    return ref<GlShader>::gen(name, vertexSource, fragmentSource);
}

ref<Shader> Shader::s_create(const std::string& vertexPath,
                             const std::string& fragmentPath) noexcept
{
    return ref<GlShader>::gen(vertexPath, fragmentPath);
}

uint32_t Shader::s_getShaderType(Shader::ShaderType type) noexcept
{
    return GlShader::s_getShaderType(type);
}

}  // namespace nimbus