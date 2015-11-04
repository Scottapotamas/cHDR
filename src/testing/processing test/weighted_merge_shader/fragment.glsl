
uniform sampler2D textureSampler;
uniform vec2 texOffset;
varying vec4 vertColor;
varying vec4 vertTexCoord;

uniform float time;
uniform vec2 resolution;
uniform vec2 mouse;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;



// Layer between Processing and Shadertoy uniforms
vec3 iResolution = vec3(resolution,0.0);
float iGlobalTime = time;
vec4 iMouse = vec4(mouse,0.0,0.0); // zw would normally be the click status
vec4 fragCoord = gl_FragCoord.xyzw;

//-----------------------------------------------------

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

void main() {


  vec2 uv = fragCoord.xy / iResolution.xy * vec2(1.0,-1.0) + vec2(0.0, 1.0);

  const int refId = 2;                        ///< LDR reference image id (image with exposure = 1)
  float weightSum = 0.0;                      ///< Sum of all weights
  vec4 hdr        = vec4(0.0, 0.0, 0.0, 0.0); ///< Destination HDR image (last channel stores log luminance)

  float lum;
  float w;
  float exposure;
  vec3 ldr;
  
  
  ldr = texture2D(iChannel0, uv).xyz;
  lum      = luminance(ldr);
  w        = weight(lum);
  exposure = pow(2.0, float(0 - refId));
  hdr.rgb       += (ldr / exposure) * w;
  weightSum     += w;

  
  ldr = texture2D(iChannel1, uv).xyz;
  lum      = luminance(ldr);
  w        = weight(lum);
  exposure = pow(2.0, float(1 - refId));
  hdr.rgb       += (ldr / exposure) * w;
  weightSum     += w;
  
  
  ldr = texture2D(iChannel2, uv).xyz;

  lum      = luminance(ldr);
  w        = weight(lum);
  exposure = pow(2.0, float(2 - refId));
  hdr.rgb       += (ldr / exposure) * w;
  weightSum     += w;
  
  
  hdr.rgb /= weightSum + 1e-6;    
  hdr.a = log(luminance(hdr.rgb) + 1e-6);

  gl_FragColor = hdr;
}