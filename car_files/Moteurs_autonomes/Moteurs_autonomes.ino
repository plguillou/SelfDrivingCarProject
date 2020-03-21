#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

int autonome = 1;

int old_speed = 0;
int new_speed = 0;
int old_dir = 90;
int new_dir = 90;

// pins de commande radio
int pinCSN = 10;
int pinCE = 9;

int pinServo = 8;

// pins de commande du moteur CC
int pinMoteur = 5;
int in3 = 3;
int in4 = 4;

// initialisation des variables lors de la conduite manuelle
int angleServo = 0;
int valMoteur = 0;

// initialisation des variables lors de la conduite autonome
char centre = '1';
char droite = '2';
char gauche = '0';
char arret = '3';

// creation de l'objet Radio et de l'objet Servo
RF24 radio(pinCE, pinCSN);
Servo servo;

// adresse de communication des deux radios
const byte adresse[6] = "00001";

void setup()
{
  // initialisation de la communication arduino raspberry
  Serial.begin(9600);
  // initialisation des pins
  pinMode(pinMoteur, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  // la voiture ne pourra pas reculer (qu'avancer)
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // mise a 0 du moteur cc
  analogWrite(pinMoteur, 0);
  // mise a 0 du servo moteur
  servo.attach(pinServo);
  servo.write(90);
  // demarrage de la radio
  radio.begin();
  // cette radio va lire les infos 
  radio.openReadingPipe(0, adresse);
  // puissance emettrice
  radio.setPALevel(RF24_PA_LOW);
  // pret a l'emploi
  radio.startListening();
}

void loop()
{
  // conduite manuelle
  if (autonome == 0)
  {
    delay(5);  // 5ms
    if (radio.available()) // attendre qu'un message soit envoye
    {
      // recuperation des donnees radio
      double val_servo_dc;
      radio.read(&val_servo_dc, sizeof(val_servo_dc));
      if (val_servo_dc == 10000) // stop
      {
        Serial.println("stop");
      }
      else
      {
        // mise a jour des moteurs
        angleServo = decode_val_servo(val_servo_dc);
        valMoteur = decode_val_dc(val_servo_dc);
        int data_to_python = val_servo_dc*100;
        Serial.println(data_to_python);
        servo.write(angleServo);
        analogWrite(pinMoteur, valMoteur);
        delay(10);
      }
    }
  }
  // conduite autonome
  else if (autonome == 1)
  {
    if (Serial.available() > 0) // attendre qu'un message soit envoye
    {
      // lecture du message
      char rasp_info = Serial.read();
      delay(5);
      // on le renvoie pour que raspberry continue son execution
      // cette etape est comme une synchro entre arduino et raspberry
      Serial.println(rasp_info);
      // mise a jour des moteurs
      new_dir = pred_to_angle(rasp_info, gauche, droite, centre, arret);
      new_speed = pred_to_vit(rasp_info, gauche, droite, centre, arret);
      servo.write(new_dir);
      analogWrite(pinMoteur, new_speed);
      delay(10);
    }
  }
}

// mes fonctions
int decode_val_servo(double v_radio)
{
  if (v_radio == round(v_radio))
  {
    return v_radio*30;
  }
  else
  {
    int val_servo = round(v_radio);
    val_servo = 30*val_servo;
    return val_servo;
  }
}

int decode_val_dc(double v_radio)
{
  if (v_radio == round(v_radio))
  {
    return 0;
  }
  else
  {
    int val_dc = (v_radio - round(v_radio - 0.5))*1000;
    return val_dc;
  }
}

int pred_to_vit(char x, char gauche, char droite, char centre, char arret)
{
  if (x == arret)
  {
    return 0;
  }
  else if (x == centre)
  {
    return 250;
  }
  else if (x == gauche)
  {
    return 190;
  }
  else
  {
    return 190;
  }
}

int pred_to_angle(char x, char gauche, char droite, char centre, char arret)
{
  if (x == arret)
  {
    return 90;
  }
  else if (x == centre)
  {
    return 90;
  }
  else if (x == gauche)
  {
    return 150;
  }
  else
  {
    return 30;
  }
}
