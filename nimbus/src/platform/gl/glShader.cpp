#include "nimbus/core/nmpch.hpp"
#include "nimbus/core/core.hpp"

#include "nimbus/platform/gl/glShader.hpp"
#include "nimbus/renderer/renderer.hpp"

#include <fstream>
#include <sstream>

#include "glad.h"

namespace nimbus
{
GlShader::GlShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
{
    m_loaded       = false;
    m_vertexPath   = "none";
    m_fragmentPath = "none";
    m_name         = name;
    _compileShader(vertexSource, fragmentSource);
}

GlShader::GlShader(const std::string& vertexPath, const std::string& fragmentPath)

{
    m_loaded       = false;
    m_vertexPath   = vertexPath;
    m_fragmentPath = fragmentPath;
    m_name         = vertexPath + fragmentPath;

    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexSource;
    std::string fragmentSource;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    NB_CORE_ASSERT(std::filesystem::exists(vertexPath), "Vertex Shader file %s doesn't exist!", vertexPath.c_str());
    NB_CORE_ASSERT(std::filesystem::exists(fragmentPath),
                   "Fragment Shader file %s doesn't exist!",
                   fragmentPath.c_str());

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

    _compileShader(vertexSource, fragmentSource);
}

GlShader::~GlShader()
{
    NB_PROFILE_DETAIL();

    glDeleteProgram(m_id);
}

const std::string& GlShader::getVertexPath() const
{
    return m_vertexPath;
}

const std::string& GlShader::getFragmentPath() const
{
    return m_fragmentPath;
}

// user/activate the shader
void GlShader::bind() const
{
    NB_PROFILE_TRACE();

    ref<GlShader> p_this = const_cast<GlShader*>(this);

    Renderer::s_submit([p_this]() { glUseProgram(p_this->m_id); });
}

// utility uniform functions
void GlShader::setBool(const std::string& name, bool value) const
{
    u32_t id = _getUniformLocation(name);

    Renderer::s_submit([id, value]() { glUniform1i(id, (GLuint)value); });
}

void GlShader::setInt(const std::string& name, const std::vector<i32_t>& value, u32_t count) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value, count]() { glUniform1iv(id, count, reinterpret_cast<const GLint*>(value.data())); });
}

void GlShader::setInt(const std::string& name, i32_t value) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value]() { glUniform1i(id, value); });
}

void GlShader::setFloat(const std::string& name, const std::vector<f32_t>& value, u32_t count) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value, count]()
                       { glUniform1fv(id, count, reinterpret_cast<const GLfloat*>(value.data())); });
}

void GlShader::setFloat(const std::string& name, f32_t value) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value]() { glUniform1f(id, value); });
}

void GlShader::setVec2(const std::string& name, const std::vector<glm::vec2>& value, u32_t count) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value, count]()
                       { glUniform2fv(id, count, reinterpret_cast<const GLfloat*>(value.data())); });
}

void GlShader::setVec2(const std::string& name, const glm::vec2& value) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value]() { glUniform2fv(id, 1, glm::value_ptr(value)); });
}

void GlShader::setVec2(const std::string& name, f32_t x, f32_t y) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, x, y]() { glUniform2f(id, x, y); });
}

void GlShader::setVec3(const std::string& name, const std::vector<glm::vec3>& value, u32_t count) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value, count]()
                       { glUniform3fv(id, count, reinterpret_cast<const GLfloat*>(value.data())); });
}

void GlShader::setVec3(const std::string& name, const glm::vec3& value) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value]() { glUniform3fv(id, 1, glm::value_ptr(value)); });
}

void GlShader::setVec3(const std::string& name, f32_t x, f32_t y, f32_t z) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, x, y, z]() { glUniform3f(id, x, y, z); });
}

void GlShader::setVec4(const std::string& name, const std::vector<glm::vec4>& value, u32_t count) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value, count]()
                       { glUniform4fv(id, count, reinterpret_cast<const GLfloat*>(value.data())); });
}

void GlShader::setVec4(const std::string& name, const glm::vec4& value) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value]() { glUniform4fv(id, 1, glm::value_ptr(value)); });
}

void GlShader::setVec4(const std::string& name, f32_t x, f32_t y, f32_t z, f32_t w) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, x, y, z, w]() { glUniform4f(id, x, y, z, w); });
}

void GlShader::setMat2(const std::string& name, const std::vector<glm::mat2>& value, u32_t count) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value, count]()
                       { glUniformMatrix2fv(id, count, GL_FALSE, reinterpret_cast<const GLfloat*>(value.data())); });
}

void GlShader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, mat]() { glUniformMatrix2fv(id, 1, GL_FALSE, glm::value_ptr(mat)); });
}

void GlShader::setMat3(const std::string& name, const std::vector<glm::mat3>& value, u32_t count) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value, count]()
                       { glUniformMatrix3fv(id, count, GL_FALSE, reinterpret_cast<const GLfloat*>(value.data())); });
}

void GlShader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, mat]() { glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(mat)); });
}

void GlShader::setMat4(const std::string& name, const std::vector<glm::mat4>& value, u32_t count) const
{
    u32_t id = _getUniformLocation(name);
    Renderer::s_submit([id, value, count]()
                       { glUniformMatrix4fv(id, count, GL_FALSE, reinterpret_cast<const GLfloat*>(value.data())); });
}

void GlShader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    u32_t id = _getUniformLocation(name);

    Renderer::s_submit([id, mat]() { glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(mat)); });
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
u32_t GlShader::s_getShaderType(ShaderType type)
{
    switch (type)
    {
        case (ShaderType::INT):
            return GL_INT;
        case (ShaderType::UINT):
            return GL_UNSIGNED_INT;
        case (ShaderType::FLOAT):
            return GL_FLOAT;
        case (ShaderType::BOOL):
            return GL_BOOL;
        default:
            NB_CORE_ASSERT_STATIC(false, "Unknown Shader::ShaderType %i", type);
            return GL_INT;  // compiler snuffing
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GlShader::_compileShader(const std::string& vertexSource, const std::string& fragmentSource)
{
    NB_PROFILE_DETAIL();

    Renderer::s_submitObject(
        [=]()
        {
            const char* vShaderCode = vertexSource.c_str();
            const char* fShaderCode = fragmentSource.c_str();

            // 2. compile shaders
            u32_t vertex;
            u32_t fragment;
            i32_t success;
            char  infoLog[512];

            // vertex Shader
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);

            // print compile errors if any
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(vertex, sizeof(infoLog), NULL, infoLog);
                NB_CORE_ASSERT(0, "SHADER::VERTEX::COMPILATION_FAILED %s", infoLog);
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
                NB_CORE_ASSERT(0, "SHADER::FRAGMENT::COMPILATION_FAILED %s", infoLog);
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
                NB_CORE_ASSERT(0, "SHADER::PROGRAM::COMPILATION_FAILED %s", infoLog);
            }

            // delete shaders as they're linked into our program now and no
            // longer necessary
            glDeleteShader(vertex);
            glDeleteShader(fragment);

            ///////////////////////////
            // Get Uniform Locations
            ///////////////////////////
            // Get the number of active uniforms
            GLint numUniforms = 0;
            glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &numUniforms);

            // Get the maximum name length of any uniform
            GLint maxNameLength = 0;
            glGetProgramiv(m_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

            // Allocate memory to hold the uniform name
            char* uniformName = new char[maxNameLength];

            for (GLint i = 0; i < numUniforms; ++i)
            {
                GLsizei actualLength = 0;
                GLint   size         = 0;
                GLenum  type         = 0;

                // Retrieve the uniform's name, size, and type
                glGetActiveUniform(m_id, i, maxNameLength, &actualLength, &size, &type, uniformName);

                u32_t loc                      = glGetUniformLocation(m_id, uniformName);
                m_uniformLocCache[uniformName] = loc;

                Log::coreInfo("Uniform %s at %i in %s", uniformName, loc, m_name.c_str());
            }

            delete[] uniformName;
        });
}

i32_t GlShader::_getUniformLocation(const std::string& name) const
{
    NB_PROFILE_TRACE();

    auto p_uniformLoc = m_uniformLocCache.find(name);

    i32_t location;

    if (p_uniformLoc != m_uniformLocCache.end())
    {
        location = p_uniformLoc->second;
    }
    else
    {
        Log::coreError("Uniform %s doesn't exist in shader %s", name.c_str(), m_name.c_str());
        location = -1;
    }

    return location;
}

}  // namespace nimbus