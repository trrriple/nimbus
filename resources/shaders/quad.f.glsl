#version 460 core
layout(location = 0) in vec2 v_texCoord;
layout(location = 1) in vec4 v_color;
layout(location = 2) in float v_texIndex;
layout(location = 3) in float v_texTilingFactor;
layout(location = 4) flat in int v_entityId;

layout(binding = 0) uniform sampler2D u_textures[32];

layout(location = 0) out vec4 o_fragColor;
layout(location = 1) out int o_entityId;

void main()
{
    o_entityId = v_entityId;

    o_fragColor = v_color
                  * texture(u_textures[int(v_texIndex)],
                            v_texCoord * v_texTilingFactor);

    if (o_fragColor.a == 0.0f)
        discard;
}
