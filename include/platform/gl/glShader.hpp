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
    GlShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);

    /// Constructor for Shader.
    /// @param vertexPath Path to the vertex shader file.
    /// @param fragmentPath Path to the fragment shader file.
    GlShader(const std::string& vertexPath, const std::string& fragmentPath);

    virtual ~GlShader() override;

    const std::string& getVertexPath() const override;

    const std::string& getFragmentPath() const override;

    void bind() const override;

    u32_t getId() const override
    {
        return m_id;
    }

    const std::string& getName() const override
    {
        return m_name;
    }

    void setBool(const std::string& name, bool value) const override;

    void setInt(const std::string& name, const std::vector<i32_t>& value, u32_t count) const override;

    void setInt(const std::string& name, i32_t value) const override;

    void setFloat(const std::string& name, const std::vector<f32_t>& value, u32_t count) const override;

    void setFloat(const std::string& name, f32_t value) const override;

    void setVec2(const std::string& name, const std::vector<glm::vec2>& value, u32_t count = 1) const override;

    void setVec2(const std::string& name, const glm::vec2& value) const override;

    void setVec2(const std::string& name, f32_t x, f32_t y) const override;

    void setVec3(const std::string& name, const std::vector<glm::vec3>& value, u32_t count = 1) const override;

    void setVec3(const std::string& name, const glm::vec3& value) const override;

    void setVec3(const std::string& name, f32_t x, f32_t y, f32_t z) const override;

    void setVec4(const std::string& name, const std::vector<glm::vec4>& value, u32_t count = 1) const override;

    void setVec4(const std::string& name, const glm::vec4& value) const override;

    void setVec4(const std::string& name, f32_t x, f32_t y, f32_t z, f32_t w) const override;

    void setMat2(const std::string& name, const std::vector<glm::mat2>& value, u32_t count = 1) const override;

    void setMat2(const std::string& name, const glm::mat2& mat) const override;

    void setMat3(const std::string& name, const std::vector<glm::mat3>& value, u32_t count = 1) const override;

    void setMat3(const std::string& name, const glm::mat3& mat) const override;

    void setMat4(const std::string& name, const std::vector<glm::mat4>& value, u32_t count = 1) const override;

    void setMat4(const std::string& name, const glm::mat4& mat) const override;

    static u32_t s_getShaderType(ShaderType type);

   private:
    /// Compiles the shader.
    /// @param vertexPath The path to the vertex shader.
    /// @param fragmentPath The path to the fragment shader.
    void _compileShader(const std::string& vertexPath, const std::string& fragmentPath);

    /// Retrieves the location of a uniform in the shader.
    /// @param name The name of the uniform.
    /// @return The location of the uniform.
    i32_t _getUniformLocation(const std::string& name) const;
};

}  // namespace nimbus
