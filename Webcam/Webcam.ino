#include <Bridge.h>
#include <Process.h>
#include <FileIO.h>

const String sdPath = "/mnt/sda1/pictures/";
const String picturesPath = "/www/slideshow/pictures/";
const String picturesListFile = "/www/slideshow/picturesList.txt";
const int potPin1 = A1;    // Analog ping for Potentiometer
const int potPin2 = A2;
const int sensor1Pin = A0;  // Analog pin for sensor 1
const int sensor2Pin = A3;
const int ledPin= 13;
const int acPin = 3;  // Digital pin for AC control
int sensor1Value = 0;
int sensor2Value = 0;
int pot1Value = 0;  // variable to store the value coming from the potentiometer
int pot2Value = 0;
const bool debugEnabled = false;  // enable debug logging to file
const String debugLogFile = "/www/slideshow/debuglog.txt";
const bool picturesEnabled = true; // enable or disable picture taking

Process process;
String filename;

void setup()
{
  Bridge.begin();
  FileSystem.begin();
  pinMode(ledPin, OUTPUT);
  pinMode(acPin, OUTPUT);
    
}

void loop() 
{
  pot1Value = analogRead(potPin1);  // get current value from pot
  DebugLog("Pot1Value: " + String(pot1Value));
  pot2Value = analogRead(potPin2);  // get current value from pot
  DebugLog("Pot2Value: " + String(pot2Value));
  
  sensor1Value = analogRead(sensor1Pin);  // get current value from sensor
  DebugLog("Sensor1Value: " + String(sensor1Value));
  
  sensor2Value= analogRead(sensor2Pin);  // get current value from sensor
  DebugLog("Sensor2Value: " + String(sensor2Value));
 
  if (sensor1Value >= pot1Value || sensor2Value >= pot2Value)  // trigger action if sensor >= pot
  {
    digitalWrite(ledPin, HIGH);
    digitalWrite(acPin, HIGH);
        
    if (picturesEnabled) {
      DebugLog("Taking Picture");
      generateTimestampFilename();
   
      // Take picture
      processShellCommand("fswebcam " + sdPath + filename + " -r 1280x720");
      
      // Copy picture to /www/pictures/
      processShellCommand("cp " + sdPath + filename + " " + picturesPath);
      
      appendToFile(filename, picturesListFile);
        
    }
    sensor1Value = 0;
    sensor2Value = 0;
  } 
  else 
  digitalWrite(ledPin, LOW);
  digitalWrite(acPin, LOW);
   
}

void processShellCommand(String command)
{
  process.runShellCommand(command);
  while(process.running());
}

void appendToFile(String text, String filename)
{
  File file = FileSystem.open(filename.c_str(), FILE_APPEND);
  if (file)
  {
    file.println(text);
    file.close();
  }
}

void DebugLog(String debugtext)
{
  if(debugEnabled) {
    File Debugfile = FileSystem.open(debugLogFile.c_str(), FILE_APPEND);
    if(Debugfile)
    {
      Debugfile.println(debugtext);
      Debugfile.close();
    }
  }
}

void generateTimestampFilename()
{
  // Generate filename with timestamp
  filename = "";
  processShellCommand("date +%s");
  while (process.available() > 0)
  {
    char c = process.read();
    filename += c;
  }
  filename.trim();
  filename += ".png";
}
