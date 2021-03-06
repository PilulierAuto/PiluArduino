/*************************
 * Code.ino
 * Fichier principal
 * 
 * Création : 27/12/17
 * Dernière modification : 21/5/18
 */

#include <EEPROM.h>
#include <DS1302RTC.h>
#include <Time.h>
#include "def.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "rgb_lcd.h"

#define PinA 2    //  1ere sortie du codeur
#define PinB 3    //  2e sortie du codeur

volatile boolean mouvement;
volatile boolean up;
volatile boolean serial;

/* Moteurs (l298n) M1:Rotation plateau M2:Montée médicaments*/
const int M1HorPin = 7;   //IN1
const int M1TrigPin = 8;  //IN2
const int M1Pwm = 5;      //ENA

const int M2HorPin = 9;   //IN3
const int M2TrigPin = 10; //IN4
const int M2Pwm = 6;      //ENB

/*Diodes IR*/
const int IREmmPin = 13, IRRecPin = A0;

/*Capteur contact*/
const int CptCtcPin = A2; 
const int CptFinCoursePin = A3;

/*Boutons*/
const int BtnPin = A1; 
/*DS1302 RTC*/
const int CEPin = 4, IOPin = 12, SLCKPin = 11;
DS1302RTC RTC(CEPin, IOPin, SLCKPin);
tmElements_t tm;

int PosPlat = 0; //position du plateau (0->4)

struct Med Med[5];
//const int EEPromOct = 42; //vàd
int heure = 0;
int Id = 0;
//short NbMedAct=0, NbMedVol=0;
unsigned short MedSeq[5];
/*écran */
int Pos = 0;


rgb_lcd Lcd;

void RotPlat(); //rotation du plateau
void AddMed(int nbr); //chute des médicaments
void RempTab();
void ActuRTC(); //syncronisation temps avec bluetooth
void routineInterruption();
void ActuPos();
int AqPers();
boolean Fin(); //vérifie si les médicaments ont été distribués

void setup() {
  Serial.begin(9600);
  pinMode(IREmmPin, OUTPUT);
  Lcd.begin(16,2);
  digitalWrite (PinA, HIGH);
  digitalWrite (PinB, HIGH);
  attachInterrupt (0, routineInterruption, FALLING);

  pinMode(M1TrigPin, OUTPUT);
  pinMode(M1HorPin, OUTPUT);
  pinMode(M1Pwm, OUTPUT);
  pinMode(M2TrigPin, OUTPUT);
  pinMode(M2HorPin, OUTPUT);
  pinMode(M2Pwm, OUTPUT);

  pinMode(CptCtcPin, INPUT);
  pinMode(CptFinCoursePin, INPUT);
  
   //------------------------------*MENU*----------------------------
  Lcd.createChar(0, Cust1);
  Lcd.createChar(1, Cust2);
  Lcd.createChar(2, Cust3);
  Lcd.createChar(3, Cust4);
  Lcd.createChar(4, Cust5);
}

void loop() {
//  while(analogRead(BtnPin)!=0){ //ou bluetooth
//    delay(1000);
//  }
  Id = AqPers();
  EEPROM.get(0, Med);
  RempTab();
  boolean fin=0;
  while(fin==0){
   fin=Fin();
   AddMed(MedSeq[PosPlat]);
   RotPlat();        
  }

}

void SerialEvent(){
  if(serial){
   byte ReadVar = Serial.read();
   switch(ReadVar){
    case Mes_Actu:
      ActuRTC();
      break;
    case Mes_DemPos:
      ActuPos();
      break;
   }
  }   
}















