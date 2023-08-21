#pragma once

#include "nimbus/core/common.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

#include "glm.hpp"

namespace nimbus
{

class Shader : public refCounted
{
   public:
    enum class ShaderType
    {
        INT,
        UINT,
        FLOAT,
        BOOL
    };
    /// Destructor for Shader.
    virtual ~Shader() = default;

    /// Getter for vertex shader path.
    /// @return Path to the vertex shader.
    virtual const std::string& getVertexPath() const = 0;

    /// Getter for fragment shader path.
    /// @return Path to the fragment shader.
    virtual const std::string& getFragmentPath() const = 0;

    /// Activates the shader.
    virtual bool bind() const = 0;

    /// Getter for the ID of the shader.
    /// @return The ID of the shader.
    virtual u32_t getId() const = 0;

    /// Getter for the name of the shader.
    /// @return The name of the shader.
    virtual const std::string& getName() const = 0;

    /// Sets a boolean uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    virtual void setBool(const std::string& name, bool value) const = 0;

    /// Sets an integer uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    /// @param count The number of ints in the data
    virtual void setInt(const std::string& name, const std::vector<i32_t>& value, u32_t count) const = 0;

    /// Sets an integer uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    virtual void setInt(const std::string& name, i32_t value) const = 0;

    /// Sets a f32_t uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    /// @param count The number of floats in the data
    virtual void setFloat(const std::string& name, const std::vector<f32_t>& value, u32_t count) const = 0;

    /// Sets a f32_t uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    virtual void setFloat(const std::string& name, f32_t value) const = 0;

    /// Sets a 2D vector uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of vectors in the data
    virtual void setVec2(const std::string& name, const std::vector<glm::vec2>& value, u32_t count = 1) const = 0;

    /// Sets a 2D vector uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    virtual void setVec2(const std::string& name, const glm::vec2& value) const = 0;

    /// Sets a 2D vector uniform with individual components.
    /// @param name The name of the uniform.
    /// @param x The x component.
    /// @param y The y component.
    virtual void setVec2(const std::string& name, f32_t x, f32_t y) const = 0;

    /// Sets a 3D vector uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of vectors in the data
    virtual void setVec3(const std::string& name, const std::vector<glm::vec3>& value, u32_t count = 1) const = 0;

    /// Sets a 3D vector uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    virtual void setVec3(const std::string& name, const glm::vec3& value) const = 0;

    /// Sets a 3D vector uniform with individual components.
    /// @param name The name of the uniform.
    /// @param x The x component.
    /// @param y The y component.
    /// @param z The z component.
    virtual void setVec3(const std::string& name, f32_t x, f32_t y, f32_t z) const = 0;

    /// Sets a 4D vector uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of vectors in the data
    virtual void setVec4(const std::string& name, const std::vector<glm::vec4>& value, u32_t count = 1) const = 0;

    /// Sets a 4D vector uniform.
    /// @param name The name of the uniform.
    /// @param value The value to set.
    virtual void setVec4(const std::string& name, const glm::vec4& value) const = 0;

    /// Sets a 4D vector uniform with individual components.
    /// @param name The name of the uniform.
    /// @param x The x component.
    /// @param y The y component.
    /// @param z The z component.
    /// @param w The w component.
    virtual void setVec4(const std::string& name, f32_t x, f32_t y, f32_t z, f32_t w) const = 0;

    /// Sets a 2x2 matrix uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of matrix in the data
    virtual void setMat2(const std::string& name, const std::vector<glm::mat2>& value, u32_t count = 1) const = 0;

    /// Sets a 2x2 matrix uniform.
    /// @param name The name of the uniform.
    /// @param mat The matrix to set.
    virtual void setMat2(const std::string& name, const glm::mat2& mat) const = 0;

    /// Sets a 3x3 matrix uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of matrix in the data
    virtual void setMat3(const std::string& name, const std::vector<glm::mat3>& value, u32_t count = 1) const = 0;

    /// Sets a 3x3 matrix uniform.
    /// @param name The name of the uniform.
    /// @param mat The matrix to set.
    virtual void setMat3(const std::string& name, const glm::mat3& mat) const = 0;

    /// Sets a 4x4 matrix uniform.
    /// @param name The name of the uniform.
    /// @param value Vector of values to set.
    /// @param count The number of matrix in the data
    virtual void setMat4(const std::string& name, const std::vector<glm::mat4>& value, u32_t count = 1) const = 0;

    /// Sets a 4x4 matrix uniform.
    /// @param name The name of the uniform.
    /// @param mat The matrix to set.
    virtual void setMat4(const std::string& name, const glm::mat4& mat) const = 0;

    static u32_t s_getShaderType(ShaderType type);

   protected:
    u32_t       m_id;            ///< The unique ID of the shader.
    std::string m_vertexPath;    ///< The path to the vertex shader.
    std::string m_fragmentPath;  ///< The path to the fragment shader.
    std::string m_name;          ///< The name of the shader.
    bool        m_loaded;        ///< Shader is loaded.

    /// Cache of uniform locations in the shader.
    /// Used for keeping track of where uniforms are located.
    /// In the future, it may make sense to make a uniform object that
    /// gets added to a shader and then can be set accordingly such that it
    /// already knows its location. Probably not a ton of room for optimzation
    /// here anyways.
    mutable std::unordered_map<std::string, i32_t> m_uniformLocCache;

   private:
    // ensure only Resouce manager can call this
    static ref<Shader> s_create(const std::string& name,
                                const std::string& vertexSource,
                                const std::string& fragmentSource);

    static ref<Shader> s_create(const std::string& vertexPath, const std::string& fragmentPath);

    friend class ResourceManager;
};

}  // namespace nimbus
