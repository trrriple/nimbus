#version 460 core
layout (location = 0) in vec4 a_position; 
layout (location = 1) in vec2 a_texCoord;
layout (location = 2) in vec4 a_fgColor;
layout (location = 3) in vec4 a_bgColor;
layout (location = 4) in vec2 a_unitRange;
layout (location = 5) in float a_texIndex;

layout(location = 0) out vec2 texCoord;
layout(location = 1) out vec4 fgColor;
layout(location = 2) out vec4 bgColor;
layout(location = 3) out vec2 unitRange;
layout(location = 4) out float texIndex;

uniform mat4 u_viewProjection;

void main()
{
    texCoord  = a_texCoord;
    fgColor   = a_fgColor;
    bgColor   = a_bgColor;
    unitRange = a_unitRange;
    texIndex  = a_texIndex;

    gl_Position = u_viewProjection * a_position;
}