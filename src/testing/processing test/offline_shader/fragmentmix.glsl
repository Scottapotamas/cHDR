
uniform sampler2D textureSampler;
uniform vec2 texOffset;
varying vec4 vertColor;
varying vec4 vertTexCoord;

uniform float time;
uniform vec2 resolution;
uniform vec2 mouse;

uniform float gamma_uniform;
uniform float fps_uniform;


uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D iChannel2;



// Layer between Processing and Shadertoy uniforms
vec3 iResolution = vec3(resolution,0.0);
float iGlobalTime = time;
vec4 iMouse = vec4(mouse,0.0,0.0); // zw would normally be the click status
vec4 fragCoord = gl_FragCoord.xyzw;
float gamma = gamma_uniform;
float fps = fps_uniform;

//-----------------------------------------------------


//#define CLIP_SHOW 


float Cubic (float value) {
  
    // Possibly slightly faster calculation
    // when compared to Sigmoid
    
    if (value < 0.5)
    {
        return value * value * value * value * value * 16.0; 
    }
    
    value -= 1.0;
    
    return value * value * value * value * value * 16.0 + 1.0;
}

float Sigmoid (float x) {

  //return 1.0 / (1.0 + (exp(-(x * 14.0 - 7.0))));
    return 1.0 / (1.0 + (exp(-(x - 0.5) * 14.0))); 
}

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

vec3 changeSaturation(vec3 color, float saturation)
{
  float luma = luminance(color);
  return mix(vec3(luma), color, saturation);
}

void main() {

  vec2 uv = fragCoord.xy / iResolution.xy * vec2(1.0,-1.0) + vec2(0.0, 1.0);

    //input textures from webcam(s)
    vec3 texLow = texture2D(iChannel0, uv).xyz;
    vec3 texMid = texture2D(iChannel1, uv).xyz;
    vec3 texHigh = texture2D(iChannel2, uv).xyz;
    
    vec4 buffer = vec4(1.0);
    vec4 buffer2 = vec4(0.0);;
    
  
    //buffer = vec4(mix(texLow, texMid, weight(luminance(texLow))), 1.0);
    //buffer = vec4(mix(texMid, texHigh, weight(luminance(texHigh))), 1.0);
    buffer = vec4(mix(texLow, texHigh, weight(luminance(texHigh))), 1.0); //flat curve

    //buffer2 = vec4(mix(buffer.xyz, texMid, weight(luma)), 1.0);
    
    vec4 C = buffer;
    //C = vec4(Cubic(C.r), Cubic(C.g),Cubic(C.b), 1.0); 
    C = vec4(Sigmoid(C.r), Sigmoid(C.g),Sigmoid(C.b), 1.0); 
        
    C = pow(C, vec4(gamma)); 

  gl_FragColor = vec4(changeSaturation(C.xyz, 0.9), 1.0);
}