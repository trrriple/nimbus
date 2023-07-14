#pragma once

#include "glm.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace nimbus
{

class Shader
{
   public:
    Shader() = default;

    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    ~Shader();

    const std::string& getVertexPath() const;

    const std::string& getFragmentPath() const;

    // user/activate the shader
    void use() const;

    std::uint32_t getId()
    {
        return m_id;
    }

    // utility uniform functions
    void setBool(const std::string& name, bool value) const;

    void setInt(const std::string& name, uint32_t value) const;

    void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;
    
    void setVec2(const std::string& name, float x, float y) const;

    void setVec3(const std::string& name, const glm::vec3& value) const;
    
    void setVec3(const std::string& name, float x, float y, float z) const;

    void setVec4(const std::string& name, const glm::vec4& value) const;

    void setVec4(const std::string& name,
                 float              x,
                 float              y,
                 float              z,
                 float              w) const;

    void setMat2(const std::string& name, const glm::mat2& mat) const;

    void setMat3(const std::string& name, const glm::mat3& mat) const;

    void setMat4(const std::string& name, const glm::mat4& mat) const;

   private:
    std::uint32_t m_id;
    std::string   m_vertexPath;
    std::string   m_fragmentPath;

    inline static std::uint32_t s_lastId = 0;

    // we use this map to keep track of where uniforms are located
    // in the future, it may make sense to make a uniform object that
    // gets added to a shader and then can be set accordingly such that it
    // already knows its location. Probably not a ton of room for optimzation
    // here anyways
    mutable std::unordered_map<std::string, std::int32_t> m_uniformLocCache;

    std::int32_t _getUniformLocation(const std::string& name) const;
};

}  // namespace nimbus