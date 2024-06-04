// Define pin mappings
const int APPstepPin = 2;  // Step pin connected to digital pin 2 (PD2)
const int APPdirPin = 3;   // Direction pin connected to digital pin 3 (PD3)
const int BPPstepPin = 4;  // Step pin connected to digital pin 4 (PD4)
const int BPPdirPin = 5;   // Direction pin connected to digital pin 5 (PD5)
const int SWstepPin = 6;   // Step pin connected to digital pin 6 (PD6)
const int SWdirPin = 7;    // Direction pin connected to digital pin 7 (PD7)

const int delayMicros = 700; // 5000 microseconds = 5 milliseconds
const int APPMaxDisplacement = 690; 
const int BPPMaxDisplacement = 660;
const int SWMaxDisplacement = 400;

int APPPos = 0;
int BPPPos = 0;
int SWPos = 0;

const int printInterval = 1000;
unsigned long previousMillis = millis();

void setup() {
  // Set pins as outputs
  DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7);

  // Set direction pins to HIGH
  PORTD |= (1 << PD3) | (1 << PD5) | (1 << PD7);

  Serial.begin(19200);
}

void loop() {
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
    char* ptr = data.c_str();
    while (*ptr != '\0') {
        if (isdigit(*ptr)) {
            break;
        }
        ptr++;
    }
    ptr = strtok(ptr, ",");
    while (ptr != NULL && valueidx < 3) {
        values[valueidx++] = strtol(ptr, NULL, 10);
        ptr = strtok(NULL, ",]"); // Get the next token, also consider ']'
    }
}

void constrainVals(int values[]) {
  values[0] = constrain(values[0], 0, APPMaxDisplacement);
  values[1] = constrain(values[1], 0, BPPMaxDisplacement);
  values[2] = constrain(values[2], -SWMaxDisplacement, SWMaxDisplacement);
}

void StepHandler(int values[]) {
  if (APPPos < values[0]) {
    PORTD |= (1 << PD3); // Set APPdirPin HIGH
  } else if (APPPos > values[0]) {
    PORTD &= ~(1 << PD3); // Set APPdirPin LOW
  }

  if (BPPPos > values[1]) {
    PORTD |= (1 << PD5); // Set BPPdirPin HIGH
  } else if (BPPPos < values[1]) {
    PORTD &= ~(1 << PD5); // Set BPPdirPin LOW
  }

  if (SWPos < values[2]) {
    PORTD |= (1 << PD7); // Set SWdirPin HIGH
  } else if (SWPos > values[2]) {
    PORTD &= ~(1 << PD7); // Set SWdirPin LOW
  }

  while (APPPos != values[0] || BPPPos != values[1] || SWPos != values[2]) {
    if (APPPos != values[0]) {
      PORTD |= (1 << PD2); // Set APPstepPin HIGH
      APPPos += (2 * ((PORTD & (1 << PD3)) >> PD3) - 1);
    }

    if (BPPPos != values[1]) {
      PORTD |= (1 << PD4); // Set BPPstepPin HIGH
      BPPPos += (2 * ((PORTD & (1 << PD5)) >> PD5) - 1);
    }

    if (SWPos != values[2]) {
      PORTD |= (1 << PD6); // Set SWstepPin HIGH
      SWPos += (2 * ((PORTD & (1 << PD7)) >> PD7) - 1);
    }

    delayMicroseconds(delayMicros);

    PORTD &= ~(1 << PD2); // Set APPstepPin LOW
    PORTD &= ~(1 << PD4); // Set BPPstepPin LOW
    PORTD &= ~(1 << PD6); // Set SWstepPin LOW
  }
}
