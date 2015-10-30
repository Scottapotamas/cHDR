
uniform sampler2D textureSampler;

// The inverse of the texture dimensions along X and Y
uniform vec2 texOffset;
varying vec4 vertColor;
varying vec4 vertTexCoord;

uniform float time;
uniform vec2 resolution;
uniform vec2 mouse;

// Layer between Processing and Shadertoy uniforms
vec3 iResolution = vec3(resolution,0.0);
float iGlobalTime = time;
vec4 iMouse = vec4(mouse,0.0,0.0); // zw would normally be the click status
vec4 fragCoord = gl_FragCoord.xyzw;

#define halfPhase 3.5
#define speed_modifier 1.5

void main() {

  float activeTime = iGlobalTime * speed_modifier;
  vec3 col; 
  float timeMorph = 0.0;
  
  #ifdef IS_IOS 
    vec2 p = -1.0 + 2.0 * fragCoord.xy; //iOS Dynamic AR App support
  #else
    vec2 p = -1.0 + 2.0 * fragCoord.xy / iResolution.xy;
  #endif
  
  p *= 7.0;
  
  float a = atan(p.y,p.x);
  float r = sqrt(dot(p,p));
  
  if(mod(activeTime, 2.0 * halfPhase) < halfPhase)
    timeMorph = mod(activeTime, halfPhase);
  else
    timeMorph = (halfPhase - mod(activeTime, halfPhase)); 
    
  timeMorph = 2.0*timeMorph + 1.0;
  
  float w = 0.25 + 3.0*(sin(activeTime + 1.0*r)+ 3.0*cos(activeTime + 5.0*a)/timeMorph);
  float x = 0.8 + 3.0*(sin(activeTime + 1.0*r)+ 3.0*cos(activeTime + 5.0*a)/timeMorph);
  
  col = vec3(0.1,0.2,0.82)*1.1;

  gl_FragColor = vec4(col*w*x,1.0);


  //vec2 uv = gl_FragCoord.xy / resolution.xy;
  //gl_FragColor = vec4(uv,0.5+0.5*sin(iGlobalTime),1.0);
}