#version 460 core
layout (location = 0) in vec4 a_position; 
layout (location = 1) in vec2 a_texCoord;
layout (location = 2) in vec4 a_color;
layout (location = 3) in float a_texIndex;
layout (location = 4) in float a_texTilingFactor;



layout(location = 0) out vec2 texCoord;
layout(location = 1) out vec4 color;
layout(location = 2) out float texIndex;
layout(location = 3) out float texTilingFactor;


uniform mat4 u_viewProjection;

void main()
{
    texCoord        = a_texCoord;
    color           = a_color;
    texIndex        = a_texIndex;
    texTilingFactor = a_texTilingFactor;

    gl_Position = u_viewProjection * a_position;
}