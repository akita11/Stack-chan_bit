#include <Arduino.h>

#include <M5Unified.h>
#if defined(ARDUINO_M5STACK_Core2)
  #define SERVO_PIN_X 32
  #define SERVO_PIN_Y 27
  #define UART_PIN_TXD 14
  #define UART_PIN_RXD 13
#elif defined(ARDUINO_M5Stack_Core_ESP32)
  #define SERVO_PIN_X 12
  #define SERVO_PIN_Y 2
  #define UART_PIN_TXD 17
  #define UART_PIN_RXD 16
#endif

#define X_MIN -90  // [deg]
#define X_MAX 90   // [deg]
#define Y_MIN 0    // [deg]
#define Y_MAX 30   // [deg]
#define E_MIN 0    // emotion num
#define E_MAX 6    // emotion num

int servo_offset_x = 0;  // X軸サーボのオフセット（90°からの+-で設定）
int servo_offset_y = 0;  // Y軸サーボのオフセット（90°からの+-で設定）

#include <Avatar.h> // https://github.com/meganetaaan/m5stack-avatar
#include <ServoEasing.hpp> // https://github.com/ArminJo/ServoEasing       
#include "formatString.hpp" // https://gist.github.com/GOB52/e158b689273569357b04736b78f050d6

#define RX_BUF_SIZE 32
char RXbuf[RX_BUF_SIZE];
uint8_t pRX = 0;

using namespace m5avatar;
Avatar avatar;

const Expression expressions[] = {
  Expression::Angry,
  Expression::Sleepy,
  Expression::Happy,
  Expression::Sad,
  Expression::Doubt,
  Expression::Neutral
};

#define START_DEGREE_VALUE_X 90
#define START_DEGREE_VALUE_Y 90

ServoEasing servo_x;
ServoEasing servo_y;

uint32_t mouth_wait = 2000;  // 通常時のセリフ入れ替え時間（msec）
uint32_t last_mouth_millis = 0;

const char* lyrics[] = { "BtnA:MoveTo90  ", "BtnB:ServoTest  ", "BtnC:RandomMode  ", "BtnALong:AdjustMode" };
const int lyrics_size = sizeof(lyrics) / sizeof(char*);
int lyrics_idx = 0;

void moveX(int x, uint32_t millis_for_move = 0) {
  if (millis_for_move == 0) {
    servo_x.easeTo(x + servo_offset_x);
  } else {
    servo_x.easeToD(x + servo_offset_x, millis_for_move);
  }
}

void moveY(int y, uint32_t millis_for_move = 0) {
  if (millis_for_move == 0) {
    servo_y.easeTo(y + servo_offset_y);
  } else {
    servo_y.easeToD(y + servo_offset_y, millis_for_move);
  }
}

void moveXY(int x, int y, uint32_t millis_for_move = 0) {
  if (millis_for_move == 0) {
    servo_x.setEaseTo(x + servo_offset_x);
    servo_y.setEaseTo(y + servo_offset_y);
  } else {
    servo_x.setEaseToD(x + servo_offset_x, millis_for_move);
    servo_y.setEaseToD(y + servo_offset_y, millis_for_move);
  }
  // サーボが停止するまでウェイトします。
  synchronizeAllServosStartAndWaitForAllServosToStop();
}

void adjustOffset() {
  // サーボのオフセットを調整するモード
  servo_offset_x = 0;
  servo_offset_y = 0;
  moveXY(90, 90);
  bool adjustX = true;
  for (;;) {
    M5.update();
    if (M5.BtnA.wasPressed()) {
      // オフセットを減らす
      if (adjustX) {
        servo_offset_x--;
      } else {
        servo_offset_y--;
      }
    }
    if (M5.BtnB.pressedFor(2000)) {
      // 調整モードを終了
      break;
    }
    if (M5.BtnB.wasPressed()) {
      // 調整モードのXとYを切り替え
      adjustX = !adjustX;
    }
    if (M5.BtnC.wasPressed()) {
      // オフセットを増やす
      if (adjustX) {
        servo_offset_x++;
      } else {
        servo_offset_y++;
      }
    }
    moveXY(90, 90);

    std::string s;

    if (adjustX) {
      s = formatString("%s:%d:BtnB:X/Y", "X", servo_offset_x);
    } else {
      s = formatString("%s:%d:BtnB:X/Y", "Y", servo_offset_y);
    }
    avatar.setSpeechText(s.c_str());
  }
}

void moveRandom() {
  for (;;) {
    // ランダムモード
    int x = random(45, 135);  // 45〜135° でランダム
    int y = random(60, 90);   // 50〜90° でランダム
    M5.update();
    if (M5.BtnC.wasPressed()) {
      break;
    }
    int delay_time = random(10);
    moveXY(x, y, 1000 + 100 * delay_time);
    delay(2000 + 500 * delay_time);
    //avatar.setSpeechText("Stop BtnC");
    avatar.setSpeechText("");
  }
}

int getParam()
{
  return(atoi(RXbuf+1));
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial2.begin(115200, SERIAL_8N1, UART_PIN_RXD, UART_PIN_TXD);
  while (!Serial2)
    ;

  if (servo_x.attach(SERVO_PIN_X,
                     START_DEGREE_VALUE_X + servo_offset_x,
                     DEFAULT_MICROSECONDS_FOR_0_DEGREE,
                     DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo x");
  }
  if (servo_y.attach(SERVO_PIN_Y,
                     START_DEGREE_VALUE_Y + servo_offset_y,
                     DEFAULT_MICROSECONDS_FOR_0_DEGREE,
                     DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo y");
  }
  servo_x.setEasingType(EASE_QUADRATIC_IN_OUT);
  servo_y.setEasingType(EASE_QUADRATIC_IN_OUT);
  setSpeedForAllServos(60);
  avatar.init();
  last_mouth_millis = millis();
  //moveRandom();
}

void loop() {
  M5.update();
  if (M5.BtnA.pressedFor(2000)) {
    // サーボのオフセットを調整するモードへ
    adjustOffset();
  }
  if (M5.BtnA.wasPressed()) {
    Serial2.println("A");
    moveXY(90, 90);
  }

  if (M5.BtnB.wasPressed()) {
    Serial2.println("B");
    for (int i = 0; i < 2; i++) {
      avatar.setSpeechText("X 90 -> 0  ");
      moveX(0);
      avatar.setSpeechText("X 0 -> 180  ");
      moveX(180);
      avatar.setSpeechText("X 180 -> 90  ");
      moveX(90);
      avatar.setSpeechText("Y 90 -> 50  ");
      moveY(50);
      avatar.setSpeechText("Y 50 -> 90  ");
      moveY(90);
    }
  }
  if (M5.BtnC.wasPressed()) {
    // ランダムモードへ
    moveRandom();
  }
/*
  if ((millis() - last_mouth_millis) > mouth_wait) {
    const char* l = lyrics[lyrics_idx++ % lyrics_size];
    avatar.setSpeechText(l);
    avatar.setMouthOpenRatio(0.7);
    delay(200);
    avatar.setMouthOpenRatio(0.0);
    last_mouth_millis = millis();
  }
  */
  if (Serial2.available()) {
    char c = Serial2.read();
    RXbuf[pRX++] = c;
    if (c == '\n' || pRX >= RX_BUF_SIZE - 1) {
      RXbuf[pRX] = '\0';
      pRX = 0;
      Serial.println(RXbuf);
      int param;
      switch (RXbuf[0]) {
        case 'X':
          param = getParam();
          if (param < X_MIN) param = X_MIN;
          if (param > X_MAX) param = X_MAX;
//	        Serial.println(param);
          moveX(param + START_DEGREE_VALUE_X);
          break;
        case 'Y':
          param = getParam();
          if (param < Y_MIN) param = Y_MIN;
          if (param > Y_MAX) param = Y_MAX;
//	        Serial.println(param);
          moveY(-param + START_DEGREE_VALUE_Y);
          break;
        case 'T':
          avatar.setSpeechText(RXbuf + 1);
	  for (int i = 0; i < 3; i++){
            avatar.setMouthOpenRatio(0.7);
            delay(200);			
            avatar.setMouthOpenRatio(0);
            delay(200);			
	  }
          break;
        case 'E':
          param = getParam();
          if (param < E_MIN) param = E_MIN;
          if (param > E_MAX) param = E_MAX;
          avatar.setExpression(expressions[param]);
          break;
      }
    }
  }
}
