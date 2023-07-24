#include "renderer/shader.hpp"

#include <fstream>
#include <sstream>

#include "core.hpp"
#include "nmpch.hpp"

namespace nimbus
{
Shader::Shader(const std::string& name,
               const std::string& vertexSource,
               const std::string& fragmentSource)
    : m_vertexPath("none"), m_fragmentPath("none"), m_name(name)
{
    _compileShader(vertexSource, fragmentSource);
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : m_vertexPath(vertexPath),
      m_fragmentPath(fragmentPath),
      m_name(vertexPath + fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexSource;
    std::string fragmentSource;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open files
        vShaderFile.open(vertexPath);

        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream;
        std::stringstream fShaderStream;

        // read file buffer contents into the streams;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // close file handles
        vShaderFile.close();
        fShaderFile.close();

        // convert stream to string
        vertexSource   = vShaderStream.str();
        fragmentSource = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        NM_CORE_ASSERT(0, "SHADER::FILE_NO_SUCCESFULLY_READ");
    }

    _compileShader(vertexSource, fragmentSource);
}

Shader::~Shader()
{
    NM_PROFILE_DETAIL();

    glDeleteProgram(m_id);
}

const std::string& Shader::getVertexPath() const
{
    return m_vertexPath;
}

const std::string& Shader::getFragmentPath() const
{
    return m_fragmentPath;
}

// user/activate the shader
void Shader::use() const
{
    NM_PROFILE_TRACE();

    // don't rebind the same shader
    if (m_id != s_currBoundId)
    {
        glUseProgram(m_id);
        s_currBoundId = m_id;
    }
}
// utility uniform functions
void Shader::setBool(const std::string& name, bool value) const
{
    NM_PROFILE_TRACE();

    glUniform1i(_getUniformLocation(name), (GLuint)value);
}

void Shader::setInt(const std::string&          name,
                    const std::vector<int32_t>& value,
                    uint32_t                    count) const
{
    NM_PROFILE_TRACE();

    glUniform1iv(_getUniformLocation(name),
                 count,
                 reinterpret_cast<const GLint*>(value.data()));
}

void Shader::setInt(const std::string& name, int32_t value) const
{
    NM_PROFILE_TRACE();

    glUniform1i(_getUniformLocation(name), value);
}

void Shader::setFloat(const std::string&        name,
                      const std::vector<float>& value,
                      uint32_t                  count) const
{
    NM_PROFILE_TRACE();

    glUniform1fv(_getUniformLocation(name),
                 count,
                 reinterpret_cast<const GLfloat*>(value.data()));
}

void Shader::setFloat(const std::string& name, float value) const
{
    NM_PROFILE_TRACE();

    glUniform1f(_getUniformLocation(name), value);
}

void Shader::setVec2(const std::string&            name,
                     const std::vector<glm::vec2>& value,
                     uint32_t                      count) const
{
    NM_PROFILE_TRACE();

    glUniform2fv(_getUniformLocation(name),
                 count,
                 reinterpret_cast<const GLfloat*>(value.data()));
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    NM_PROFILE_TRACE();

    glUniform2fv(_getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    NM_PROFILE_TRACE();

    glUniform2f(_getUniformLocation(name), x, y);
}

void Shader::setVec3(const std::string&            name,
                     const std::vector<glm::vec3>& value,
                     uint32_t                      count) const
{
    NM_PROFILE_TRACE();

    glUniform3fv(_getUniformLocation(name),
                 count,
                 reinterpret_cast<const GLfloat*>(value.data()));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    NM_PROFILE_TRACE();

    glUniform3fv(_getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    NM_PROFILE_TRACE();

    glUniform3f(_getUniformLocation(name), x, y, z);
}

void Shader::setVec4(const std::string&            name,
                     const std::vector<glm::vec4>& value,
                     uint32_t                      count) const
{
    NM_PROFILE_TRACE();

    glUniform4fv(_getUniformLocation(name),
                 count,
                 reinterpret_cast<const GLfloat*>(value.data()));
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    NM_PROFILE_TRACE();

    glUniform4fv(_getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name,
                     float              x,
                     float              y,
                     float              z,
                     float              w) const
{
    NM_PROFILE_TRACE();

    glUniform4f(_getUniformLocation(name), x, y, z, w);
}

void Shader::setMat2(const std::string&            name,
                     const std::vector<glm::mat2>& value,
                     uint32_t                      count) const
{
    NM_PROFILE_TRACE();

    glUniformMatrix2fv(_getUniformLocation(name),
                       count,
                       GL_FALSE,
                       reinterpret_cast<const GLfloat*>(value.data()));
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    NM_PROFILE_TRACE();

    glUniformMatrix2fv(
        _getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat3(const std::string&            name,
                     const std::vector<glm::mat3>& value,
                     uint32_t                      count) const
{
    NM_PROFILE_TRACE();

    glUniformMatrix3fv(_getUniformLocation(name),
                       count,
                       GL_FALSE,
                       reinterpret_cast<const GLfloat*>(value.data()));
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    NM_PROFILE_TRACE();

    glUniformMatrix3fv(
        _getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat4(const std::string&            name,
                     const std::vector<glm::mat4>& value,
                     uint32_t                      count) const
{
    NM_PROFILE_TRACE();

    glUniformMatrix4fv(_getUniformLocation(name),
                       count,
                       GL_FALSE,
                       reinterpret_cast<const GLfloat*>(value.data()));
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    NM_PROFILE_TRACE();

    glUniformMatrix4fv(
        _getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

uint32_t Shader::s_getShaderType(Shader::ShaderType type)
{
    switch (type)
    {
        case (ShaderType::INT):
            return GL_INT;
        case (ShaderType::FLOAT):
            return GL_FLOAT;
        case (ShaderType::BOOL):
            return GL_BOOL;
        default:
            NM_CORE_ASSERT_STATIC(false, "Unknown Shader::ShaderType %i", type);
            return GL_INT;  // compiler snuffing
    }
}

////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////

void Shader::_compileShader(const std::string& vertexSource,
                            const std::string& fragmentSource)
{
    NM_PROFILE_DETAIL();

    const char* vShaderCode = vertexSource.c_str();
    const char* fShaderCode = fragmentSource.c_str();

    // 2. compile shaders
    std::uint32_t vertex;
    std::uint32_t fragment;
    std::int32_t  success;
    char          infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, sizeof(infoLog), NULL, infoLog);
        NM_CORE_ASSERT(0, "SHADER::VERTEX::COMPILATION_FAILED %s", infoLog);
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, sizeof(infoLog), NULL, infoLog);
        NM_CORE_ASSERT(0, "SHADER::FRAGMENT::COMPILATION_FAILED %s", infoLog);
    }

    // shader program
    m_id = glCreateProgram();
    glAttachShader(m_id, vertex);
    glAttachShader(m_id, fragment);
    glLinkProgram(m_id);
    // print linking errors if any
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_id, sizeof(infoLog), NULL, infoLog);
        NM_CORE_ASSERT(0, "SHADER::PROGRAM::COMPILATION_FAILED %s", infoLog);
    }

    // delete shaders as they're linked into our program now and no longer
    // necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

std::int32_t Shader::_getUniformLocation(const std::string& name) const
{
    NM_PROFILE_TRACE();

    auto p_uniformLoc = m_uniformLocCache.find(name);

    int32_t location;

    if (p_uniformLoc != m_uniformLocCache.end())
    {
        location = p_uniformLoc->second;
    }
    else
    {
        location                = glGetUniformLocation(m_id, name.c_str());
        m_uniformLocCache[name] = location;

        if (location == -1)
        {
            Log::coreWarn(
                "Uniform %s not found in shader program (vertex: "
                "%s, fragment: %s",
                name.c_str(),
                m_vertexPath.c_str(),
                m_fragmentPath.c_str());
        }
    }

    return location;
}

}  // namespace nimbus