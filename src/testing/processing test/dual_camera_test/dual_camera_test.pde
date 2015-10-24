import processing.video.*;

Capture cam0;
Capture cam1;

void setup() {
  size(1280, 360);
  frameRate(30);

  String[] cameras = Capture.list();
  
  if (cameras.length == 0) {
    println("There are no cameras available for capture.");
    exit();
  } else {
    println("Available cameras:");
        
    for (int i = 0; i < cameras.length; i++) {
      println("Camera array index: "+i);
      println(cameras[i]);
    }
    
    // The camera can be initialized directly using an 
    // element from the array returned by list():
    cam0 = new Capture(this, cameras[4]);
    cam1 = new Capture(this, cameras[16]);

    cam0.start();
    cam1.start();
  }      
}

void draw() {
  if (cam0.available() == true) {
    cam0.read();
  }
 
  if (cam1.available() == true) {
    cam1.read();
  }
  
  image(cam0, 0, 0, 640, 360);
  image(cam1, 640, 0, 640, 360);
  // The following does the same, and is faster when just drawing the image
  // without any additional resizing, transformations, or tint.
  //set(0, 0, cam);
}