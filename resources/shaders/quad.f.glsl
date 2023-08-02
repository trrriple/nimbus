#version 460 core
layout(location = 0) in vec2 texCoord;
layout(location = 1) in vec4 color;
layout(location = 2) in float texIndex;
layout(location = 3) in float texTilingFactor;

layout (binding = 0) uniform sampler2D u_textures[32];

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor
        = color
          * texture(u_textures[int(texIndex)], texCoord * texTilingFactor);

    // if (fragColor.a == 0.0f)
    //     discard;
}
