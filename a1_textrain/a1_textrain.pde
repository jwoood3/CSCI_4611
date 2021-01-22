/*
 * CSci-4611 Assignment #1 Text Rain
 */


import processing.video.*;

// Global variables for handling video data and the input selection screen
String[] cameras;
Capture cam;
Movie mov;
PImage inputImage;
boolean inputMethodSelected = false;
char[] randomLetterOptions = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
Letters[] letterArray = new Letters[50];
int threshold = 130;
boolean debug = false;
boolean hasntOccurred = true;
String[] wordsArray = {"RAIN", "UMBRELLA", "HAND", "PERSON", "FALLING"};
char[] selectedWord;


void setup() {
  size(1280, 720);  
  inputImage = createImage(width, height, RGB);
  CreateLetterArray();
}


void draw() {
  
  // When the program first starts, draw a menu of different options for which camera to use for input
  // The input method is selected by pressing a key 0-9 on the keyboard
  if (!inputMethodSelected) {
    cameras = Capture.list();
    int y=40;
    text("O: Offline mode, test with TextRainInput.mov movie file instead of live camera feed.", 20, y);
    y += 40; 
    for (int i = 0; i < min(9,cameras.length); i++) {
      text(i+1 + ": " + cameras[i], 20, y);
      y += 40;
    }
    return;
  }

  // This part of the draw loop gets called after the input selection screen, during normal execution of the program.
  
  // STEP 1.  Load an image, either from a movie file or from a live camera feed. Store the result in the inputImage variable
  if ((cam != null) && (cam.available())) {
    cam.read();
    inputImage.copy(cam, 0,0,cam.width,cam.height, 0,0,inputImage.width,inputImage.height);
  }
  else if ((mov != null) && (mov.available())) {
    mov.read();
    inputImage.copy(mov, 0,0,mov.width,mov.height, 0,0,inputImage.width,inputImage.height);
  }

  // This code draws the current camera/movie image to the screen
  set(0, 0, inputImage);
  mirrorImage();

  // TODO: Fill in your code to implement the rest of TextRain here..
  if (debug)
  {
    EnterDebugging(); 
  }
  PrintLetterArray();
}


void keyPressed() {
  if (!inputMethodSelected) {
    
    // If we haven't yet selected the input method, then check for 0 to 9 keypresses to select from the input menu
    if ((key >= '0') && (key <= '9')) { 
      int input = key - '0';
      if (input == 0) {
        println("Offline mode selected.");
        mov = new Movie(this, "TextRainInput.mov"); 
        //Note to grader: I had to change this to mov = new Movie(this, "/Users/jeremywood/Desktop/a1_textrain/TextRainInput.mov")
        //to get it to work on my machine, but I changed it back to what it is now to submit.
        mov.loop();
        inputMethodSelected = true;
      }
      else if ((input >= 1) && (input <= 9)) {
        println("Camera " + input + " selected.");
        
        // The camera can be initialized directly using an element from the array returned by list():
        cam = new Capture(this, cameras[input-1]);
        cam.start();
        inputMethodSelected = true;
      }
    }
    return;
  }

  // This part of the keyPressed routine gets called after the input selection screen during normal execution of the program
  // TODO: Fill in your code to handle keypresses here..
  if (key == CODED) {
    if (keyCode == UP) {
      // up arrow key pressed
      threshold+=10;
    }
    else if (keyCode == DOWN) {
      // down arrow key pressed
      threshold-=10;
    }
  }
  else if (key == ' ') {
    // spacebar pressed
    debug = !debug;
  } 
}

//Class defining each letter, contains what letter it is, its x and y position, and its velocity
class Letters
{
  public float xPos, yPos, velocity;
  public char letterChar;
  
  public Letters(char letterCharacter, float x, float y, float v)
  {
   xPos = x;
   yPos = y;
   letterChar = letterCharacter;
   velocity = v;
  }
  
  //Creates the text to be seen on screen
  public void createText()
  {
    fill(0, 102, 153);
    text(letterChar, xPos, yPos);
  }
}

//Fills LetterArray with 50 objects of type Letters
void CreateLetterArray()
{
 
 int randomWordPlacement = 0;
 boolean enabled = false;
 for (int i = 0; i < letterArray.length; i++)
 {
   //Next line and if statement below have chance to put a word on the screen
   int r2 = (int)random(62-i);
   if (r2 == 3 && i < letterArray.length - 8 && hasntOccurred) {
      selectedWord = wordsArray[(int)random(5)].toCharArray();
      randomWordPlacement = (int)random(0, 1100);
      for (int j = 0; j < selectedWord.length; j++) {
        letterArray[i] = new Letters(selectedWord[j], randomWordPlacement, 30, (float)random(2.4, 2.6));
        i++;
        randomWordPlacement+=10;
      }
      hasntOccurred = false;
      enabled = true;
   }
   int r = (int)random(26);
   int x = (int)random(1280);
   int x2 = (int)random(1280);
   //makes sure that other letters dont cover the word
   if (x <= randomWordPlacement && enabled) {
     x2 = (int)random(0, randomWordPlacement); 
   }
   else if (x > randomWordPlacement && enabled){
     x2 = (int)random(randomWordPlacement+selectedWord.length*10, 1280); 
   }
   letterArray[i] = new Letters(randomLetterOptions[r], x2, 30, (float)random(2, 3));
 }
}

//Prints the letters to the screen, and moves them based on given conditions
void PrintLetterArray()
{
  for (int i = 0; i < 50; i++)
  {
    letterArray[i].createText();
    loadPixels();
    
    //Convert the coordinate of the letter its corresponding index in the pixels[] array. Get the grayscale value of said pixel
    int pixelIndex = (int)letterArray[i].xPos + (inputImage.width * ((int)letterArray[i].yPos));
    float grayScaleValue = brightness(pixels[pixelIndex]);
    
    if (grayScaleValue >= threshold)
    {
      if (letterArray[i].yPos + letterArray[i].velocity > inputImage.height)
      {
         //Call wrap around function if increasing the y position would put the letter below the screen
         LetterWrapAround(i);
      }
      else
      {
        //Moves the letter down the screen by its velocity value
        letterArray[i].yPos += letterArray[i].velocity;
      }
    }
    else if (grayScaleValue < threshold)
    {
      //move the letter up the screen if it is on top of a dark object. If if reaches the top of the screen, don't advance past the top
      letterArray[i].yPos -= letterArray[i].yPos-7 < 1 ? -(letterArray[i].yPos + 1) : 7;
    }
  }
  updatePixels();
}

//This function is called if a letter reaches the bottom of the screen.
//A new letter is created at the same index in the LetterArray with a new character, x position, velocity, and a reset y position (top of the screen)
void LetterWrapAround(int index)
{
   letterArray[index].letterChar =  randomLetterOptions[(int)random(26)];
   letterArray[index].xPos = (int)random(1280);
   letterArray[index].yPos = 1;
   letterArray[index].velocity = (float)random(1, 2);
}

//Flips the screen and gives a grayscale
void mirrorImage()
{
 pushMatrix();
 scale(-1.0, 1.0);
 image(inputImage,-inputImage.width,0);
 filter(GRAY);
 popMatrix(); 
}

//If the space bar is pressed, this will change the screen to black and white based on the threshold
void EnterDebugging()
{
  loadPixels();
  for (int p = 0; p < pixels.length; p++)
  {
    pixels[p] = (brightness(pixels[p]) < threshold) ? color(0) : color(255);
  }
  updatePixels();
}

