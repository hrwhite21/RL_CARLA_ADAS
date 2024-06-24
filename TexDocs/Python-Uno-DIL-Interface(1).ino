const int APPstepPin = 2;  // Step pin connected to digital pin 2
const int APPdirPin = 3;   // Direction pin connected to digital pin 3
const int BPPstepPin = 4;  // Step pin connected to digital pin 4
const int BPPdirPin = 5;   // Direction pin connected to digital pin 5
const int SWstepPin = 6;   // Step pin connected to digital pin 6
const int SWdirPin = 7;    // Direction pin connected to digital pin 7
// The following values are based on hand calibration
const int delayMicros = 700; // 5000 microseconds = 5 milliseconds
const int APPMaxDisplacement = 690; 
const int BPPMaxDisplacement = 660;
const int SWMaxDisplacement = 400; //1210 - this changes becasue of the 3.95 scaling in python on inputs,
                                  // but doesn't HAVE to, necessarily.;
int APPPos = 0;
int BPPPos = 0;
int SWPos = 0;
const int printInterval = 1000;
unsigned long previousMillis = millis();
void setup() {
  // put your setup code here, to run once:
  pinMode(APPstepPin, OUTPUT);
  pinMode(APPdirPin, OUTPUT);
  pinMode(BPPstepPin, OUTPUT);
  pinMode(BPPdirPin, OUTPUT);
  pinMode(SWstepPin, OUTPUT);
  pinMode(SWdirPin, OUTPUT);

  digitalWrite(APPdirPin, HIGH);
  digitalWrite(BPPdirPin, HIGH);
  digitalWrite(SWdirPin, HIGH);

  Serial.begin(19200);
}

void loop() {
/*
  if ( millis() - previousMillis >= printInterval) {
    Serial.println("APP POS: " + String(APPPos) + " BPP POS: " + String(BPPPos) + " SW POS: " + String(SWPos));
    previousMillis = millis();
    
    if (previousMillis > millis()){
      previousMillis = millis() -1;
    }
  }
*/

 if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n'); // Read data until newline character
    int values[3];
    parseData(data, values); // Parse and process the received data
    constrainVals(values);
    StepHandler(values);
    Serial.println(" A: " + String(APPPos) + " B: " + String(BPPPos) + " SW: " + String(SWPos));

  }

}

void parseData(String data, int values[]) {
    int valueidx = 0;
    // Find the first digit in the input string
    char* ptr = data.c_str();
    while (*ptr != '\0') {
        if (isdigit(*ptr)) {
            break;
        }
        ptr++;
    }
    // Start tokenizing from the first digit found
    ptr = strtok(ptr, ",");
    while (ptr != NULL && valueidx < 3) {
        // Convert token to integer using strtol
        values[valueidx++] = strtol(ptr, NULL, 10);
        ptr = strtok(NULL, ",]"); // Get the next token, also consider ']'
    }
}

void constrainVals(int values[]) {
 // Should also double check negatives.
  values[0] = constrain(values[0],0, APPMaxDisplacement);
  values[1] = constrain(values[1],0, BPPMaxDisplacement);
  values[2] = constrain(values[2],-SWMaxDisplacement, SWMaxDisplacement);
}

void StepHandler(int values[]) {
  if (APPPos < values[0]) {
    digitalWrite(APPdirPin, HIGH);
  } else if (APPPos > values[0]) {
    digitalWrite(APPdirPin, LOW);
  }
  
  if (BPPPos > values[1]) {
    digitalWrite(BPPdirPin, HIGH);
  } else if (BPPPos < values[1]) {
    digitalWrite(BPPdirPin, LOW);
  }
  
  if (SWPos < values[2]) {
    digitalWrite(SWdirPin, HIGH);
  } else if (SWPos > values[2]) {
    digitalWrite(SWdirPin, LOW);
  }

  while (APPPos != values[0] || BPPPos != values[1] || SWPos != values[2]) {
    if (APPPos != values[0]) {
      digitalWrite(APPstepPin, HIGH);
      APPPos += (2 * digitalRead(APPdirPin) - 1);
    }

    if (BPPPos != values[1]) {
      digitalWrite(BPPstepPin, HIGH);
      BPPPos += (-2 * digitalRead(BPPdirPin) + 1);
    }

    if (SWPos != values[2]) {
      digitalWrite(SWstepPin, HIGH);
      SWPos += (2 * digitalRead(SWdirPin) - 1);
    }

    delayMicroseconds(delayMicros);

    digitalWrite(APPstepPin, LOW);
    digitalWrite(BPPstepPin, LOW);   
    digitalWrite(SWstepPin, LOW);
  }
}


