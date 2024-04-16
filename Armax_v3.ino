#include <SoftwareSerial.h> // include the SoftwareSerial library
#include <Adafruit_MPU6050.h> // include the Adafruit_MPU6050 library
#include <Adafruit_SSD1306.h> // include the Adafruit_SSD1306 library
#include <Adafruit_Sensor.h> // include the Adafruit_Sensor library
#define SPEAKER_PIN 9  // define the speaker pin

SoftwareSerial HC05(10, 11); // set bluetooth
Adafruit_MPU6050 gyro; // set gyro
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire); //set display

char blueVal; // used to store bluetooth value
int currentExercise = 0; // used to store the current exercise
int progressBarWidth = 0; // used to store the width of the progress bar
bool reachedLow = false; // used to store if user reached the lower test case
bool reachedHigh = false; // used to store if user reached the higher test case
bool exerciseComplete = false;  // true if the current exercise is complete

void setup() {
  Serial.begin(115200); // initialize serial communication
  HC05.begin(38400); // initialize bluetooth communication
  Serial.println("Armax is ready! You can pair it with bluetooth now."); // print message to serial monitor
  
  if (!gyro.begin()) { // initialize gyro
    Serial.println("Sensor init failed"); // print message to serial monitor if gyro failed
    while (1) // loop forever
      yield();
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // initialize display
    Serial.println(F("SSD1306 allocation failed")); // print message to serial monitor if display failed
    for (;;) // loop forever
      ;
  }

  display.display(); // display
  delay(500); // delay for 500ms
  display.setTextSize(1); // set text size to 1
  display.setTextColor(WHITE);  // set text color to white
  display.setRotation(0); // set rotation to 0

  welcomeScreen(); // call the welcome screen function
}

void bluetoothFunc() { // bluetooth function
  if (Serial.available()) { // check if serial is available
    blueVal = Serial.read();  // read the serial value and store it in val
    HC05.print(blueVal); // print the value to bluetooth
  }

  if (HC05.available()) { // check if bluetooth is available
    blueVal = HC05.read(); // read the bluetooth value and store it in val
    Serial.print(blueVal); // print the value to serial monitor
  }
}

void welcomeScreen() { // welcome screen function used to display the welcome message
  display.clearDisplay(); // clear the display
  display.setCursor(0, 0); // set the cursor to 0, 6
  display.println("Welcome to Armax!");
  display.println("Wrist up to switch exercises");
  display.println("Wrist down to comfirm");
  display.display(); // display
  chooseExercise(); // call the choose exercise function
  delay(1000);
}

void chooseExercise() { // this function is used to provide a user interface to choose the exercise
  bool exerciseChosen = false; // flag to check if exercise is chosen

  while (!exerciseChosen) {
    sensors_event_t a, g, temp; // set sensors_event_t variables a, g, temp
    gyro.getEvent(&a, &g, &temp); // get the gyro event and store it in a, g, temp

    float xValue = a.acceleration.x; // store the x value in xValue

    if (xValue >= 5) { // check if xValue is greater than or equal to 5
      currentExercise++; // increase the current exercise by 1
      if (currentExercise > 4) { // check if the current exercise is greater than 3
        currentExercise = 1; // set the current exercise to 1
      }
      // Display the current exercise
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Current Exercise: ");
      display.setTextSize(3); // set text size to 3
      display.print(currentExercise);
      display.setTextSize(1);
      display.display();
      delay(1000); // delay for 1000ms
    } else if (xValue <= -5) { // check if xValue is less than or equal to -5
      exerciseChosen = true; // set exerciseChosen to true
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Comfirmed Exercise: ");
      display.setTextSize(3); // set text size to 3
      display.print(currentExercise);
      display.setTextSize(1);
      display.display();
      return;
    }
    delay(100); // delay for 100ms to prevent rapid changes in the exercise due to sensor noise
  }
}

void checkProgressionE1(float xValue) { // this function is used to check the progression of the user in exercise 1
  if (xValue >= 7) { // check if xValue is greater than or equal to 7
    reachedHigh = true; // set reachedHigh to true
  }

  if (xValue <= 0) { // check if xValue is less than or equal to 0
    reachedLow = true; // set reachedLow to true
  }

  if ((reachedLow && xValue >= 7) || (reachedHigh && xValue <= 0)) { // check if the user reached the goal
    progressBarWidth += 8; // increase the progress bar width by 8
    playTone(392, 300);
    if (progressBarWidth > 128) { // check if the progress bar width is greater than 128
      progressBarWidth = 128; // set the progress bar width to 128
    }
    reachedLow = false;  // set reachedLow to false
    reachedHigh = false; // set reachedHigh to false
  }
}

void checkProgressionE2(float xValue) { // this function is used to check the progression of the user in exercise 2
  if (xValue <= -7) { // check if xValue is less than or equal to -7
    reachedHigh = true; // set reachedHigh to true
  }

  if (xValue >= 0) { // check if xValue is greater than or equal to 0
    reachedLow = true; // set reachedLow to true
  }

  if ((reachedLow && xValue <= -7) || (reachedHigh && xValue >= 0)) { // check if the user reached the goal
    progressBarWidth += 6.4; // increase the progress bar width by 6.4
    playTone(1319, 500);
    if (progressBarWidth > 128) { // check if the progress bar width is greater than 128
      progressBarWidth = 128; // set the progress bar width to 128
    }
    reachedLow = false;  // set reachedLow to false
    reachedHigh = false; // set reachedHigh to false
  }
}

void checkProgressionE3(float yValue) { // this function is used to check the progression of the user in exercise 3
  if (yValue >= 5) { // check if yValue is greater than or equal to 5
    reachedHigh = true; // set reachedHigh to true
  }

  if (yValue <= 0) { // check if yValue is less than or equal to 0
    reachedLow = true; // set reachedLow to true
  }

  if ((reachedLow && yValue >= 5) || (reachedHigh && yValue <= 0)) { // check if the user reached the goal
    progressBarWidth += 6.4; // increase the progress bar width by 6.4
    playTone(1319, 500);
    if (progressBarWidth > 128) { // check if the progress bar width is greater than 128
      progressBarWidth = 128; // set the progress bar width to 128
    }
    reachedLow = false;  // set reachedLow to false
    reachedHigh = false; // set reachedHigh to false
  }
}

void checkProgressionE4(float yValue) { // this function is used to check the progression of the user in exercise 4
  if (yValue <= -5) { // check if yValue is less than or equal to -5
    reachedHigh = true; // set reachedHigh to true
  }

  if (yValue >= 0) { // check if yValue is greater than or equal to 0
    reachedLow = true; // set reachedLow to true
  }

  if ((reachedLow && yValue <= -5) || (reachedHigh && yValue >= 0)) { // check if the user reached the goal
    progressBarWidth += 6.4; // increase the progress bar width by 6.4
    playTone(1319, 500);
    if (progressBarWidth > 128) { // check if the progress bar width is greater than 128
      progressBarWidth = 128; // set the progress bar width to 128
    }
    reachedLow = false;  // set reachedLow to false
    reachedHigh = false; // set reachedHigh to false
  }
}

void displayDraw() { // this function is used to display the progression and the motion data of the user
  sensors_event_t a, g, temp; // set sensors_event_t variables a(accelaration), g(gravity), temp(temperature)
  gyro.getEvent(&a, &g, &temp); // get the gyro event and store it in a(accelaration), g(gravity), temp(temperature)

  if (!exerciseComplete) { // check if the exercise is complete
    switch (currentExercise) { // check the current exercise and call the respective function
      case 1:
        checkProgressionE1(a.acceleration.x); // call the checkProgressionE1 function used for exercise 1
        break;
      case 2:
        checkProgressionE2(a.acceleration.x); // call the checkProgressionE2 function used for exercise 2
        break;
      case 3:
        checkProgressionE3(a.acceleration.y); // call the checkProgressionE3 function used for exercise 3
        break;
      case 4:
        checkProgressionE4(a.acceleration.y); // call the checkProgressionE4 function used for exercise 4
        break;
    }
  }

  display.clearDisplay();
  display.setCursor(0, 0);

  display.drawRect(0, 0, 128, 7, WHITE);
  display.println();

  if (progressBarWidth >= 128 && !exerciseComplete) { // check if the exercise is complete
    // Play a sequence of tones when the exercise is complete
    delay(500);
    display.clearDisplay();
    display.println("Congratulations!");
    display.println("Exercise Complete!");
    display.display();
    playTone(262, 500);  // C4
    playTone(294, 500);  // D4
    playTone(330, 500);  // E4
    playTone(349, 500);  // F4
    exerciseComplete = true;
    return;
  } else {
    switch (currentExercise) {
      case 1:
        display.println("Workout 1 Progression"); // print "Workout 1 Progression"
        break;
      case 2:
        display.println("Workout 2 Progression"); // print "Workout 2 Progression"
        break;
      case 3:
        display.println("Workout 3 Progression"); // print "Workout 3 Progression"
        break;
      case 4:
        display.println("Workout 4 Progression"); // print "Workout 4 Progression"
        break;
    }
    display.fillRect(1, 1, progressBarWidth, 5, WHITE);
  }

  display.println("Motion Data:");
  display.print("x:" + String(a.acceleration.x, 1) + ", ");
  display.print("y:" + String(a.acceleration.y, 1) + ", ");
  display.print("z:" + String(a.acceleration.z, 1) + ", ");
  display.println("");

  display.display();
  delay(100);
}

void playTone(int note, int duration) { // Play a note on the speaker pin for a certain duration
  tone(SPEAKER_PIN, note, duration);
  delay(duration);
  noTone(SPEAKER_PIN);
}

void loop() {
  bluetoothFunc(); // call the bluetooth function
  displayDraw(); // call the display draw function
  while (exerciseComplete) {
    // do nothing
  }
}
