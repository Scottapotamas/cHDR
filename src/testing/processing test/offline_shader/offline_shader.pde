PImage cam0;
PImage cam1;
PImage cam2;

PShader shader;  
PImage img;
boolean enabled = true;

void setup() {
 size(720, 480, P2D);
 frameRate(300);

 cam0 = loadImage("dark.JPG");
 cam1 = loadImage("normal.JPG");
 cam2 = loadImage("light.JPG");
  
 shader = loadShader("fragmentmix.glsl");
  //shaders.set("fraction", 1.0);

 //uniforms
 shader.set("resolution", float(width), float(height));   
 shader.set("gamma_uniform", 0.35);   
 shader.set("fps", 0.0);   

 shader.set("iChannel0", cam0);
 shader.set("iChannel1", cam1);
 shader.set("iChannel2", cam2);

}

void draw() {
//  image(cam0, 0, 0, 720, 480);
//  image(cam1, 0, 480, 720, 480);
//  image(cam2, 0, 960, 720, 480);
 shader.set("time", millis() / 1000.0);  //update time sent to shader
 shader(shader);

 image(cam0, 0, 0);

 if (frameCount % 10 == 0) {  // every 10th frame
   println("frame: " + frameCount + " - fps: " + frameRate);
 }
  shader.set("fps", frameRate);   


}