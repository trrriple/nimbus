#version 460 core

layout(location = 0) in vec2 texCoord;
layout(location = 1) in vec4 fgColor;
layout(location = 2) in vec4 bgColor;
layout(location = 3) in vec2 unitRange;
layout(location = 4) in float texIndex;

layout (binding = 0) uniform sampler2D u_atlases[32];

layout(location = 0) out vec4 fragColor;

float screenPxRange()
{
    vec2 screenTexSize = vec2(1.0f) / fwidth(texCoord);
    return max(0.5f * dot(unitRange, screenTexSize), 1.0f);
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    vec3  msd              = texture(u_atlases[int(texIndex)], texCoord).rgb;
    float sd               = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange() * (sd - 0.5f);
    float opacity          = clamp(screenPxDistance + 0.5f, 0.0f, 1.0f);
    if (opacity == 0.0f)
        discard;

    fragColor = mix(bgColor, fgColor, opacity);
    if (fragColor.a == 0.0f)
        discard;
}
