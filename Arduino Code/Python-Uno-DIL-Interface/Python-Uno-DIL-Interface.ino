// const int APPstepPin = 2;  // Step pin connected to digital pin 2
// const int APPdirPin = 3;   // Direction pin connected to digital pin 3
// const int BPPstepPin = 4;  // Step pin connected to digital pin 4
// const int BPPdirPin = 5;   // Direction pin connected to digital pin 5
// const int SWstepPin = 6;   // Step pin connected to digital pin 6
// const int SWdirPin = 7;    // Direction pin connected to digital pin 7

const int delayMicros = 425; // 5000 microseconds = 5 milliseconds
const int APPMaxDisplacement = 690; 
const int BPPMaxDisplacement = 660;
const int SWMaxDisplacement = 400;

int APPPos = 0;
int BPPPos = 0;
int SWPos = 0;

const int bufferSize = 64; // Adjust buffer size as needed
char inputBuffer[bufferSize];
int bufferIndex = 0;

void setup() {
  // Set pins as outputs
  DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);

  // Set direction pins to HIGH
  PORTD |= (1 << PD3) | (1 << PD5) | (1 << PD7);

  Serial.begin(115200);
}

void loop() {
  // NOTE: Inputs cannot include '[' or ']' now, must be in format of x,y,z 
  while (Serial.available() > 0) {
    // Serial.println("Avaialable");
    char c = Serial.read();
    // if (c == '[' || c == ']') {
    //   continue;
    // }
    if (c == '\n') {
      inputBuffer[bufferIndex] = '\0'; // Null-terminate the string
      bufferIndex = 0;

      int values[3];
      // Serial.println("line39");
      parseData(inputBuffer, values);
      // Serial.println("line41");
      constrainVals(values);
      // Serial.println("line43");
      StepHandler(values);
      // Serial.println("line45");

      // Use a more efficient method to form the output string
      char output[64];
      sprintf(output, " A: %d B: %d SW: %d", APPPos, BPPPos, SWPos);
      Serial.println(output);
    } else {
      if (bufferIndex < bufferSize - 1) {
        inputBuffer[bufferIndex++] = c;
      }
    }
  }
}

void parseData(char* data, int values[]) {
  int valueidx = 0;
  char* ptr = data;
  while (*ptr != '\0' && valueidx < 3) {
    values[valueidx++] = strtol(ptr, &ptr, 10); // may need to be strtol
    if (*ptr == ',') ptr++; // Skip the commas and [
  }
}

void constrainVals(int values[]) {
  values[0] = constrain(values[0], 0, APPMaxDisplacement);
  values[1] = constrain(values[1], 0, BPPMaxDisplacement);
  values[2] = constrain(values[2], -SWMaxDisplacement, SWMaxDisplacement);
  char output2[64];
  // sprintf(output2, " V1: %d V2: %d V3: %d", values[0], values[1], values[2]);
  // Serial.println(output2);
  }

void StepHandler(int values[]) {

  if (APPPos < values[0]) {
    PORTD |= (1 << PD3);
  } else if (APPPos > values[0]) {
    PORTD &= ~(1 << PD3);
  }

  if (BPPPos > values[1]) {
    PORTD |= (1 << PD5);
  } else if (BPPPos < values[1]) {
    PORTD &= ~(1 << PD5);
  }

  if (SWPos < values[2]) {
    PORTD |= (1 << PD7);
  } else if (SWPos > values[2]) {
    PORTD &= ~(1 << PD7);
  }

  int APPDirMultiplier = (PORTD & (1 << PD3)) ? 1 : -1;
  int BPPDirMultiplier = (PORTD & (1 << PD5)) ? -1 : 1;
  int SWDirMultiplier = (PORTD & (1 << PD7)) ? 1 : -1;
  // Serial.print("APPDirMultiplier ");
  // Serial.print(APPDirMultiplier);
  // Serial.print(" BPPDirMultiplier ");
  // Serial.print(BPPDirMultiplier);
  // Serial.print(" SWDirMultiplier ");
  // Serial.print(SWDirMultiplier);
  // Serial.println(" Done");

  while (APPPos != values[0] || BPPPos != values[1] || SWPos != values[2]) {
    if (APPPos != values[0]) {
      PORTD |= (1 << PD2);
      APPPos += APPDirMultiplier;
    }

    if (BPPPos != values[1]) {
      PORTD |= (1 << PD4);
      BPPPos += BPPDirMultiplier;
    }

    if (SWPos != values[2]) {
      PORTD |= (1 << PD6);
      SWPos += SWDirMultiplier;
    }

    delayMicroseconds(delayMicros);

    PORTD &= ~(1 << PD2);
    PORTD &= ~(1 << PD4);
    PORTD &= ~(1 << PD6);
    // Serial.print("A: ");
    // Serial.print(APPPos);
    // Serial.print(" B: ");
    // Serial.print(BPPPos);
    // Serial.print(" SW: ");
    // Serial.print(SWPos);
    // Serial.println(" Done ");
    
  }
}
