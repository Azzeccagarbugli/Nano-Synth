/* 
 _   _                     ____              _   _     
| \ | | __ _ _ __   ___   / ___| _   _ _ __ | |_| |__  
|  \| |/ _` | '_ \ / _ \  \___ \| | | | '_ \| __| '_ \ 
| |\  | (_| | | | | (_) |  ___) | |_| | | | | |_| | | |
|_| \_|\__,_|_| |_|\___/  |____/ \__, |_| |_|\__|_| |_|
                                 |___/                 

Sintetizzatore anologico nano powered by Arduino

Progetto scolastico a cura di:    Francesco Coppola
                                  Matteo Mancini
                                  Enrico Tozzi
                                  Samuele Fugnanesi
                                  Alessandro Raggi
                                  Mosè Zamparini
*/

#include <MozziGuts.h>
#include <Oscil.h> // Oscillatore
#include <tables/cos2048_int8.h> // Tavole armoniche per l'oscillatore
#include <AutoMap.h> //AutoMap
#include <EventDelay.h>

// Dichiaro valore max e min per l'AutoMap, for AutoMap
const int MIN_CARRIER_FREQ = 22;
const int MAX_CARRIER_FREQ = 440;

// Faccio la stessa cosa, in questo caso per la fotoresitenza, valore max and min
const int MIN_INTENSITY = 700;
const int MAX_INTENSITY = 10;

AutoMap kMapCarrierFreq(0,1023,MIN_CARRIER_FREQ,MAX_CARRIER_FREQ);
AutoMap kMapIntensity(0,1023,MIN_INTENSITY,MAX_INTENSITY);
AutoMap KPot_Harmonic(1,1023,1,6);

const int KNOB_PIN = 0; //Modifico ampiezza dell'onda, Potenziometro A0
const int LDR_PIN = 1; //Vario la frequenza dell'onda, Fotoresitenza A1
const int HARMONIC = 2; //Cambio il numero di armoniche, Potenziometro A2

Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCarrier(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aModulator(COS2048_DATA);

int mod_ratio; // Armoniche
long fm_intensity; // Valore che modifico nell'UpdateAudio();


void setup(){
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  Serial.begin(115200);
  startMozzi(); // :))
}


void updateControl()
{
  
  int Pot_Harmonic = mozziAnalogRead(HARMONIC); //Leggo il valore del secondo pot
  
  int ARMONICHE = KPot_Harmonic(Pot_Harmonic);  //Richiamo la mappatura per un effettiva conversione del valore
  
  mod_ratio = ARMONICHE; //Il valore del potenziomentro (0,6) sarà il valore dell'ottava
  
  Serial.print("ARMONICA = ");
  Serial.print(mod_ratio);
  Serial.print("\t");
  
  
  int knob_value = mozziAnalogRead(KNOB_PIN); // Valore compreso 0-1023 // Leggo il Pot A0

  int carrier_freq = kMapCarrierFreq(knob_value);   // Mappo il Pot A0
  
  int mod_freq = carrier_freq * mod_ratio;  //Modifico i vari parametri nella Modulation

  aCarrier.setFreq(carrier_freq); 
  aModulator.setFreq(mod_freq);
  
  //Leggo il valore della fotoresitenza
  int light_level= mozziAnalogRead(LDR_PIN); // Valore compreso 0-1023
  
  if(light_level<50)
  {
    digitalWrite(7, HIGH);
    EventDelay(100);
    digitalWrite(7, LOW);
    EventDelay(100);
   }
   else
   {
     digitalWrite(7,HIGH);
    }
 
  Serial.print("light_level = "); 
  Serial.print(light_level); 
  Serial.print("\t"); 
  
  fm_intensity = kMapIntensity(light_level);
  
  if(fm_intensity<=100)
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
  }
  else if(fm_intensity>100 && fm_intensity<300)
  {
    digitalWrite(3, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(4, LOW);
  }
  else if(fm_intensity>=300 && fm_intensity<600)
  {
   digitalWrite(4, HIGH);
   digitalWrite(2, HIGH);
   digitalWrite(3, HIGH);
  }
  else if(fm_intensity>600)
  {
    digitalWrite(2, HIGH);
    EventDelay(100);
    digitalWrite(2, LOW);
    EventDelay(100);
    digitalWrite(3, HIGH);
    EventDelay(100);
    digitalWrite(3, LOW);
    EventDelay(100);
    digitalWrite(4, HIGH);
    EventDelay(100);
    digitalWrite(4, LOW);
    EventDelay(100);
  }
  else
  {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
  }
  
  Serial.print("fm_intensity = ");
  Serial.print(fm_intensity);
  Serial.println(); 

}


int updateAudio(){
  long modulation = fm_intensity * aModulator.next(); 
  return aCarrier.phMod(modulation); 
}


void loop(){
  
  audioHook();
}

