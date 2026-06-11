#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  133 
#define SERVOMAX  481 

int potHand     = A0;
int potElbow    = A1;
int potShoulder = A2;
int potBase     = A3;

int buttonPin = 13;

int hand     = 11;
int wrist    = 12;
int elbow    = 13;
int shoulder = 14;
int base     = 15;

// مصفوفة لحفظ آخر قراءة لكل مقاومة
int lastPotVals[4] = {0, 0, 0, 0}; 

// متغير لحفظ حالة الزر السابقة (لمنع تكرار الطباعة)
int lastButtonState = HIGH;

void setup() 
{
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(50); 
  pinMode(buttonPin, INPUT_PULLUP);
  delay(500); 
  
  Serial.println("System Ready! Waiting for movements...");
  Serial.println("-------------------------------------");
}

// دالة تحريك المحرك مع طباعة الحركة في السيريال مونيتور
void moveMotorSmooth(int potPin, int motorPin, int index)
{
  int currentVal = analogRead(potPin);
  
  // التحقق من وجود حركة حقيقية وتجاوز التشويش الكهربائي
  if (abs(currentVal - lastPotVals[index]) > 15) 
  {
    lastPotVals[index] = currentVal; // تحديث القراءة المحفوظة
    
    int pulse = map(currentVal, 0, 1023, SERVOMIN, SERVOMAX);
    pwm.setPWM(motorPin, 0, pulse);
    
    // حساب الزاوية التقريبية (من 0 إلى 180) لطباعتها بشكل مفهوم
    int angle = map(currentVal, 0, 1023, 0, 180);
    
    // طباعة الحركة بناءً على رقم الـ index للمقاومة
    switch(index) 
    {
      case 0: Serial.print("Hand (Gripper) -> "); break;
      case 1: Serial.print("Elbow -----------> "); break;
      case 2: Serial.print("Shoulder --------> "); break;
      case 3: Serial.print("Base ------------> "); break;
    }
    Serial.print("Moved to angle: ");
    Serial.println(angle);
  }
}

void loop() 
{
  // قراءة المقاومات وتحديث المحركات الأربعة وطباعة حركاتها
  moveMotorSmooth(potHand, hand, 0);
  moveMotorSmooth(potElbow, elbow, 1);
  moveMotorSmooth(potShoulder, shoulder, 2);
  moveMotorSmooth(potBase, base, 3);

  // التحكم بالمعصم (Wrist) عن طريق الزر مع طباعة الحركة عند التغيير فقط
  int buttonState = digitalRead(buttonPin);
  
  if (buttonState != lastButtonState) 
  {
    if(buttonState == LOW) 
    {
      int pulse = map(180, 0, 180, SERVOMIN, SERVOMAX);
      pwm.setPWM(wrist, 0, pulse);
      Serial.println("Wrist -----------> Moved to angle: 180 (Button Pressed)");
    } 
    else 
    {
      int pulse = map(90, 0, 180, SERVOMIN, SERVOMAX);
      pwm.setPWM(wrist, 0, pulse);
      Serial.println("Wrist -----------> Moved to angle: 90 (Button Released)");
    }
    
    lastButtonState = buttonState; // تحديث حالة الزر المحفوظة
  }

  delay(15); 
} 
