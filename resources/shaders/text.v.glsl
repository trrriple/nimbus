#version 460 core
layout(location = 0) in vec4  a_position;
layout(location = 1) in vec2  a_texCoord;
layout(location = 2) in vec4  a_fgColor;
layout(location = 3) in vec4  a_bgColor;
layout(location = 4) in vec2  a_unitRange;
layout(location = 5) in int   a_texIndex;
layout(location = 6) in uint  a_entityId;

layout(location = 0)      out vec2 v_texCoord;
layout(location = 1)      out vec4 v_fgColor;
layout(location = 2)      out vec4 v_bgColor;
layout(location = 3)      out vec2 v_unitRange;
layout(location = 4) flat out int  v_texIndex;
layout(location = 5) flat out uint v_entityId;

uniform mat4 u_viewProjection;

void main()
{
    v_texCoord  = a_texCoord;
    v_fgColor   = a_fgColor;
    v_bgColor   = a_bgColor;
    v_unitRange = a_unitRange;
    v_texIndex  = a_texIndex;
    v_entityId  = a_entityId;

    gl_Position = u_viewProjection * a_position;
}