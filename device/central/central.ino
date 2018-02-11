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

const char CR = (int8_t)'\r';
const char LF = (int8_t)'\n';
const char LETTER_B = (int8_t)'B';
const char LETTER_G = (int8_t)'G';
const char LETTER_L = (int8_t)'L';
const char LETTER_R = (int8_t)'R';
const char FIGURE_0 = (int8_t)'0';

const char toStr[]   = {"T> "};
const char fromStr[] = {"T< "};

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

// Command polling period, in ms.
const char commPeriod[] = "5000";

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

// For commands sent by PC application.
const int8_t appBuffL = 5;
char appBuff[appBuffL];
int8_t appL;

// Data received from remote device.
const int8_t remBuffL = 20;
char remBuff[remBuffL];

enum events {
  evEndOfWait, evMsgRec, evUnknown
};
uint8_t event;

char msgType;

boolean boolRs;
int intRs;

boolean blueLedOn;

const int ledMsgL = 4;    // L x CR LF

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

  traceSerial.println(F("Initializing LoRa central device application"));

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
  digitalWrite(LED_BUILTIN, LOW);
  blueLedOn = true;

  ledOff();

  respL = 0;
  currRespState = stWaitCR;

  appL = 0;

  exitSetup:
  if (!initialized) traceSerial.println(F("Error during initialization"));
  
}

/**
 * 
 */
void loop() {

  uint8_t start, end;

  if (!initialized) return;

  traceSerial.println(F("T--------------------"));

  // Enter continuous reception mode after having set appropriate
  // timeout.
  strcpy(commBuff, cmdRadioSetWdt);
  strcat(commBuff, commPeriod);
  sendCommand(commBuff, strlen(commBuff));
  boolRs = waitForResponse(RESET);
  if (!boolRs) {
    traceSerial.println(F("TWTD request error"));
    initialized = false;
    return;
  }
  
  sendCommand(cmdRadioRx, sizeof(cmdRadioRx) - 1);
  boolRs = waitForResponse(RESET);
  if (!boolRs) {
    traceSerial.println(F("TRX request error"));
    initialized = false;
    return;
  }
  
  // A second response is expected now, when the watchdog timer expires, or
  // when a command message is received.
  boolRs = waitForResponse(NORESET);
  if (!boolRs) {
    traceSerial.println(F("Tend of RX request error"));
    initialized = false;
    return;
  }
  
  // Which event was it?
  event = evUnknown;
  if (respL >= (int8_t)(sizeof(respRadioRx) - 1)) {
    intRs = memcmp(respBuff, respRadioRx, sizeof(respRadioRx) - 1);
    if (intRs == 0) {
      // We received a message: this should be temperature.
      event = evMsgRec;
      // Save it.
      start = sizeof(respRadioRx);
      end = respL - 1;
      for (uint8_t i = start; i <= end; i++) {
        remBuff[i - start] = respBuff[i];
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
    // Check message type. We consider we have the right number of bytes.
    msgType = hex2Char(remBuff[0], remBuff[1]);
    switch (msgType) {
    case 'T':
      processTemperature();
      break;
    default:
      traceSerial.print(F("Tunknown message type: "));
      traceSerial.println(msgType);
    }
    break;
  case evEndOfWait:
    // Nothing special in this case.
    break;
  case evUnknown:
    traceSerial.println(F("TERROR: unknown event type"));
    break;
  default:
    traceSerial.print(F("TERROR: unknown event type - "));
    traceSerial.println(event);
  }
  
  // Check whether the application sent us a command for remote device, and
  // send it, if present.
  processCommand();

  // Toggle blue LED.
  if (blueLedOn) {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  blueLedOn = !blueLedOn;
  
}

/**
 * Checks whether a command from the application is present in serial link buffer.
 * We currently support only one command: set RGB LED, so our check is very simple
 * and naive.
 */
void processCommand() {

  char tempStr[5];
  int recChar;

  // Get all possibly waiting characters.
  recChar = traceSerial.read();
  while (recChar != -1) {
    appBuff[appL] = (char)recChar;
    appL++;
    // A very small delay seems to be required, in order to get all characters
    // waiting in serial-over-USB RX buffer! Otherwise, control is returned to
    // main loop, and command is processed in next iteration only. To be
    // investigated...
    delay(1);
    recChar = traceSerial.read();
  }
  if (appL >= 4) {
    if (appBuff[0] == LETTER_L && appBuff[2] == CR && appBuff[3] == LF) {
      appL = 0;
      // LED command. Forward it.
      tempStr[0] = hexa[(uint8_t)(LETTER_L >> 4 & 0x0F)];
      tempStr[1] = hexa[(uint8_t)(LETTER_L & 0x0F)];
      tempStr[2] = hexa[(uint8_t)(appBuff[1] >> 4 & 0x0F)];
      tempStr[3] = hexa[(uint8_t)(appBuff[1] & 0x0F)];
      tempStr[4] = 0;
      strcpy(commBuff, cmdRadioTx);
      strcat(commBuff, tempStr);
      sendCommand(commBuff, strlen(commBuff));
      // Wait for first response.
      boolRs = waitForResponse(RESET);
      if (!boolRs) {
        traceSerial.println(F("TRX request error"));
        return;
      }
      // Wait for second response.
      boolRs = waitForResponse(RESET);
      if (!boolRs) {
        traceSerial.println(F("Tend of RX request error"));
        return;
      }
    } else {
      traceSerial.println(F("Tunknown command"));
      appL = 0;
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
void processTemperature() {

  // Forward message to the application.
  // First send prefix.
  traceSerial.print('A');
  // Then send received message.
  for (uint8_t i = 0; i < 7; i++) {
    traceSerial.print(remBuff[i]);
  }
  traceSerial.println();
  
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
 * 
 */
void ledOff() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}
