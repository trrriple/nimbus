#pragma once

#include "nimbus/renderer/shader.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

#include "glm.hpp"

namespace nimbus
{

/// This class is used for handling shaders.
class GlShader : public Shader
{
   public:
    /// Constructor for Shader.
    /// @param name Name of the shader.
    /// @param vertexSource Source code for the vertex shader.
    /// @param fragmentSource Source code for the fragment shader.
    GlShader(const std::string& name,
             const std::string& vertexSource,
             const std::string& fragmentSource) noexcept;

    /// Constructor for Shader.
    /// @param vertexPath Path to the vertex shader file.
    /// @param fragmentPath Path to the fragment shader file.
    GlShader(const std::string& vertexPath,
             const std::string& fragmentPath) noexcept;

    virtual ~GlShader() override;

    const std::string& getVertexPath() const noexcept override;

    const std::string& getFragmentPath() const noexcept override;

    void bind() const noexcept override;

    std::uint32_t getId() const noexcept override
    {
        return m_id;
    }

    const std::string& getName() const noexcept override
    {
        return m_name;
    }

    void setBool(const std::string& name, bool value) const noexcept override;

    void setInt(const std::string&          name,
                const std::vector<int32_t>& value,
                uint32_t                    count) const noexcept override;

    void setInt(const std::string& name, int32_t value) const noexcept override;

    void setFloat(const std::string&        name,
                  const std::vector<float>& value,
                  uint32_t                  count) const noexcept override;

    void setFloat(const std::string& name, float value) const noexcept override;

    void setVec2(const std::string&            name,
                 const std::vector<glm::vec2>& value,
                 uint32_t count = 1) const noexcept override;

    void setVec2(const std::string& name,
                 const glm::vec2&   value) const noexcept override;

    void setVec2(const std::string& name,
                 float              x,
                 float              y) const noexcept override;

    void setVec3(const std::string&            name,
                 const std::vector<glm::vec3>& value,
                 uint32_t count = 1) const noexcept override;

    void setVec3(const std::string& name,
                 const glm::vec3&   value) const noexcept override;

    void setVec3(const std::string& name,
                 float              x,
                 float              y,
                 float              z) const noexcept override;

    void setVec4(const std::string&            name,
                 const std::vector<glm::vec4>& value,
                 uint32_t count = 1) const noexcept override;

    void setVec4(const std::string& name,
                 const glm::vec4&   value) const noexcept override;

    void setVec4(const std::string& name,
                 float              x,
                 float              y,
                 float              z,
                 float              w) const noexcept override;

    void setMat2(const std::string&            name,
                 const std::vector<glm::mat2>& value,
                 uint32_t count = 1) const noexcept override;

    void setMat2(const std::string& name,
                 const glm::mat2&   mat) const noexcept override;

    void setMat3(const std::string&            name,
                 const std::vector<glm::mat3>& value,
                 uint32_t count = 1) const noexcept override;

    void setMat3(const std::string& name,
                 const glm::mat3&   mat) const noexcept override;

    void setMat4(const std::string&            name,
                 const std::vector<glm::mat4>& value,
                 uint32_t count = 1) const noexcept override;

    void setMat4(const std::string& name,
                 const glm::mat4&   mat) const noexcept override;

    static uint32_t s_getShaderType(ShaderType type);

   private:
    /// Compiles the shader.
    /// @param vertexPath The path to the vertex shader.
    /// @param fragmentPath The path to the fragment shader.
    void _compileShader(const std::string& vertexPath,
                        const std::string& fragmentPath) noexcept;

    /// Retrieves the location of a uniform in the shader.
    /// @param name The name of the uniform.
    /// @return The location of the uniform.
    std::int32_t _getUniformLocation(const std::string& name) const noexcept;
};

}  // namespace nimbus
