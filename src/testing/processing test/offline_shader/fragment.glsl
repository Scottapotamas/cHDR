
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

#define mixsoft 0.01   
#define mixrange 0.9


//#define CLIP_SHOW 
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

vec4 lumaKey(float threshH, float threshL, vec3 tex){
    
  vec4 buffer = vec4(tex, 1.0);
    
  float f = mixsoft/2.0;
  float a = mixrange - f;
  float b = mixrange + f;
  
    float luma = (tex.x + tex.y + tex.z) / 3.0;   //construct a grayscale from channel averages

  float mask = smoothstep(a, b, luma);      //apply thresholding to luma map
    
    //if the abs luma value is above/below we have clipped data and should filter it out.
    if(luma > threshH) {      
        #ifdef CLIP_SHOW
          buffer = vec4(1.0,0.0,0.0, 0.0);   
        #else
          buffer = vec4(0.0);   
        #endif
    } 
    
    if(luma < threshL) {      
        #ifdef CLIP_SHOW
          buffer = vec4(0.0,0.0,1.0, 0.0);   
        #else
          buffer = vec4(0.0);   
        #endif
    }
    
    return buffer;  //return the texture with transparent sections for clipped data
}

void main() {

  vec2 uv = fragCoord.xy / iResolution.xy * vec2(1.0,-1.0) + vec2(0.0, 1.0);
  vec2 m = iMouse.xy/iResolution.xy;

  //input textures from webcam(s)
  vec3 texLow = texture2D(iChannel0, uv).xyz;
  vec3 texMid = texture2D(iChannel1, uv).xyz;
  vec3 texHigh = texture2D(iChannel2, uv).xyz;

  float threshH = 0.85;
  float threshL = 0.08;
    
  vec4 buffer = vec4(texMid, 1.0);
    
  float f = mixsoft/2.0;
  float a = mixrange - f;
  float b = mixrange + f;
  
    float luma = luminance(texMid);//(texMid.x + texMid.y + texMid.z) / 3.0;    //construct a grayscale from channel averages

  float mask = smoothstep(a, b, luma);      //apply thresholding to luma map
    
    //if the abs luma value is above/below we have clipped data and should filter it out.
    if(luma > threshH) {      
        #ifdef CLIP_SHOW
          buffer = vec4(1.0,0.0,0.0, 0.0);   
        #else
        buffer = vec4(mix(texMid, texHigh, weight(luminance(texMid))), 1.);
        #endif
    } 
    
    if(luma < threshL) {      
        #ifdef CLIP_SHOW
          buffer = vec4(0.0,0.0,1.0, 0.0);   
        #else
        buffer = vec4(mix(texLow, texMid, weight(luminance(texMid))), 1.);
      //buffer = vec4(texLow, 1.0);   
        #endif
    }
    
        
  vec4 tempout = buffer;
  
  gl_FragColor = tempout;
}