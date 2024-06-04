const int bufferSize = 64; // Adjust buffer size as needed
char inputBuffer[bufferSize];
int bufferIndex = 0;

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      inputBuffer[bufferIndex] = '\0'; // Null-terminate the string
      bufferIndex = 0;
      
      int values[3];
      parseData(inputBuffer, values);
      constrainVals(values);
      StepHandler(values);
      
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
    values[valueidx++] = strtol(ptr, &ptr, 10);
    if (*ptr == ',') ptr++; // Skip the comma
  }
}

void constrainVals(int values[]) {
  values[0] = constrain(values[0], 0, APPMaxDisplacement);
  values[1] = constrain(values[1], 0, BPPMaxDisplacement);
  values[2] = constrain(values[2], -SWMaxDisplacement, SWMaxDisplacement);
}

void StepHandler(int values[]) {
  int APPDirMultiplier = (PORTD & (1 << PD3)) ? 1 : -1;
  int BPPDirMultiplier = (PORTD & (1 << PD5)) ? 1 : -1;
  int SWDirMultiplier = (PORTD & (1 << PD7)) ? 1 : -1;

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
  }
}
