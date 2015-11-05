import processing.video.*;

PImage cam0;
PImage cam1;
PImage cam2;

PShader shader;  
PImage img;
boolean enabled = true;

void setup() {
 size(720, 480, P2D);
 frameRate(256);

//String[] cameras = Capture.list();
  
//  if (cameras.length == 0) {
//    println("There are no cameras available for capture.");
//    exit();
//  } else {
//    println("Available cameras:");
        
//    for (int i = 0; i < cameras.length; i++) {
//      println("Camera array index: "+i);
//      println(cameras[i]);
//    }
//  }
    // The camera can be initialized directly using an 
    // element from the array returned by list():
    //cam0 = new Capture(this, cameras[4]);
    //cam1 = new Capture(this, cameras[16]);
    
    //cam0.start();
    //cam1.start();

 cam0 = loadImage("dark.JPG");
 cam1 = loadImage("normal.JPG");
 cam2 = loadImage("light.JPG");
  
 shader = loadShader("fragment.glsl");
  //shader.set("fraction", 1.0);

 //uniforms
 shader.set("resolution", float(width), float(height));   
 shader.set("time", millis() / 1000.0);  //time in seconds since start
 shader.set("iChannel0", cam2);
 shader.set("iChannel1", cam1);
 shader.set("iChannel2", cam0);

}

void draw() {
//  image(cam0, 0, 0, 720, 480);
//  image(cam1, 0, 480, 720, 480);
//  image(cam2, 0, 960, 720, 480);
 shader.set("time", millis() / 1000.0);  //update time sent to shader
 shader(shader);
 
 image(cam0, 0, 0);
 //image(cam1, 0, 0);
 //image(cam2, 0, 0);

 if (frameCount % 10 == 0) {  // every 10th frame
   println("frame: " + frameCount + " - fps: " + frameRate);
 }

}