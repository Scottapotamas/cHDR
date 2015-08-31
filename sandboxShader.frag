#define mixsoft 0.1		
#define mixrange 0.6


#define CLIP_SHOW 
 


vec4 lumaKey(float threshH, float threshL, vec3 tex){
    
    vec4 buffer = vec4(tex, 1.0);
    
   	float f = mixsoft/2.0;
	float a = mixrange - f;
	float b = mixrange + f;
	
    float luma = (tex.x + tex.y + tex.z) / 3.0;		//construct a grayscale from channel averages

	float mask = smoothstep(a, b, luma);			//apply thresholding to luma map
    
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
    
    return buffer;	//return the texture with transparent sections for clipped data
}


void mainImage( out vec4 fragColor, in vec2 fragCoord ){
	
    vec2 uv = fragCoord.xy / iResolution.xy * vec2(1.0,-1.0) + vec2(0.0, 1.0);
    vec2 m = iMouse.xy/iResolution.xy;

    //input textures from webcam(s)
    vec3 texLow = texture2D(iChannel0, uv).xyz;
	vec3 texMid = texture2D(iChannel1, uv).xyz;
    vec3 texHigh = texture2D(iChannel2, uv).xyz;
    
	   
    //highclip, lowclip, texture   
    vec4 clipLow 	= lumaKey(0.9, 0.03, texLow);
    vec4 clipMid 	= lumaKey(0.6, 0.2, texMid);
    vec4 clipHigh 	= lumaKey(0.7, 0.5, texHigh);
   
    //use input data to merge for HDR
    
    //hdrImage = mix(texLow, texHigh, texMid);

    //now we have our HDR image data, we process for output on low dynamic range displays
    vec3 ldrOutput = vec3(0.50);

   

    
    vec4 tempout = clipMid;
 
    	
	fragColor = tempout;
}




/*
vec3 changeSaturation(vec3 color, float saturation)
{
	float luma = dot(vec3(0.213, 0.715, 0.072) * color, vec3(1.));
	return mix(vec3(luma), color, saturation);
}

vec3 rgb2hsv(vec3 rgb) {
	float Cmax = max(rgb.r, max(rgb.g, rgb.b));
	float Cmin = min(rgb.r, min(rgb.g, rgb.b));
    float delta = Cmax - Cmin;

	vec3 hsv = vec3(0., 0., Cmax);
	
	if (Cmax > Cmin) {
		hsv.y = delta / Cmax;

		if (rgb.r == Cmax)
			hsv.x = (rgb.g - rgb.b) / delta;
		else {
			if (rgb.g == Cmax)
				hsv.x = 2. + (rgb.b - rgb.r) / delta;
			else
				hsv.x = 4. + (rgb.r - rgb.g) / delta;
		}
		hsv.x = fract(hsv.x / 6.);
	}
	return hsv;
}

float chromaKey(vec3 color){
	vec3 backgroundColor = vec3(0.157, 0.576, 0.129);
	vec3 weights = vec3(4., 1., 2.);

	vec3 hsv = rgb2hsv(color);
	vec3 target = rgb2hsv(backgroundColor);
	float dist = length(weights * (target - hsv));
	return 1. - clamp(3. * dist - 1.5, 0., 1.);
}

*/

