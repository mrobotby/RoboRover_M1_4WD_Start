/*
    http://www.mrobot.by
    http://www.maxxlife.by
    http://www.vk.com/cookrobot
    Copyright (c) Кухня роботов <maxxlife.by> All rights reserved.
    Copyright (c) Макслайф Робот Maxxlife Robot All rights reserved.
    Наши лаборатории по робототехнике:
    Ленинская РОС ДОСААФ, ул. Рокоссовского 63/2, Минск, Беларусь
    Подробнее в нашей группе Вконтакте http://www.vk.com/cookrobot
    И на сайте http://www.maxxlife.by
    ****************************************************
    Мы всегда рады новым членам нашего сообщества Кухня Роботов.
    У нас есть бесплатные вводные курсы, где мы объясняем
    как работать с нашими образовательными наборами по робототехнике и электронике.
    ****************************************************
    Название набора: РобоРовер М1
    Программа создана и протестирована разработчиком:
    Имя: Максим Массальский
    Ник: maxxlife
    E-mail: maxxliferobot@gmail.com
*/

class Servo // имя класса
{
  public:
    int write(int data)
    {
      analogWrite(9, data);
    }
};
Servo radar;

int trigPin = 7;
int echoPin = 4;

double voltconst = 0.0048;

//Начало секции определения
volatile int rfront = 0;
volatile int lfront = 0;

// Серва Радар Pan
volatile int posr;
int radarfront = 185;//80
int radarleft = 120; //44
int radarright = 250; //


// Ультразвуковой сенсор
volatile long USrange = 0;
int UShigh = 100;
int USlow = 40;
volatile long USrangeleft = 0;
volatile long USrangeright = 0;

// Датчики Инфракрасные Передние

//double anglefront=cos((30*PI)/180);// Угол 30 градусов
float anglefront = 1;
int irpinfrontleft = 0; // Потом поставить 3
int irpinfrontright = 1; // Потом поставить 4

//Переменные по настройке алгоритма Передних IR под условия и расстояния
int fronthigh = 65; // Для матусевича 75// Для гамарника 65
int frontlow = 20;

class IR_Sharp_Measure // имя класса для измерения расстояния с ИК-датчиков робота
{
  public:
    int irmeasure(int irpin)
    {
      int distance;
      double volts = analogRead(irpin) * voltconst; // Измеряем значение с датчика в милливольтах. Порт 0
      // 02 01 14 расширили область измерений датчиком с 70 до 110 сантиметров
      //Serial.println(volts);
      double val[46] = {
        2.53, 2.43, 2.31, 2.17, 2.05, 1.95, 1.84, 1.73, 1.66, 1.57, 1.47, 1.42, 1.34, 1.29, 1.23, 1.18, 1.14, 1.10, 1.05, 1.01, 0.99, 0.96, 0.93, 0.90, 0.87, 0.86, 0.84, 0.80, 0.78, 0.76, 0.74, 0.73, 0.71, 0.70, 0.68, 0.67, 0.65, 0.63, 0.62, 0.61, 0.59, 0.57, 0.56, 0.55, 0.53, 0.52
      };
      int cm[46] = {
        20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110
      };
      //            {17,19,20,     ,26,           ,34,           ,44,           ,52,           ,61,           ,70,           ,78,           ,87,                ,95,}// Это пересчитано при косинусе 30 градусов ДИАПАЗОН==78
      for (int i = 0; i < 46; i++)
      {
        if (val[i] <= volts)
        {
          distance = cm[i] * anglefront;
          break;
        }
        if (volts < 0.52)
        {
          distance = 120 * anglefront;
        }
      }
      if (volts > 2.53)
      {
        distance = 10 * anglefront; //==9
      }
      return distance;
    }
};
IR_Sharp_Measure IR;
void IRMeasure()
{
  rfront = IR.irmeasure(irpinfrontright);
  lfront = IR.irmeasure(irpinfrontleft);
}

//----------------------------------------------------Ultrasonic Start-----------------------------------------------------//

void USMeasure () {

  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  USrange = (duration / 2) / 29.1;
  delay(50);
}
//----------------------------------------------------Ultrasonic End-----------------------------------------------------//



void setup() {
  Serial.begin(9600);
  radar.write(radarfront);
  delay(5);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  delay(1000);
}

void loop() {
  USMeasure();
  Serial.print("Front Ultrasonic (US) sensor, distance ");
  Serial.print(USrange);
  Serial.println(" cm");

  IRMeasure();
  Serial.print("Left Infared (IK) sensor, distance ");
  Serial.print(lfront);
  Serial.println(" cm");

  Serial.print("Right Infared (IK) sensor, distance ");
  Serial.print(rfront);
  Serial.println(" cm");

  delay(500);

}




