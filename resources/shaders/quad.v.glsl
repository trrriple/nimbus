#version 460 core
layout(location = 0) in vec4  a_position;
layout(location = 1) in vec2  a_texCoord;
layout(location = 2) in vec4  a_transform0;
layout(location = 3) in vec4  a_transform1;
layout(location = 4) in vec4  a_transform2;
layout(location = 5) in vec4  a_transform3;
layout(location = 6) in vec4  a_color;
layout(location = 7) in int   a_texIndex;
layout(location = 8) in float a_texTilingFactor;
layout(location = 9) in int   a_entityId;

layout(location = 0)      out vec2  v_texCoord;
layout(location = 1)      out vec4  v_color;
layout(location = 2) flat out int   v_texIndex;
layout(location = 3)      out float v_texTilingFactor;
layout(location = 4) flat out int   v_entityId;

uniform mat4 u_viewProjection;

void main()
{
    mat4 transform
        = mat4(a_transform0, a_transform1, a_transform2, a_transform3);

    v_texCoord        = a_texCoord;
    v_color           = a_color;
    v_texIndex        = a_texIndex;
    v_texTilingFactor = a_texTilingFactor;
    v_entityId        = a_entityId;

    gl_Position = u_viewProjection * transform * a_position;
}