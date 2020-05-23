#include <SoftwareSerial.h>// import the serial library
#include <DFRobotDFPlayerMini.h>
#include <Servo.h>
#define ledPin 3 //red led pin
#define servoPin 9 //eyeServo pin

static const uint8_t PIN_MP3_RX = 2; // Connects to module's RX
static const uint8_t PIN_MP3_TX = 4; // Connects to module's TX
SoftwareSerial mp3PlayerSerial(PIN_MP3_RX, PIN_MP3_TX);

static const uint8_t PIN_BT_RX = 6; // Connects to module's RX
static const uint8_t PIN_BT_TX = 5; // Connects to module's TX
SoftwareSerial BTserial(PIN_BT_RX, PIN_BT_TX); // RX, TX

Servo eyeServo; //servo
DFRobotDFPlayerMini soundPlayer;

boolean started = false;
int eyeServopos = 90;
int BT_state = 0;
int brightness = 0;
int fadeAmount = 5;
int delayLedBright = 60;
int delayServo = 15;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  eyeServo.attach(servoPin);
  eyeServo.write(eyeServopos);
  Serial.begin(9600); 
  BTserial.begin(19200);
  mp3PlayerSerial.begin(9600);
  Serial.println("ready");
  if (soundPlayer.begin(mp3PlayerSerial)) {
    Serial.println("MP3 OK");
    soundPlayer.volume(25);

  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
}

void loop() {
  if (BTserial.available()) {
    //Serial.println("connected");
    BT_state = BTserial.read();
    Serial.println(BT_state);
  }else{
    BT_state = Serial.read();
  }

  switch (BT_state) {

    case '0':
      BTserial.print("LED|OFF");
      Serial.println("OFF");
      while (brightness > 0) {
        brightness -= fadeAmount;
        analogWrite(ledPin, brightness);
        delay(delayLedBright);
      }
      started = false;
      resetBTState();
      break;
    case '1':
      BTserial.print("LED|ON");
      Serial.println("ON");
      soundPlayer.play(1);
      while (brightness < 255) {
        analogWrite(ledPin, brightness);
        brightness += fadeAmount;
        delay(delayLedBright);
      }
      started = true;
      resetBTState();
      break;
    case '5':
      if (started) {

      }
      resetBTState();
      break;
    case '6':
      if (started) {
        eyeServo.write(10);
        delay(500);
        servoRotate(30, 130, eyeServo, delayServo);
        delay(500);
        eyeServo.write(99);
        delay(500);
        servoRotate(99, 40, eyeServo, delayServo);
        delay(500);
        eyeServo.write(150);
        eyeServopos = 150;
      }

      resetBTState();
      break;
    case 'L':
      if (started) {
        while (eyeServopos > 30) {
          eyeServopos -= 1;
          eyeServo.write(eyeServopos);
          delay(delayServo);
          if (BTserial.available() && BTserial.read() == 'l') { //this is L
            resetBTState();
            break;
          }else if(Serial.read() == 'l'){
            resetBTState();
            break;            
          }
        }
      }

      resetBTState();
      break;
    case 'R':
      if (started) {
        while (eyeServopos < 150) {
          eyeServopos += 1;
          eyeServo.write(eyeServopos);
          delay(delayServo);
          if (BTserial.available() && BTserial.read() == 'r') {
            resetBTState();
            break;
          }else if(Serial.read() == 'r'){
            resetBTState();
            break;            
          }
        }
      }

      resetBTState();
      break;


    default:
      resetBTState();
      break;
  }

}

void servoRotate(int startD, int endD, Servo servoT, int delayTimeS) {
  if (startD < endD) {
    for (int pos = startD; pos <= endD; pos++) {
      servoT.write(pos);
      delay(delayTimeS);
    }
  } else if (startD > endD) {
    for (int pos = startD; pos >= endD; pos--) {
      servoT.write(pos);
      delay(delayTimeS);
    }
  }

}

void resetBTState() {
  BT_state = 0;
}


/*
  49(1) > start
  48(0) > end

  76(L) > left down
  108(l) > left up

  82(R) > right down
  114(r) > right up

  53(5) > mode 1
  54(6) > mode 2
*/
