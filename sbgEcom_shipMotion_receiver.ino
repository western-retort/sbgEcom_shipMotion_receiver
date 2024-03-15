/*******************************
sbgEcom_shipMotion_receiver.ino
J.Serrand
14.03.24
*******************************/
const uint8_t SBG_E_COM_MAX_BYTES = 52;
const uint8_t START_BYTE_HEAVE_PERIOD = 10; // HEAVE_PERIOD start byte in SBG_ECOM_LOG_SHIP_MOTION
const uint8_t START_BYTE_HEAVE = 22; // HEAVE start byte in SBG_ECOM_LOG_SHIP_MOTION
uint8_t E_COM_START_MARKER = 0xFF; // Start marker of a sbgECom frame

uint8_t receivedBytes[SBG_E_COM_MAX_BYTES];
float heavePeriod=0, heave=0;


/************************************
          Arduino Setup
************************************/
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.println("<Arduino is ready>");
}


/************************************
        Arduino main loop
************************************/
void loop() {
  getSbgECom();
  processShipMotion();
}


/************************************
    Hexadecimal to Float converter
************************************/
float hexToFloat(uint8_t *hexTab) {

    uint32_t hexNumber{0}, buffer{0};
    int shift{0};
    float floatNumber{0};

    for (int i{0}; i < 4; i++) {
        buffer=hexTab[i];
        hexNumber |= buffer << shift;
        shift+=8;
    }
    memcpy(&floatNumber, &hexNumber, sizeof(hexNumber));

    return floatNumber;
}


/************************************
        Process Conversions
************************************/
void processShipMotion() {
    heavePeriod=hexToFloat(&(receivedBytes[START_BYTE_HEAVE_PERIOD]));
    heave=hexToFloat(&(receivedBytes[START_BYTE_HEAVE]));

  // DISPLAY Arduino
    Serial.println();
    Serial.print("HEAVE_PERIOD = ");
    Serial.println(heavePeriod);
    Serial.print("HEAVE = ");
    Serial.println(heave);
}


/************************************
    sbgEComm Serial Acquisition
************************************/
void getSbgECom() {
    static bool rcptInProgress = false;
    static uint8_t bytesCounter = 0; // Count the number of bytes acquired
    uint8_t currentByte=0; // Currrent byte read from Serial buffer
   
    while (Serial1.available() > 0) {
        currentByte = Serial1.read();
        
        // RECEPTION IN PROGRESS
        if (rcptInProgress) {
            if (bytesCounter < SBG_E_COM_MAX_BYTES) {
                receivedBytes[bytesCounter] = currentByte;
                bytesCounter++;    
            }
            // END OF RECEPTION
            else {
                receivedBytes[bytesCounter] = '\0'; // Terminate the string
                rcptInProgress = false;
                bytesCounter = 0;
            }
        }

        // NEW RECEPTION
        else if (currentByte == E_COM_START_MARKER) {
            rcptInProgress = true;
            receivedBytes[bytesCounter] = currentByte;
            bytesCounter++;
        }
    }
}