import processing.video.*;

PImage cam0;
PImage cam1;
PImage cam2;

void setup() {
  size(720, 1440);
  frameRate(30);

  cam0 = loadImage("dark.JPG");
  cam1 = loadImage("normal.JPG");
  cam2 = loadImage("light.JPG");

  
}

void draw() {

  
  image(cam0, 0, 0, 720, 480);
  image(cam1, 0, 480, 720, 480);
  image(cam2, 0, 960, 720, 480);

  // The following does the same, and is faster when just drawing the image
  // without any additional resizing, transformations, or tint.
  //set(0, 0, cam);
}