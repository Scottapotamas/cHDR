#version 330

in vec2 texCoord;                      ///< Texture coordinate from the vertex shader

uniform sampler2D MySampler[10];       ///< LDR image samplers
uniform int numImages;                 ///< Actual number of LDR images

layout(location=0) out vec4 fragColor; ///< Destination color

float luminance(vec3 color)
{
    // Assuming that input color is in linear sRGB color space.

    return (color.r * 0.2126) + 
           (color.g * 0.7152) + 
           (color.b * 0.0722);
}

float weight(float val)
{
    // Uses a tent function.

    float w;

    if (val <= 0.5)
    {
        w = val * 2.0;
    }
    else
    {
        w = (1.0 - val) * 2.0;
    }

    return w;
}

void main()
{
    const int refId = 8;                        ///< LDR reference image id (image with exposure = 1)
    float weightSum = 0.0;                      ///< Sum of all weights
    vec4 hdr        = vec4(0.0, 0.0, 0.0, 0.0); ///< Destination HDR image (last channel stores log luminance)

    for (int i = 0; i < 10; i++)
    {
        if (i < numImages)
        {
            vec3 ldr       = texture(MySampler[i], texCoord).rgb;
            //vec3 ldr       = vec3(0.5, 0.5, 0.5);
            float lum      = luminance(ldr);
            //float lum      = 0.5;
            float w        = weight(lum);
            //float w        = 0.5;
            float exposure = pow(2.0, float(i - refId));

            hdr.rgb       += (ldr / exposure) * w;
            weightSum     += w;
        }
    }

    hdr.rgb /= weightSum + 1e-6;    
    hdr.a = log(luminance(hdr.rgb) + 1e-6);

    fragColor = hdr;
}
