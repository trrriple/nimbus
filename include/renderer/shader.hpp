#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "glm.hpp"

namespace nimbus
{

/// This class is used for handling shaders.
class Shader
{
   public:
    enum class ShaderType
    {
        INT,
        FLOAT,
        BOOL
    };

    /// Default constructor for Shader.
    Shader() = default;

    /// Constructor for Shader.
    /// @param name Name of the shader.
    /// @param vertexSource Source code for the vertex shader.
    /// @param fragmentSource Source code for the fragment shader.
    Shader(const std::string& name,
           const std::string& vertexSource,
           const std::string& fragmentSource);

    /// Constructor for Shader.
    /// @param vertexPath Path to the vertex shader file.
    /// @param fragmentPath Path to the fragment shader file.
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    /// Destructor for Shader.
    ~Shader();

    /// Getter for vertex shader path.
    /// @return Path to the vertex shader.
    const std::string& getVertexPath() const;

    /// Getter for fragment shader path.
    /// @return Path to the fragment shader.
    const std::string& getFragmentPath() const;

    /// Activates the shader.
    void use() const;

    /// Getter for the ID of the shader.
    /// @return The ID of the shader.
    std::uint32_t getId() const
    {
        return m_id;
    }

    /// Getter for the name of the shader.
    /// @return The name of the shader.
    const std::string& getName() const
    {
        return m_name;
    }

    /// Sets a boolean uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    void setBool(const std::string& name, bool value) const;

    /// Sets an integer uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    /// @param count The number of ints in the data
    void setInt(const std::string&          name,
                const std::vector<int32_t>& value,
                uint32_t                    count) const;

    /// Sets an integer uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    void setInt(const std::string& name, int32_t value) const;

    /// Sets a float uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    /// @param count The number of floats in the data
    void setFloat(const std::string&        name,
                  const std::vector<float>& value,
                  uint32_t                  count) const;

    /// Sets a float uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    void setFloat(const std::string& name, float value) const;

    /// Sets a 2D vector uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of vectors in the data
    void setVec2(const std::string&            name,
                 const std::vector<glm::vec2>& value,
                 uint32_t                      count = 1) const;

    /// Sets a 2D vector uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    void setVec2(const std::string& name, const glm::vec2& value) const;

    /// Sets a 2D vector uniform with individual components.
    /// @param name The name of the uniform.
    /// @param x The x component.
    /// @param y The y component.
    void setVec2(const std::string& name, float x, float y) const;

    /// Sets a 3D vector uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of vectors in the data
    void setVec3(const std::string&            name,
                 const std::vector<glm::vec3>& value,
                 uint32_t                      count = 1) const;

    /// Sets a 3D vector uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    void setVec3(const std::string& name, const glm::vec3& value) const;

    /// Sets a 3D vector uniform with individual components.
    /// @param name The name of the uniform.
    /// @param x The x component.
    /// @param y The y component.
    /// @param z The z component.
    void setVec3(const std::string& name, float x, float y, float z) const;

    /// Sets a 4D vector uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of vectors in the data
    void setVec4(const std::string&            name,
                 const std::vector<glm::vec4>& value,
                 uint32_t                      count = 1) const;

    /// Sets a 4D vector uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    void setVec4(const std::string& name, const glm::vec4& value) const;

    /// Sets a 4D vector uniform with individual components.
    /// @param name The name of the uniform.
    /// @param x The x component.
    /// @param y The y component.
    /// @param z The z component.
    /// @param w The w component.
    void setVec4(const std::string& name,
                 float              x,
                 float              y,
                 float              z,
                 float              w) const;

    /// Sets a 2x2 matrix uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of matrix in the data
    void setMat2(const std::string&            name,
                 const std::vector<glm::mat2>& value,
                 uint32_t                      count = 1) const;

    /// Sets a 2x2 matrix uniform.
    /// @param name The name of the uniform.
    /// @param mat The matrix to set.
    void setMat2(const std::string& name, const glm::mat2& mat) const;

    /// Sets a 3x3 matrix uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of matrix in the data
    void setMat3(const std::string&            name,
                 const std::vector<glm::mat3>& value,
                 uint32_t                      count = 1) const;

    /// Sets a 3x3 matrix uniform.
    /// @param name The name of the uniform.
    /// @param mat The matrix to set.
    void setMat3(const std::string& name, const glm::mat3& mat) const;

    /// Sets a 4x4 matrix uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of matrix in the data
    void setMat4(const std::string&            name,
                 const std::vector<glm::mat4>& value,
                 uint32_t                      count = 1) const;

    /// Sets a 4x4 matrix uniform.
    /// @param name The name of the uniform.
    /// @param mat The matrix to set.
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    static uint32_t s_getShaderType(ShaderType type);

   private:
    std::uint32_t m_id;            ///< The unique ID of the shader.
    std::string   m_vertexPath;    ///< The path to the vertex shader.
    std::string   m_fragmentPath;  ///< The path to the fragment shader.
    std::string   m_name;          ///< The name of the shader.

    inline static std::uint32_t s_currBoundId = 0;  ///< Static counter for IDs.

    /// Cache of uniform locations in the shader.
    /// Used for keeping track of where uniforms are located.
    /// In the future, it may make sense to make a uniform object that
    /// gets added to a shader and then can be set accordingly such that it
    /// already knows its location. Probably not a ton of room for optimzation
    /// here anyways.
    mutable std::unordered_map<std::string, std::int32_t> m_uniformLocCache;

    /// Compiles the shader.
    /// @param vertexPath The path to the vertex shader.
    /// @param fragmentPath The path to the fragment shader.
    void _compileShader(const std::string& vertexPath,
                        const std::string& fragmentPath);

    /// Retrieves the location of a uniform in the shader.
    /// @param name The name of the uniform.
    /// @return The location of the uniform.
    std::int32_t _getUniformLocation(const std::string& name) const;
};

}  // namespace nimbus
