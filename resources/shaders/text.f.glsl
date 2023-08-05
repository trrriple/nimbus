#version 460 core

layout(location = 0)      in vec2  v_texCoord;
layout(location = 1)      in vec4  v_fgColor;
layout(location = 2)      in vec4  v_bgColor;
layout(location = 3)      in vec2  v_unitRange;
layout(location = 4) flat in int   v_texIndex;
layout(location = 5) flat in int   v_entityId;

layout(binding = 0) uniform sampler2D u_atlases[32];

layout(location = 0) out vec4 fragColor;
layout(location = 1) out int  o_entityId;

float screenPxRange()
{
    vec2 screenTexSize = vec2(1.0f) / fwidth(v_texCoord);
    return max(0.5f * dot(v_unitRange, screenTexSize), 1.0f);
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    o_entityId = v_entityId;

    vec3  msd = texture(u_atlases[v_texIndex], v_texCoord).rgb;
    float sd  = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5f);
    float opacity          = clamp(screenPxDistance + 0.5f, 0.0f, 1.0f);
    if (opacity == 0.0f)
        discard;

    fragColor = mix(v_bgColor, v_fgColor, opacity);
    if (fragColor.a == 0.0f)
        discard;
}
