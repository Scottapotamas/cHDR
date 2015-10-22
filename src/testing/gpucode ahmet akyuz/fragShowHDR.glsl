#version 420

in vec2 texCoord;                      ///< Texture coordinate from the vertex shader

uniform sampler2D hdrSampler;           ///< HDR image sampler
uniform float factor;                   ///< Burn-out threshold

layout(location=0) out vec4 fragColor; ///< Destination color

void main()
{
    vec3 hdr = texture(hdrSampler, texCoord).rgb;

    fragColor.rgb = hdr * factor;
}
