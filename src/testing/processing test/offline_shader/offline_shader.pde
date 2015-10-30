PImage cam0;
//PImage cam1;
//PImage cam2;

PShader shader;  
PImage img;
boolean enabled = true;

void setup() {
 size(720, 480, P2D);

 cam0 = loadImage("dark.JPG");
 //cam1 = loadImage("normal.JPG");
 //cam2 = loadImage("light.JPG");
  
 shader = loadShader("fragment.glsl");
  
 shader.set("resolution", float(width), float(height));   
 //shaders.set("fraction", 1.0);
 //shaders.set("texture", cam0);
 //frameRate(30);

}

void draw() {
  
//  image(cam0, 0, 0, 720, 480);
//  image(cam1, 0, 480, 720, 480);
//  image(cam2, 0, 960, 720, 480);
 //shaders.set("time", millis() / 1000.0);

 if (enabled == true) {
   shader(edges);
 }
 image(cam0, 0, 0);
  
 if (frameCount % 10 == 0) {  // every 10th frame
   println("frame: " + frameCount + " - fps: " + frameRate);
 }

}