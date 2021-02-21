  #include <Max3421e.h>
  #include <Usb.h>
  #include <AndroidAccessory.h>
  #include<Keypad.h>
  #include<EEPROM.h>
  #include<Stepper.h>
  
  // Constant
  #define AOA_DELAY 100
  #define RED_PIN 22 // RED LED
  #define GREEN_PIN 23 // GREEN LED
  #define MAX_OTP_USED_NUM 127 // Otp 최대 이용 횟수
  // Command
  #define CMD_CHG_PW 0x7 // Command Change Password
  #define CMD_CHG_OPT 0x8 // (TODO : Reserved) Command Change One-time-password
  #define CMD_REQUEST_OTP_USED_NUM 0x9 // 안드로이드로부터 OTP 이용 횟수 요청이 왔다
  // State
  #define STATE_DEBUG 0x0
  #define STATE_START_INPUT 0x1
  #define STATE_OPEN_OUTSIDE 0x2
  #define STATE_OPEN_INSIDE 0x3
  #define STATE_FAIL 0x4
  #define STATE_SEND_OTP_USED_NUM 0x5
  
  #define BTN_PIN 14 // BUTTON
  #define BTN_PW 15 // Button Power
  
  // Accessory instance
  AndroidAccessory acc("Manufacturer name",
    "Model",
    "Description",
    "1.0",
    "http://www.example.com",
    "Serial number");
  
  // Keypad instance
  #define ROWS 4
  #define COLS 3
  char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
  };
  // Key pad
  byte rowPins[ROWS] = {37, 39, 41, 43};
  byte colPins[COLS] = {45, 47, 49};
  Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
  
  // Password & OTP array
  char *secretCode = "1234";
  char *otpCode = "????";
  int position = 0; // passwoard check position
  
  // Feedback Notification
  void flash() {
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, HIGH);
    delay(500);
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    Serial.println("flash()!!");
  }
  
  // Debug Print
  void debugPrintCommand(byte msg0, byte msg1, byte msg2) {
    if(msg0 == CMD_CHG_PW) {
      if(msg1 > 3) {
        Serial.println("debugPrintCommand() : index Error");
        return;
      }
      Serial.print("Android Command Receive : chage Password secretCode[] index ");
      Serial.print(msg1, DEC);
      Serial.print(" change [ ");
      Serial.print(secretCode[msg1], DEC);
      Serial.print(" -> ");
      Serial.print((char)msg2, DEC);
      Serial.println(" ]");
    }
    else if(msg0 == CMD_CHG_OPT) {
      if(msg1 > 3) {
        Serial.println("debugPrintCommand() : index Error");
        return;
      }
      Serial.print("Android Command Receive : chage OTP otpCode[] index ");
      Serial.print(msg1, DEC);
      Serial.print(" change [ ");
      Serial.print(otpCode[msg1], DEC);
      Serial.print(" -> ");
      Serial.print((char)msg2, DEC);
      Serial.println(" ]");
    }
    else if(msg0 == CMD_REQUEST_OTP_USED_NUM) {
      Serial.print("Android Command Receive : request otp used num (");
      Serial.print(EEPROM.read(5));
      Serial.println(")");
    }
    else {
      Serial.println("debugPrintCommand() : Message From Android is Error");
    }
  }
  
  void execCommand(byte msg0, byte msg1, byte msg2) { // execution of command
    if(msg0 == CMD_CHG_PW) { // Change Password Command
      if(msg1 > 3) { // index error checking ~
        Serial.println("execCommand() : index Error");
        return;
      }
      savePwCode(msg1, msg2);
    }
    else if(msg0 == CMD_CHG_OPT) {
      if(msg1 > 3) { // index error checking ~
        Serial.println("execCommand() : index Error");
        return;
      }
      saveOtpCode(msg1, msg2);
    }
    else if(msg0 == CMD_REQUEST_OTP_USED_NUM) {
      sendStateToAndroid(STATE_SEND_OTP_USED_NUM, (uint16_t)EEPROM.read(5));
    }
    else {
      Serial.println("execCommand() : Message From Android is Error");
    }
  }
  
  void savePwCode(byte msg1, byte msg2) {
    
    secretCode[msg1] = msg2;
    EEPROM.write(msg1 + 1, secretCode[msg1]);
  
    EEPROM.write(0, 1); // change flag setting ~~!!
    
    Serial.print("Code changed to : ");
    Serial.println(secretCode);
  }
  
  void saveOtpCode(byte msg1, byte msg2) {
    
    otpCode[msg1] = msg2;
  //  EEPROM.write(msg1 + 6, otpCode[msg1]); //not used OTP !!!!!!!!!
  
  //  EEPROM.write(5, 1); // change flag setting ~~!! //not used OTP !!!!!!!!!
    
    Serial.print("Code changed to : ");
    Serial.println(otpCode);
  }
  
  // seaved password from EEPROM
  void loadCode() {
    if(EEPROM.read(0) == 1) { // is saved password(Checking ~!!)
      secretCode[0] = EEPROM.read(1);
      secretCode[1] = EEPROM.read(2);
      secretCode[2] = EEPROM.read(3);
      secretCode[3] = EEPROM.read(4);
    }
  }
  
  void lock() {
    position = 0; // rewind
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
  
    // CLOSE ㅠ ㅠ!!!!!!!! Good bye
    closeMotor();
    Serial.println("LOCKED");
  }
  
  void unlock() {
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, HIGH);
    
    // OPEN >_< !!!!!!!!!! congratulation
    openMotor();
    Serial.println("UN-LOCKED");
    delay(3000); // 5second Hold On please
    
    // ROCK ~
    lock();
  }


  /***************************** Motor ********************************/

  // give the motor control pins names:
  #define dirA 12  // Direction pin dirA on Motor Control Shield
  #define dirB 13  // Direction pin dirB on Motor Control Shield
  #define pwmA 3
  #define pwmB 11
  #define brakeA 9
  #define brakeB 8
  // change this to fit the number of steps per revolution
  #define stepsPerRevolution 30
  
  // Initialize the stepper library on pins 8, 9, 12, 13:
  Stepper myStepper(stepsPerRevolution, brakeB, dirA, brakeA, dirB);
  
  void openMotor() {
    // Step revolutions into one direction:
    myStepper.step(stepsPerRevolution);
    delay(200);
  }
  
  void closeMotor() {
    // Step five revolutions in the other direction:
    myStepper.step(-stepsPerRevolution);
    delay(200);
  }
  
  /*******************************************************************/
  
  void setup() {
    // Set speed to 120 rpm:
    myStepper.setSpeed(200);
  
    // Motor ~!!
    pinMode(pwmA, OUTPUT); // 3
    digitalWrite(pwmA, HIGH);
    pinMode(pwmB, OUTPUT); // 11
    digitalWrite(pwmB, HIGH);
    pinMode(brakeA, OUTPUT); // 9
    digitalWrite(brakeA, LOW);
    pinMode(brakeB, OUTPUT); // 8
    digitalWrite(brakeB, LOW);
    
    // pinMode Settings
    pinMode(RED_PIN, OUTPUT); // red led
    pinMode(GREEN_PIN, OUTPUT); // green led
    pinMode(BTN_PIN, INPUT); // // Button
    pinMode(BTN_PW, OUTPUT); // Button Poewr
    digitalWrite(BTN_PW, HIGH);
    
    openMotor();
    closeMotor();
    
    // baud rate (bit/s) Setting
    Serial.begin(115200);
    
    // get seaved password from EEPROM
    loadCode();
    flash();
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
      
    Serial.print("Password Code is: ");
    Serial.println(secretCode);
    Serial.print("OTP Code is: ");
    Serial.println(otpCode);
    
    if(EEPROM.read(5) > MAX_OTP_USED_NUM)
      EEPROM.write(5, 0);
    Serial.print("OTP used number: ");
    Serial.println(EEPROM.read(5));
    
    // Accessroy mode ON !!!
    acc.powerOn();
    Serial.println("\r\nADK has setup().");
    Serial.println("Ready to start reading the temp...");
  }
  
  bool pwInputFlag = false;
  bool otpInputFlag = false;
  
  void rewindPosition() {
    pwInputFlag = false;
    otpInputFlag = false;
    position = 0; // rewind
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
  }
  void checkCode(char key) {
    if(key == 0)
      return; // empty therfore checking stop
      
    // input # ???
    if((key == '#') && !pwInputFlag && !otpInputFlag) {
      pwInputFlag = true; // !!!!!!!!! >_<
      // flash(); // feedback
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      Serial.println("Please Input Password ~!");
      
      sendStateToAndroid(STATE_START_INPUT, 0);
      
      return;
    }
    else if((key == '#')) {
      rewindPosition();
      // flash(); // feedback
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      
      Serial.println("LOCKED (Why? Duplicated '#' input OR During input One-time-password)");
      sendStateToAndroid(STATE_FAIL, 0);
      
      return;
    }
    // input * ???
    if((key == '*') && !pwInputFlag && !otpInputFlag) {
      otpInputFlag = true; // !!!!!!!!! >_<
      // flash(); // feedback
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      Serial.println("Please One-time-password ~!");
      
      sendStateToAndroid(STATE_START_INPUT, 0);
      
      return;
    }
    else if((key == '*')) {
      rewindPosition();
      // flash(); // feedback
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      
      Serial.println("LOCKED (Why? Duplicated '*' input) OR During input Real-password");
      sendStateToAndroid(STATE_FAIL, 0);
      
      return;
    }
    
    if(!pwInputFlag && !otpInputFlag) {
      // flash(); // feedback
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);
      
      Serial.println("LOCKED (Why? first input => (pw:'#') or (otp:'*'))");
      return;
    }
    
    if(pwInputFlag) {
      if(key == secretCode[position]) {
        Serial.print("Password Match position(");
        Serial.print(position, DEC);
        Serial.println(") ^^");
        position++;
        
        if(position == 4) {
          Serial.println("Perfect Password !!!");
          
          sendStateToAndroid(STATE_OPEN_OUTSIDE, 0);
          
          rewindPosition();
          unlock();
        }
        
        return;
      }
      else {
        rewindPosition();
        Serial.println("LOCKED (Why? Wrong Number Input ...)");
        
        sendStateToAndroid(STATE_FAIL, 0);
        
        return;
      }
    }
    else if(otpInputFlag) {
      if(key == otpCode[position]) {
        Serial.print("One-time-password Match position(");
        Serial.print(position, DEC);
        Serial.println(") ^^");
        position++;
        
        if(position == 4) {
          Serial.println("Perfect One-time-password !!!");
          
          sendStateToAndroid(STATE_OPEN_OUTSIDE, 0);
          
          uint16_t otpUsedNum = EEPROM.read(5);
          if((++otpUsedNum) > MAX_OTP_USED_NUM)
            otpUsedNum = 0;
          EEPROM.write(5, otpUsedNum);
          Serial.print("otp used number change~~~!!!: ");
          Serial.println(EEPROM.read(5));
          
          sendStateToAndroid(STATE_SEND_OTP_USED_NUM, (uint16_t)EEPROM.read(5));
          
          rewindPosition();
          unlock();
        }
        
        return;
      }
      else {
        rewindPosition();
        Serial.println("LOCKED (Why? Wrong Number Input ...)");
        
        sendStateToAndroid(STATE_FAIL, 0);
        
        return;
      }
    }
    else {
      rewindPosition();
      Serial.println("I don't know.. Very Serious Error");
      
      sendStateToAndroid(STATE_DEBUG, 444);
    }
  }
  
  byte msg[3]; // message buffer from Android & to Android
  void loop() {
    if(acc.isConnected()) {
      // read command from Android
      int len = acc.read(msg, sizeof(msg), 1);
      if(len > 0) {
        debugPrintCommand(msg[0], msg[1], msg[2]);
        execCommand(msg[0], msg[1], msg[2]);
      }
      delay(100); // 100ms because AOA
      
      // Button variable
      if(digitalRead(BTN_PIN)) { // Button pressed .. therefore HIGH ~!!
         Serial.println("inside button is pressed >_<");
         sendStateToAndroid(STATE_OPEN_INSIDE, 0);
         unlock();
      }
      
      char key = keypad.getKey();
      checkCode(key);
    }
    else { // Disconnection
      delay(AOA_DELAY); // 100ms because AOA
    }
  }
  
  void sendStateToAndroid(byte state, uint16_t extra) {
  
    msg[0] = state;
    msg[1] = extra >> 8;
    msg[2] = extra & 0xff;
    
    acc.write(msg, 3);
    delay(AOA_DELAY); // 100ms because AOA
  }
