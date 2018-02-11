/**
 * MIT License
 * 
 * Copyright (c) 2018 Pascal Bodin
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define traceSerial SerialUSB
#define loraSerial Serial2

#define RESET true
#define NORESET false

const char CR = '\r';
const char LF = '\n';
const char LETTER_L = 'L';

const char toStr[]   = {"> "};
const char fromStr[] = {"< "};

// RN2483 commands.
const char cmdSysGetVer[]     = {"sys get ver"};
const char cmdMacPause[]      = {"mac pause"};
const char cmdRadioSetMod[]   = {"radio set mod lora"};
const char cmdRadioSetFreq[]  = {"radio set freq 868200000"};
const char cmdRadioSetPwr[]   = {"radio set pwr 0"};
const char cmdRadioSetSf[]    = {"radio set sf sf7"};
const char cmdRadioSetAfcbw[] = {"radio set afcbw 41.7"};
const char cmdRadioSetRxbw[]  = {"radio set rxbw 125"};
const char cmdRadioSetPrlen[] = {"radio set prlen 8"};
const char cmdRadioSetCrc[]   = {"radio set crc on"};
const char cmdRadioSetIqi[]   = {"radio set iqi off"};
const char cmdRadioSetCr[]    = {"radio set cr 4/5"};
const char cmdRadioSetWdt[]   = {"radio set wdt "};
const char cmdRadioSetSync[]  = {"radio set sync 12"};
const char cmdRadioSetBw[]    = {"radio set bw 125"};
const char cmdRadioRx[]       = {"radio rx 0"};
const char cmdRadioTx[]       = {"radio tx "};

// RN2483 responses.
const char respRadioRx[]      = {"radio_rx "};
const char respRadioErr[]     = {"radio_err"};

// For hexadecimal conversion.
char hexa[] = {
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

// Temperature period, in ms.
const char tempPeriod[] = "30000";

boolean initialized;

// Constants and variables for response processing.
enum respStates {stWaitCR, stWaitLF, stWaitReset};
uint8_t currRespState = stWaitCR;

// Buffer used to store response received from RN2483.
const int8_t respBuffL = 40;
char respBuff[respBuffL];
// Length of response in response buffer (no final null character).
int8_t respL;

// Constants and variables for command processing.
const int8_t commBuffL = 40;
char commBuff[commBuffL];

// Data received from central application.
const int8_t appBuffL = 20;
char appBuff[appBuffL];

enum events {
  evEndOfWait, evMsgRec, evUnknown
};
uint8_t event;

boolean boolRs;
int intRs;

boolean blueLedOn;

 /**
 * 
 */
void setup() {

  // Wait up to 3 seconds for serial link over USB.
  while ((!traceSerial) && (millis() < 3000)){
  }

  // Define temperature pin as input;
  pinMode(TEMP_SENSOR, INPUT);
  
  // Initialize blue LED.
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize RGB LED.
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // Initialize serial links.
  traceSerial.begin(57600);
  loraSerial.begin(57600);

  traceSerial.println(F("Initializing LoRa remote device application"));

  initialized = false;

  sendCommand(cmdSysGetVer, sizeof(cmdSysGetVer) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdMacPause, sizeof(cmdMacPause) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetMod, sizeof(cmdRadioSetMod) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetFreq, sizeof(cmdRadioSetFreq) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetPwr, sizeof(cmdRadioSetPwr) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetSf, sizeof(cmdRadioSetSf) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetAfcbw, sizeof(cmdRadioSetAfcbw) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetRxbw, sizeof(cmdRadioSetRxbw) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetPrlen, sizeof(cmdRadioSetPrlen) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetCrc, sizeof(cmdRadioSetCrc) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetIqi, sizeof(cmdRadioSetIqi) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetCr, sizeof(cmdRadioSetCr) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetSync, sizeof(cmdRadioSetSync) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  sendCommand(cmdRadioSetBw, sizeof(cmdRadioSetBw) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) goto exitSetup;

  initialized = true;
  digitalWrite(LED_BUILTIN, HIGH);
  blueLedOn = true;

  ledOff();

  respL = 0;
  currRespState = stWaitCR;

  exitSetup:
  if (!initialized) traceSerial.println(F("Error during initialization"));
  
}

/**
 * 
 */
void loop() {

  uint8_t start, end;
 
  if (!initialized) return;

  traceSerial.println(F("--------------------"));
  
  // Enter continuous reception mode after having set appropriate
  // timeout.
  strcpy(commBuff, cmdRadioSetWdt);
  strcat(commBuff, tempPeriod);
  sendCommand(commBuff, strlen(commBuff));
  boolRs = waitForResponse(RESET);
  if (!boolRs) {
    traceSerial.println(F("WTD request error"));
    initialized = false;
    return;
  }
  
  sendCommand(cmdRadioRx, sizeof(cmdRadioRx) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) {
    traceSerial.println(F("RX request error"));
    initialized = false;
    return;
  }

  // A second response is expected now: either watchdog timer expiration, or
  // command message reception.
  boolRs = waitForResponse(NORESET);
  if (!boolRs) {
    traceSerial.println(F("end of RX request error"));
    initialized = false;
    return;
  }

  // Which event was it?
  event = evUnknown;
  if (respL >= (int8_t)(sizeof(respRadioRx) - 1)) {
    intRs = memcmp(respBuff, respRadioRx, sizeof(respRadioRx) - 1);
    if (intRs == 0) {
      // We received a message: this should be a command from remote application.
      event = evMsgRec;
      // Save it.
      start = sizeof(respRadioRx);
      end = respL - 1;
      for (uint8_t i = start; i <= end; i++) {
        appBuff[i - start] = respBuff[i];
      }
    }
  }
  if (respL >= (int8_t)(sizeof(respRadioErr) - 1)) {
    intRs = memcmp(respBuff, respRadioErr, sizeof(respRadioErr) - 1);
    if (intRs == 0) {
      // End of reception period.
      event = evEndOfWait;
    }
  }
  resetResponse();

  // Process event.
  switch (event) {
  case evMsgRec:
    processCommand();
    break;
  case evEndOfWait:
    sendTemperature();
    break;
  case evUnknown:
    traceSerial.println(F("ERROR: unknown event type"));
    break;
  default:
    traceSerial.print(F("ERROR: unknown event type - "));
    traceSerial.println(event);
  }

  // Toggle blue LED.
  if (blueLedOn) {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  blueLedOn = !blueLedOn;
  
}

/**
 * 
 */
void processCommand() {

  char cmd;

  // Extract command.
  cmd = hex2Char(appBuff[0], appBuff[1]);
  if (cmd == LETTER_L) {
    traceSerial.println(F("set LED command"));
    // We consider we have the right number of bytes!
    cmd = hex2Char(appBuff[2], appBuff[3]);
    switch (cmd) {
    case 'R':
      ledRed();
      break;
    case 'G':
      ledGreen();
      break;
    case 'B':
      ledBlue();
      break;
    case '0':
      ledOff();
      break;
    default:
      traceSerial.println(F("incorrect color for LED"));
    }
    return;
  }
  traceSerial.println(F("unknown command"));
  
  
}

/**
 * 
 */
void sendTemperature() {

  char tempStr[7];

  digitalWrite(LED_BUILTIN, HIGH);
  // Read temperature and send it as a value from 0 to 1023 coded in 
  // an hexadecimal string, prefixed by ASCII code for T letter.
  uint16_t mVolts = analogRead(TEMP_SENSOR);
  traceSerial.print("temperature: ");
  traceSerial.println(mVolts);
  tempStr[0] = hexa[(uint8_t)('T' >> 4 & 0x0F)];
  tempStr[1] = hexa[(uint8_t)('T' & 0x0F)];
  tempStr[2] = hexa[(uint8_t)(mVolts >> 12 & 0x0F)];
  tempStr[3] = hexa[(uint8_t)(mVolts >> 8 & 0x0F)];
  tempStr[4] = hexa[(uint8_t)(mVolts >> 4 & 0x0F)];
  tempStr[5] = hexa[(uint8_t)(mVolts & 0x0F)];
  tempStr[6] = 0;
  strcpy(commBuff, cmdRadioTx);
  strcat(commBuff, tempStr);
  sendCommand(commBuff, strlen(commBuff));
  // Wait for first response.
  boolRs = waitForResponse(RESET);
  if (!boolRs) {
    traceSerial.println(F("RX request error"));
    initialized = false;
    return;
  }
  // Wait for second response.
  boolRs = waitForResponse(RESET);
  if (!boolRs) {
    traceSerial.println(F("end of RX request error"));
    initialized = false;
  }
  digitalWrite(LED_BUILTIN, LOW);
  
}

/**
 * getResponse() reads available characters from the RN2483 serial link 
 * buffer and transfers them to the response buffer. Responses are terminated
 * by CR LF. These two characters are not stored into the response buffer.
 * 
 * Once a response is returned, getResponse() no more reads RN2483 serial
 * link buffer, until resetResponse() is called.
 * 
 * If more characters than response buffer can accomodate are received,
 * they are lost.
 * 
 * After an error (negative value) is returned, resetResponse() must be
 * called.
 * 
 * getResponse() allows to wait for RN2483 responses in a non-blocking way.
 * This capability is currently not used by this application...
 * 
 * Returned value:
 *   0: no response available
 *   l > 0: a response is available and its length is l (no terminator
 *          character, no null character)
 *   -1: too many characters received, ignored, RN2483 serial buffer is emptied
 *   -2: error in terminator sequence, RN2483 serial buffer is emptied
 *   -3: waiting for call to resetResponse()
 */
int8_t getResponse() {

  int8_t rs = 0;
  int recChar;

  boolean weLoop = true;
  
  while (weLoop) {

    if (loraSerial.available() > 0) {
      recChar = loraSerial.read();
    } else {
      return 0;
    }

    switch (currRespState) {
    case stWaitCR:
      if ((char)recChar == CR) {
        currRespState = stWaitLF;
        break;
      }
      // At this stage, a character different from CR. Store it into response buffer.
      respBuff[respL] = (char)recChar;
      respL++;
      if (respL >= respBuffL) {
        // Response too long. Flush all serial data.
        do {
          recChar = loraSerial.read();
        } while (recChar != -1);
        currRespState = stWaitReset;
        rs = -1;
        weLoop = false;
        break;
      }
      break;
    case stWaitLF:
      if ((char)recChar != LF) {
        // Inconsistant data, flush all serial data.
        do {
          recChar = loraSerial.read();
        } while (recChar != -1);
        currRespState = stWaitReset;
        rs = -2;
        weLoop = false;
        break;
      }
      // At this stage, we have a LF. Return response.
      currRespState = stWaitReset;
      rs = respL;
      weLoop = false;      
      break;
    case stWaitReset:
      rs = -3;
      weLoop = false;
      break;
    default:
      traceSerial.println(F("**** internal error: unknown state for getResponse()"));
    }
  }

  return rs;
  
}

/**
 * 
 */
void resetResponse() {

  respL = 0;
  currRespState = stWaitCR;
  
}

/**
 * Sends a command to the RN2483, adding required terminators (CR LF).
 */
void sendCommand(const char *cmd, int8_t cmdL) {

  traceSerial.print(toStr);
  for (int8_t i = 0; i < cmdL; i++) {
    loraSerial.print(cmd[i]);
    traceSerial.print(cmd[i]);
  }
  loraSerial.println();
  traceSerial.println();
  
}

/**
 * 
 */
void dumpResponse() {

  traceSerial.print(fromStr);
  for (int8_t i = 0; i < respL; i++) {
    traceSerial.print(respBuff[i]);
  }
  traceSerial.println();
  
}

/**
 * Wait for a response. Returns true if OK, false otherwise.
 * 
 * If reset parameter is set to true, resetResponse is called.
 * 
 * This is a blocking function.
 */
boolean waitForResponse(boolean reset) {

  int8_t respRs;

  while (true) {
    respRs = getResponse();
    if (respRs > 0) {
      dumpResponse();
      if (reset) resetResponse();
      return true;
    }
    if (respRs < 0) {
      return false;
    }
  }

}

/**
 * Converts two hexadecimal characters to associated ASCII char.
 */
char hex2Char(char msb, char lsb) {

  uint8_t type;
  if (msb <= '9') {
    type = (msb - '0') << 4;  // Figure.
  } else {
    type = (msb - '7') << 4;  // Letter.
  }
  if (lsb <= '9') {
    type += (lsb - '0');  // Figure.
  } else {
    type += (lsb - '7');  // Letter.
  }
  return (char)type;
  
}

/**
 * 
 */
void ledOff() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

/**
 * 
 */
void ledRed() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

/**
 * 
 */
void ledGreen() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, HIGH);
}

/**
 * 
 */
void ledBlue() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);
}
