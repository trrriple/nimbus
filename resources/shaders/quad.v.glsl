#version 460 core
layout(location = 0) in vec4  a_position;
layout(location = 1) in vec2  a_texCoord;
layout(location = 2) in vec4  a_color;
layout(location = 3) in int   a_texIndex;
layout(location = 4) in float a_texTilingFactor;
layout(location = 5) in int   a_entityId;

layout(location = 0)      out vec2  v_texCoord;
layout(location = 1)      out vec4  v_color;
layout(location = 2) flat out int   v_texIndex;
layout(location = 3)      out float v_texTilingFactor;
layout(location = 4) flat out int   v_entityId;

uniform mat4 u_viewProjection;

void main()
{
    v_texCoord        = a_texCoord;
    v_color           = a_color;
    v_texIndex        = a_texIndex;
    v_texTilingFactor = a_texTilingFactor;
    v_entityId        = a_entityId;

    gl_Position = u_viewProjection * a_position;
}