//Nối chân TB6612FNG
#define STBY A0
#define PWMA 9
#define AIN1 10
#define AIN2 11
#define PWMB 5
#define BIN1 13
#define BIN2 A1
//Nối chân TCRT5000
#define S1 2
#define S2 3
#define S3 4
#define S4 6
#define S5 7
//Nối chân HC-SR04
#define trigPin 8
#define echoPin 12

long duration;
int distance;

//Flag tránh vật cản
bool isAvoiding = false;

//PID
float Kp = 75, Ki = 0.1, Kd = 1500;
int base_left = 90;
int base_right = 230;
int error = 0, last_error = 0, I = 0;
int left_speed, right_speed;

void setup() {
  pinMode(STBY, OUTPUT); digitalWrite(STBY, HIGH);

  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT); pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT); pinMode(PWMB, OUTPUT);

  pinMode(S1, INPUT); pinMode(S2, INPUT); pinMode(S3, INPUT);
  pinMode(S4, INPUT); pinMode(S5, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  delay(2000);
}

void loop() {

  readDistance();

  //Khi phát hiện vật cản → bật chế độ né
  if (distance > 0 && distance <= 20 && !isAvoiding) {
    isAvoiding = true;
    avoidObstacle();
    isAvoiding = false;
    return;   //chặn không cho PID hoạt động trong lúc né
  }

  //Nếu đang né thì không dò line
  if (isAvoiding) return;

  readSensors();
  pidControl();
  motorControl();
  delay(10);
}

//Hàm đo khoảng cách HC-SR04 
void readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000);
  distance = duration * 0.034 / 2;
}

//Hàm đọc line
void readSensors() {
  int s1 = digitalRead(S1);
  int s2 = digitalRead(S2);
  int s3 = digitalRead(S3);
  int s4 = digitalRead(S4);
  int s5 = digitalRead(S5);

  if      (s1==1 && s2==1 && s3==1 && s4==1 && s5==0) error =  4;
  else if (s1==1 && s2==1 && s3==1 && s4==0 && s5==0) error =  3;
  else if (s1==1 && s2==1 && s3==1 && s4==0 && s5==1) error =  2;
  else if (s1==1 && s2==1 && s3==0 && s4==0 && s5==1) error =  1;
  else if (s1==1 && s2==1 && s3==0 && s4==1 && s5==1) error =  0;
  else if (s1==1 && s2==0 && s3==0 && s4==1 && s5==1) error = -1;
  else if (s1==1 && s2==0 && s3==1 && s4==1 && s5==1) error = -2;
  else if (s1==0 && s2==0 && s3==1 && s4==1 && s5==1) error = -3;
  else if (s1==0 && s2==1 && s3==1 && s4==1 && s5==1) error = -4;

  //ngoài line → giữ trạng thái cũ
  else error = last_error;
}

//PID CONTROL
void pidControl() {
  int P = error;
  I += error;
  if (I > 50) I = 50; if (I < -50) I = -50;
  int D = error - last_error;
  int pid = Kp * P + Ki * I + Kd * D;
  last_error = error;

  if (error >= 3) {
    left_speed = 0;
    right_speed = base_right * 1.65;
  } else if (error <= -3) {
    left_speed = base_left * 1.65;
    right_speed = 0;
  } else if (error == 2 || error == -2) {
    pid *= 0.9;
    left_speed = base_left - pid;
    right_speed = base_right + pid;
  } else {
    left_speed = base_left - pid;
    right_speed = base_right + pid;
  }
  
  left_speed = constrain(left_speed, 0, 255);
  right_speed = constrain(right_speed, 0, 255);
}

//Motor Control
void motorControl() {
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
  analogWrite(PWMA, right_speed);

  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
  analogWrite(PWMB, left_speed);
}

void stopMotorHard(int t) {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  delay(t);
}

//CHUYỆN ĐỘNG THÔ (cho né vật cản)
void forward(int speedL, int speedR, int t) {
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
  analogWrite(PWMA, speedR);

  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
  analogWrite(PWMB, speedL);

  delay(t);
}

void turnLeft(int speed, int t) {
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, speed);

  digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW);
  analogWrite(PWMB, speed);

  delay(t);
}

void turnRight(int speed, int t) {
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
  analogWrite(PWMA, speed);

  digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW);
  analogWrite(PWMB, speed);

  delay(t);
}

void Lui(int speedL, int speedR, int t) {
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH);
  analogWrite(PWMA, speedR);

  digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH);
  analogWrite(PWMB, speedL);

  delay(t);
}

//AVOID OBSTACLE
void avoidObstacle() {

  stopMotorHard(300); //Dừng xe
  Lui(140, 140, 300); //Lùi xe
  stopMotorHard(300);

  turnLeft(240, 900); //Rẽ trái
  stopMotorHard(500);         
  forward(105, 185, 1300); //Đi thẳng ngắn
  stopMotorHard(500);      

  turnRight(238, 1200); //Rẽ phải     
  stopMotorHard(500);
  forward(106, 185, 2500); //Đi thẳng dài 
  stopMotorHard(500); 

  turnRight(238, 1200); //Rẽ phải
  stopMotorHard(500);   
  forward(105, 185, 1200); //Đi thẳng ngắn
  stopMotorHard(500);     

  turnLeft(240, 900); //Rẽ trái bám lại line 
  stopMotorHard(300);
}
