#include <SPI.h>
#include <RF24.h>

// pins radio
int pinCSN = 10;
int pinCE = 9;

//pins moteurs
int pinPotServo = A1;
int pinPotMoteur = A5;

// creation de l(objet radio
RF24 radio(pinCE, pinCSN);

// adresse de communication
const byte adresse[6] = "00001";

void setup()
{
  Serial.begin(9600);
  // demarrage de la radio
  radio.begin();
  // cette radio va envoyer les infos 
  radio.openWritingPipe(adresse);
  // puissance emettrice
  radio.setPALevel(RF24_PA_LOW);
  // pret a l'emploi
  radio.stopListening();
}

void loop()
{
  // recuperation des valeurs des joysticks
  int valPotServo = analogRead(pinPotServo);
  int valPotMoteur = analogRead(pinPotMoteur);
  // conversion en donnees utilisables
  int angle = my_map(valPotServo);
  int valMot = map(valPotMoteur, 0, 1023, 50, 0);
  if (valMot <= 30){
    valMot = 0;
  }
  else{
    valMot = valMot - 25;
  }
  // creation du message
  double message = encode(angle, valMot);
  // envoie du message
  radio.write(&message, sizeof(message));
  Serial.println(message);
  delay(10);  // 5ms
  
}

// mes fonctions
double encode(int angle, double valMot)
{
  return (angle + (valMot/100));
}

int my_map(int val)
{
  val = val / 205;
  val = val + 1;
  val = 6 - round(val);
  return val;
}
