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
    Название набора: РобоРовер М1 4WD Intel Edison
    Программа создана и протестирована разработчиком:
    Имя: Максим Массальский
    Ник: maxxlife
    E-mail: maxxliferobot@gmail.com
    Дата разработки: 30.08.2015
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

int enA = 5;
int in1 = 2;
int in2 = 8;
// motor two
int enB = 3;
int in3 = 11;
int in4 = 10;

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

void IrfrontGoBack()
{

  if (lfront == rfront)
  {
    radar.write(radarright);
    delay(200);
    USMeasure();
    USrangeright = USrange;
    radar.write(radarleft);
    delay(200);
    USMeasure();
    USrangeleft = USrange;
    if (USrangeleft < USrangeright)
    {
      go_right_full();
      delay(750);
      go_stop();
      radar.write(radarleft);
      while ((rfront < frontlow + 20) && (lfront < frontlow + 20))
      {
        IRMeasure();
        go_backward();
      }
      USMeasure();
      while ( USrange < 40)
      {
        USMeasure();
        go_backward();
      }
      go_right_full();
      delay(500);
      go_stop();
    }

    if (USrangeright < USrangeleft)
    {
      go_left_full();
      delay(750);
      go_stop();
      radar.write(radarright);
      while ((rfront < frontlow + 30) && (lfront < frontlow + 30))
      {
        IRMeasure();
        go_backward();
      }
      USMeasure();
      while (USrange < 40)
      {
        USMeasure();
        go_backward();
      }
      go_left_full();
      delay(500);
      go_stop();
    }

    if (USrangeright == USrangeleft)
    {
      radar.write(radarfront);
      while ((rfront < frontlow + 40) && (lfront < frontlow + 40))
      {
        IRMeasure();
        go_backward();
      }
      USMeasure();
      while ((USrange < 40))
      {
        USMeasure();
        go_backward();
      }
    }
    radar.write(radarfront);
  }

  if (rfront < lfront)
  {
    go_left_full();
    delay(750);
    go_stop();
    while ((rfront < frontlow + 40) && (lfront < frontlow + 40))
    {
      IRMeasure();
      go_backward();
    }
    USMeasure();
    while ((USrange < 40))
    {
      USMeasure();
      go_backward();
    }
    go_left_full();
    delay(500);
    go_stop();
  }
  if (lfront < rfront)
  {
    go_right_full();
    delay(750);
    go_stop();
    while ((rfront < frontlow + 40) && (lfront < frontlow + 40))
    {
      go_backward();
      IRMeasure();
    }
    USMeasure();
    while ((USrange < 40))
    {
      USMeasure();
      go_backward();
    }
    go_right_full();
    delay(500);
    go_stop();
  }
}

void USRadar()
{
  radar.write(radarright);
  delay(250);
  USMeasure();
  USrangeright = USrange;
  radar.write(radarleft);
  delay(250);
  USMeasure();
  USrangeleft = USrange;
}


//A-левый
//B-правый
void go_forward()
{
  // turn on motor A Левый
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, 250);

  // turn on motor B Правый
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, 250);
}

//Функция, которая поворачивает влево
//Правые колеса едут на полном газу вперед
//Левые колеса едут на полном газу назад
void go_left_full()
{
  // turn on motor A
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, 250);

  // turn on motor B
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, 250);
}

void go_left_half()// Легкий поворот влево
{
  // turn on motor A Левый
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, 120);

  // turn on motor B Правый
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, 255);
}

//Функция, которая поворачивает вправо
//Левые колеса едут на полном газу вперед
//Правые колеса едут на полном газу назад
void go_right_full()
{
  // turn on motor A
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, 250);

  // turn on motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, 250);
}

void go_right_half()
{
  // turn on motor A
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, 255);

  // turn on motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, 120);
}
void go_backward()
{
  // turn on motor A Левый
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, 200);

  // turn on motor B Правый
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, 200);//stable
}
void go_stop()
{
  // turn on motor A Левый
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, 200);

  // turn on motor B Правый
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, 200);
}

void setup() {
  Serial.begin(9600);

  radar.write(radarfront);
  delay(5);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  delay(5);
  pinMode(enA, OUTPUT);

  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  //pinMode(13,OUTPUT);
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




