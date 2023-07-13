
#include "renderer/shader.hpp"

#include <fstream>
#include <sstream>

namespace nimbus
{

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : m_vertexPath(vertexPath), m_fragmentPath(fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;

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
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        NM_CORE_ASSERT(0, "SHADER::FILE_NO_SUCCESFULLY_READ\n");
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

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
        NM_CORE_ASSERT(0, "SHADER::VERTEX::COMPILATION_FAILED %s\n", infoLog);
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
        NM_CORE_ASSERT(0, "SHADER::FRAGMENT::COMPILATION_FAILED %s\n", infoLog);
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
        NM_CORE_ASSERT(0, "SHADER::PROGRAM::COMPILATION_FAILED %s\n", infoLog);
    }

    // delete shaders as they're linked into our program now and no longer
    // necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader()
{
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
    glUseProgram(m_id);
}
// utility uniform functions
void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name,
                     float              x,
                     float              y,
                     float              z,
                     float              w) const
{
    glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()),
                       1,
                       GL_FALSE,
                       glm::value_ptr(mat));
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()),
                       1,
                       GL_FALSE,
                       glm::value_ptr(mat));
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(
        _getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

std::int32_t Shader::_getUniformLocation(const std::string& name) const
{
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
            NM_CORE_INFO(
                "Warning: Uniform %s not found in shader program (vertex: "
                "%s, fragment: %s\n",
                name.c_str(),
                m_vertexPath.c_str(),
                m_fragmentPath.c_str());
        }
    }

    return location;
}

}  // namespace nimbus