//Arduino code for Automated Storage and Retrieval System: One Level Shuttle
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <Servo.h> 

//Deklarasi pin Motor Servo
int servo1pin = 2; 
int servo2pin = 6;
Servo Servo1; 
Servo Servo2;

//Deklarasi pin Motor Stepper
const int stepPin = 3; 
const int dirPin1 = 4; 
const int dirPin2 = 5;
const int EN = 12;

//Pengaturan Rotary Encoder
volatile int firstnumber=99;
volatile unsigned int temp, counter = 0, rotation = 0;

//Deklarasi LCD Display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Deklarasi Keypad
const byte numRows= 4; 
const byte numCols= 4; 
char keymap[numRows][numCols]= 
{
{'1', '2', '3', 'A'}, 
{'4', '5', '6', 'B'}, 
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};
byte rowPins[numRows] = {36,38,40,42};
byte colPins[numCols]= {44,46,48,50}; 
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

//Deklarasi karakter dan variable
int i1,i2,i3,i4;
char c1,c2,c3,c4,c5;
char keypressed,keypressedx;
char Str[16];  
int IN_1 = 10;
int IN_2 = 11;
int N_Lemari=0;
int R_Lemari=0;
int activated=0;


void setup() {
  Serial.begin (9600);
  lcd.init();
  lcd.backlight ();

  //Deklarasi pin Rotary Encoder 
  pinMode(19, INPUT_PULLUP); 
  pinMode(18, INPUT_PULLUP);
  
  //Deklarasi void Rotary Encoder
  attachInterrupt(4, ai4, RISING);
  attachInterrupt(5, ai5, RISING);

  //Deklarasi Motor Servo
  Servo1.attach(servo1pin);
  Servo2.attach(servo2pin);

  //Deklarasi Motor Stepper
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin1,OUTPUT);
  pinMode(dirPin2,OUTPUT);
  pinMode(EN,OUTPUT);
  }
  

void loop() { 
  while(keypressed == NO_KEY){ //Pada saat tidak ada tombol yang ditekan (posisi awal)
  keypressed = myKeypad.getKey();
  rotation = 0;
  counter = 0;
  digitalWrite(EN, HIGH);
  Servo1.write(0);
  Servo2.write(180);
  Serial.println("NO_KEY");
  lcd.setCursor(0,0);
  lcd.print("      ASRS"); //Tampilan awal
  lcd.setCursor(0,1);
  lcd.print("OneLevel Shuttle");
  
    if (keypressed == 'A') //Perintah diberikan saat tombol "A" ditekan
    {
      Serial.println("Mulai");
      lcd.clear();
      lcd.print("Input Nmr Lemari");
      delay(1000);
      lcd.clear();
      lcd.print("Nomor Lemari"); //Dua nomor pertama (c1, c2) untuk menentukan lemari, 
      //dua nomor terakhir (c3, c4) untuk menentukan nomor rak pada lemari yang dituju
      
      char keypressed2 = myKeypad.waitForKey();  
        if (keypressed2 != NO_KEY && keypressed2 !='*' && keypressed2 !='#' && keypressed2 !='A' 
        && keypressed2 !='B' && keypressed2 !='C' && keypressed2 !='D' )
        {
          c1 = keypressed2;
          lcd.setCursor(0, 1);
          lcd.print(c1);
          Serial.println(c1);
          }
          
          char keypressed3 = myKeypad.waitForKey();
          if (keypressed3 != NO_KEY && keypressed3 !='*' && keypressed3 !='#' && keypressed3 !='A' 
          && keypressed3 !='B' && keypressed3 !='C' && keypressed3 !='D' )
          {
            c2 = keypressed3;
            lcd.setCursor(1, 1);
            lcd.print(c2);
            Serial.println(c2);
            }
            
            char  keypressed4 = myKeypad.waitForKey();
            if (keypressed4 != NO_KEY && keypressed4 !='*' && keypressed4 !='#' && keypressed4 !='A' 
            && keypressed4 !='B' && keypressed4 !='C' && keypressed4 !='D' )
            {
              c3 = keypressed4;
              lcd.setCursor(2, 1);
              lcd.print(c3);
              Serial.println(c3);
              }
              
              char   keypressed5 = myKeypad.waitForKey();
              if (keypressed5 != NO_KEY && keypressed5 !='*' && keypressed5 !='#' && keypressed5 !='A' 
              && keypressed5 !='B' && keypressed5 !='C' && keypressed5 !='D' )
              {
                c4 = keypressed5;
                lcd.setCursor(3, 1);
                lcd.print(c4);
                Serial.println(c4);
                }
                
                i1=(c1-48)*10; //Dikali 10 agar mempermudah penjumlahan untuk nomor lemari    
                i2=c2-48;
                i3=(c3-48)*10; //Dikali 10 agar mempermudah penjumlahan untuk nomor rak lemari 
                i4=c4-48;
                
                N_Lemari=i1+i2; //Hasil untuk nomor lemari
                R_Lemari=i3+i4; //Hasil untuk nomor rak lemari
                
                delay(1000);
                lcd.clear();
                lcd.print("Berhasil Input");
                delay(1000);
                lcd.clear();
                lcd.setCursor(0,0);
                lcd.print("Penyimpanan (B)"); //Pemilihan penyimpanan atau pengambilan
                lcd.setCursor(0,1);
                lcd.print("Pengambilan (C)");
                activated = 1;
                
                char  keypressed6 = myKeypad.waitForKey(); 
                //Perintah untuk  menentukan penyimpanan atau pengambilan
                if (keypressed6 != NO_KEY && keypressed6 !='*' && keypressed6 !='#' && keypressed6 !='A' 
                && keypressed6 !='D' && keypressed6 !='0' && keypressed6 !='1' && keypressed6 !='2' 
                && keypressed6 !='3' && keypressed6 !='4' && keypressed6 !='5' && keypressed6 !='6' 
                && keypressed6 !='7' && keypressed6 !='8' && keypressed6 !='9'  )
                {
                  c5 = keypressed6;
                  
                  if(c5 == 'B'){ //Jika perintah adalah Penyimpanan
                    delay(1000); 
                    Serial.println(c5);
                    Serial.println("Penyimpanan");

                    if(N_Lemari == 1){
                    digitalWrite(EN, LOW);
                    delay(500);
                    digitalWrite(dirPin1,HIGH);
                    digitalWrite(dirPin2, LOW);
                    for(int x = 0; x < 150; x++){ 
                    //x = 200 artinya 360 derajat (1 putaran), untuk 150 artinya 270 derajat
                    digitalWrite(stepPin,HIGH); 
                    delayMicroseconds(500); 
                    digitalWrite(stepPin,LOW); 
                    delayMicroseconds(500); 
                    }
                    digitalWrite(EN, HIGH);
                    delay(500);
                    Servo1.write(90); 
                    Servo2.write(90);
                    delay(2000); 

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,LOW);
                    digitalWrite(dirPin2, HIGH);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH);
                    delayMicroseconds(500);
                    digitalWrite(stepPin,LOW);
                    delayMicroseconds(500);
                    }
                    digitalWrite(EN, HIGH);

                    while(rotation<R_Lemari){ 
                    //Motor DC akan berputar sampai rak lemari yang dituju dengan Rotary Encoder sebagai sensor
                      if( counter != temp ){
                        Serial.println (counter);
                        temp = counter;
                        }
                        rotation = counter/400;
                        //1 putaran Rotary Encoder 800 pulsa, karena rasio shaft 1:2, 
                        //perhitungan pulsa menjadi 400
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Penyimpanan");
                        lcd.setCursor(0,1);
                        lcd.print(int(rotation));
                        delay(100);
                        digitalWrite(IN_1, HIGH); //Motor DC bergerak
                        digitalWrite(IN_2, LOW);
                        }
                    digitalWrite(IN_1, LOW); //Motor DC berhenti
                    digitalWrite(IN_2, LOW);
                    delay(3000);

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,HIGH);
                    digitalWrite(dirPin2, LOW);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH); 
                    delayMicroseconds(500); 
                    digitalWrite(stepPin,LOW); 
                    delayMicroseconds(500); 
                    }
                    digitalWrite(EN, HIGH);

                    Servo1.write(0); 
                    Servo2.write(180);
                    delay(2000); 

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,LOW);
                    digitalWrite(dirPin2, HIGH);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH);
                    delayMicroseconds(500);
                    digitalWrite(stepPin,LOW);
                    delayMicroseconds(500);
                    }
                    digitalWrite(EN, HIGH);

                    while(rotation>0){ 
                    //Motor DC akan berputar sampai ke posisi awal dengan Rotary Encoder sebagai sensor
                      if( counter != temp ){
                        Serial.println (counter);
                        temp = counter;
                        }
                        rotation = counter/400;
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Kembali ke awal");
                        lcd.setCursor(0,1);
                        lcd.print(int(rotation));
                        delay(100);
                        digitalWrite(IN_1, LOW); //Motor DC bergerak
                        digitalWrite(IN_2, HIGH);
                        }
                    digitalWrite(IN_1, LOW); //Motor DC berhenti
                    digitalWrite(IN_2, LOW);
                    delay(3000);
                    }
                    
                    else{
                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,HIGH);
                    digitalWrite(dirPin2,LOW);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH); 
                    delayMicroseconds(500); 
                    digitalWrite(stepPin,LOW); 
                    delayMicroseconds(500); 
                    }
                    digitalWrite(EN, HIGH);

                    Servo1.write(90); 
                    Servo2.write(90);
                    delay(2000); 

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,LOW);
                    digitalWrite(dirPin2,HIGH);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH);
                    delayMicroseconds(500);
                    digitalWrite(stepPin,LOW);
                    delayMicroseconds(500);
                    }
                    digitalWrite(EN, HIGH);

                    while(rotation<R_Lemari){ 
                    //Motor DC akan berputar sampai rak lemari yang dituju dengan Rotary Encoder sebagai sensor
                      if( counter != temp ){
                        Serial.println (counter);
                        temp = counter;
                        }
                        rotation = counter/400;
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Penyimpanan");
                        lcd.setCursor(0,1);
                        lcd.print(int(rotation));
                        delay(100);
                        digitalWrite(IN_1, HIGH); //Motor DC bergerak
                        digitalWrite(IN_2, LOW);
                        }
                    digitalWrite(IN_1, LOW); //Motor DC berhenti
                    digitalWrite(IN_2, LOW);
                    delay(3000);

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,LOW);
                    digitalWrite(dirPin2, HIGH);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH); 
                    delayMicroseconds(500); 
                    digitalWrite(stepPin,LOW); 
                    delayMicroseconds(500); 
                    }
                    digitalWrite(EN, HIGH);

                    Servo1.write(0); 
                    Servo2.write(180);
                    delay(2000); 

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,HIGH);
                    digitalWrite(dirPin2, LOW);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH);
                    delayMicroseconds(500);
                    digitalWrite(stepPin,LOW);
                    delayMicroseconds(500);
                    }
                    digitalWrite(EN, HIGH);

                    
                    while(rotation>0){ 
                    //Motor DC akan berputar sampai ke posisi awal dengan Rotary Encoder sebagai sensor
                      if( counter != temp ){
                        Serial.println (counter);
                        temp = counter;
                        }
                        rotation = counter/400;
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Kembali ke awal");
                        lcd.setCursor(0,1);
                        lcd.print(int(rotation));
                        delay(100);
                        digitalWrite(IN_1, LOW); //Motor DC bergerak
                        digitalWrite(IN_2, HIGH);
                        }
                    digitalWrite(IN_1, LOW); //Motor DC berhenti
                    digitalWrite(IN_2, LOW);
                    delay(3000);
                    }
                    
                    lcd.clear();
                    lcd.print("Berhasil");
                    delay(3000);
                    lcd.clear();
                    keypressed = NO_KEY;
                    }            
                      
                  if(c5 == 'C'){ //Jika perintah adalah Pengambilan
                    delay(1000); 
                    Serial.println(c5);
                    Serial.println("Pengambilan");
                    
                    if(N_Lemari == 1){
                    while(rotation<R_Lemari){ 
                    //Motor DC akan berputar sampai rak lemari yang dituju dengan Rotary Encoder sebagai sensor
                      if( counter != temp ){
                        Serial.println (counter);
                        temp = counter;
                        }
                        rotation = counter/400;
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Penyimpanan");
                        lcd.setCursor(0,1);
                        lcd.print(int(rotation));
                        delay(100);
                        digitalWrite(IN_1, HIGH); //Motor DC bergerak
                        digitalWrite(IN_2, LOW);
                        }
                    digitalWrite(IN_1, LOW); //Motor DC berhenti
                    digitalWrite(IN_2, LOW);
                    delay(3000);
                    
                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,HIGH);
                    digitalWrite(dirPin2, LOW);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH); 
                    delayMicroseconds(500); 
                    digitalWrite(stepPin,LOW); 
                    delayMicroseconds(500); 
                    }
                    digitalWrite(EN, HIGH);

                    Servo1.write(90); 
                    Servo2.write(90);
                    delay(2000); 

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,LOW);
                    digitalWrite(dirPin2, HIGH);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH);
                    delayMicroseconds(500);
                    digitalWrite(stepPin,LOW);
                    delayMicroseconds(500);
                    }
                    digitalWrite(EN, HIGH);

                    while(rotation>0){ 
                    //Motor DC akan berputar sampai ke posisi awal dengan Rotary Encoder sebagai sensor
                      if( counter != temp ){
                        Serial.println (counter);
                        temp = counter;
                        }
                        rotation = counter/400;
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Kembali ke awal");
                        lcd.setCursor(0,1);
                        lcd.print(int(rotation));
                        delay(100);
                        digitalWrite(IN_1, LOW); //Motor DC bergerak
                        digitalWrite(IN_2, HIGH);
                        }
                    digitalWrite(IN_1, LOW); //Motor DC berhenti
                    digitalWrite(IN_2, LOW);
                    delay(3000);
                    
                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,HIGH);
                    digitalWrite(dirPin2, LOW);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH); 
                    delayMicroseconds(500); 
                    digitalWrite(stepPin,LOW); 
                    delayMicroseconds(500); 
                    }
                    digitalWrite(EN, HIGH);

                    Servo1.write(0); 
                    Servo2.write(180);
                    delay(2000); 

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,LOW);
                    digitalWrite(dirPin2, HIGH);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH);
                    delayMicroseconds(500);
                    digitalWrite(stepPin,LOW);
                    delayMicroseconds(500);
                    }
                    digitalWrite(EN, HIGH);
                    }
                    
                    else{
                    while(rotation<R_Lemari){ 
                    //Motor DC akan berputar sampai rak lemari yang dituju dengan Rotary Encoder sebagai sensor
                      if( counter != temp ){
                        Serial.println (counter);
                        temp = counter;
                        }
                        rotation = counter/400;
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Pengambilan");
                        lcd.setCursor(0,1);
                        lcd.print(int(rotation));
                        delay(100);
                        digitalWrite(IN_1, HIGH); //Motor DC bergerak
                        digitalWrite(IN_2, LOW);
                        }
                    digitalWrite(IN_1, LOW); //Motor DC berhenti
                    digitalWrite(IN_2, LOW);
                    delay(3000);
                    
                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,LOW);
                    digitalWrite(dirPin2,HIGH);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH); 
                    delayMicroseconds(500); 
                    digitalWrite(stepPin,LOW); 
                    delayMicroseconds(500); 
                    }
                    digitalWrite(EN, HIGH);

                    Servo1.write(90);
                    Servo2.write(90); 
                    delay(2000); 

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,HIGH);
                    digitalWrite(dirPin2,LOW);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH);
                    delayMicroseconds(500);
                    digitalWrite(stepPin,LOW);
                    delayMicroseconds(500);
                    }
                    digitalWrite(EN, HIGH);

                    while(rotation>0){ 
                    //Motor DC akan berputar sampai ke posisi awal dengan Rotary Encoder sebagai sensor
                      if( counter != temp ){
                        Serial.println (counter);
                        temp = counter;
                        }
                        rotation = counter/400;
                        lcd.clear();
                        lcd.setCursor(0,0);
                        lcd.print("Kembali ke awal");
                        lcd.setCursor(0,1);
                        lcd.print(int(rotation));
                        delay(100);
                        digitalWrite(IN_1, LOW); //Motor DC bergerak
                        digitalWrite(IN_2, HIGH);
                        }
                    digitalWrite(IN_1, LOW); //Motor DC berhenti
                    digitalWrite(IN_2, LOW);
                    delay(3000);

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,HIGH);
                    digitalWrite(dirPin2, LOW);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH); 
                    delayMicroseconds(500); 
                    digitalWrite(stepPin,LOW); 
                    delayMicroseconds(500); 
                    }
                    digitalWrite(EN, HIGH);

                    Servo1.write(0); 
                    Servo2.write(180);
                    delay(2000); 

                    digitalWrite(EN, LOW);
                    digitalWrite(dirPin1,LOW);
                    digitalWrite(dirPin2, HIGH);
                    for(int x = 0; x < 150; x++){
                    digitalWrite(stepPin,HIGH);
                    delayMicroseconds(500);
                    digitalWrite(stepPin,LOW);
                    delayMicroseconds(500);
                    }
                    digitalWrite(EN, HIGH);
                    }
                    
                    lcd.clear();
                    lcd.print("Berhasil");
                    delay(3000);
                    lcd.clear();
                    keypressed = NO_KEY;
                    }
               }
               else{
               lcd.clear();
               lcd.print("ERROR");
               delay(2000);
               lcd.clear();
               keypressed = NO_KEY;
               }
    }           
}     
}
              
  
  void ai4() { //Pemrograman void untuk Rotary Encoder
  if(digitalRead(18)==LOW) {
  counter++;
  }else{
  counter--;
  }
  }
   
  void ai5() {
  if(digitalRead(19)==LOW) {
  counter--;
  }else{
  counter++;
  }
  }
