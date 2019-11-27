//projektityö Sulautettu
//Ryhmä 4, TVT18SPO
//Janne, Heikkinen, Juha Lallukka & Miika Röyttä




#include <math.h>
#include "kosteustaulukko.h"
#include "mbed.h"
//#include "L3G4200D.h"

Timer timer;
Serial pc(USBTX,USBRX);
//L3G4200D gyro(D14, D15);
//int g[3]={0, 0, 0};
//float kulma[3] = {0.0, 0.0, 0.0};

AnalogIn AnalogToDigitalTemp(A0); //
AnalogIn AnalogToDigitalMoist(A1);
DigitalOut PowerToMoisture(D2);

float Uin; //käyttöjännite

//*******************LÄMPÖTILA MUUTTUJAT**********************************
float rawDataTemp; // 3.3V = 1.0 raakadatana, viidellä voltilla tarkempi mittaustulos, mutta yli 3.3V:n arvot virheellisiä
float temperature; // celsius-asteina

float UoutTemp; //mitattu jännite, lämpötila

int resistor1; //sarjavastus ohmeina, kokoluokkaa 1 kOhm
float resistorTemp; //lämpötila-anturin resistanssi ohmeina







//float kerroin; //vaihtelee 118.05 & 97.254 välillä (0 & 50 *C)
//float potenssi; // vaihtelee -0.143 & -0.185 välillä (0 & 50 *C)

//void KosteusMittaus(); //kaavamuotoa: kosteus = kerroin * Rk ^ potenssi



//****************KOSTEUS MUUTTUJAT***********************
float moisture; //kosteus mitä lasketaan
float rawDataMoist;
float UoutMoist; //mitattu jännite

float resistor2; //sarjavastus ohmeina, kokoluokkaa 50kOhm
float resistorMoist; //kosteusanturin resistanssi ohmeina


float virtualArrayForMoisture[15]; //lasketaan anturin datasheetissa olevan taulukon pohjalta

int columHigher; //sarakkeet
int columLower;

int rowHigher; //rivit
int rowLower;



float lowerResistance;
float higherResistance;

int lowerTemp;
int higherTemp;

int lowerMoist;
int higherMoist;

float weightTempLower; //painotuskertoimet
float weightTempHigher;

float weightMoistureLower;

float virtualRowNumber;
float virtualWeightResistanceLow;
float virtualWeightResistanceHigh;





//******************KOSTEUS FUNKTIOT************************
float CalculateWeight(float value, float lowerValue, float higherValue); //laskee painotuskertoimen lowerValue muuttujalle
void CheckColumNumber();

void CalculateResistanceMoist();
void CalculateVirtualArrayForMoisture();
void CalculateVirtualRowNumber();
void CalculateMoisture();

void MeasureMoisture(); //pääfunktio joka nostaa pinnin HIGH tilaan ja ajaa muut tarvittavat funktiot








int main() 
{
    rawDataTemp = 0;
    rawDataMoist = 0;

    Uin = 3.3; //käyttöjännite
    resistor1 = 1000; //lämpötilan sarjavastus ohmeina
    resistor2 = 47000; //kosteuden sarjavastus ohmeina

    PowerToMoisture = 0;



//timer.start();
//timer.reset();


	while(true)
	{
                //rawDataMoist = AnalogToDigitalMoist;

               rawDataTemp = AnalogToDigitalTemp;
               UoutTemp = Uin - rawDataTemp*Uin; //muutetaan volteiksi. pelkkä lampotilaRaaka*Uin, jos sarvavastus on plusnavan puolella ja lämpötilasensori miinus puolella.

               resistorTemp = resistor1/(Uin/UoutTemp -1);

               temperature = 0.1291 * resistorTemp -107.99; // tämä vaikutti tarkimmalta pikavertailulla kahden muun mittarin kanssa
               //temperature = -4*(10^(-5))* resistorTemp^2 + 0.2125 * resistorTemp - 146.78;
                //temperature = (-0.00004) * (resistorTemp * resistorTemp) + (0.2125 * resistorTemp) - 146.78;


               MeasureMoisture();

               //Tulosteet
               //pc.printf("%f ", rawDataTemp);

               //pc.printf("%f ", UoutTemp);

               pc.printf("temperature sensor resistance %f ", resistorTemp);
               pc.printf("\n");


               pc.printf("temperature: %f ", temperature);
               pc.printf("\n");

               pc.printf("moisture: %f ", moisture);
               pc.printf("\n");

               //rivin vaihto
               pc.printf("\r\n");

              // wait(0.02);  //Ei tarvi odottaa jos pikavauhtia etsitään


            //temperature = 21;
            //resistorMoist = 10;

            //MeasureMoisture();

            wait(10);
	}

}

//******************KOSTEUS FUNKTIOT************************************
void MeasureMoisture()
{
    PowerToMoisture = 1;
    wait(0.01);

    rawDataMoist = AnalogToDigitalMoist;

    PowerToMoisture = 0;


    CalculateResistanceMoist();
    CalculateVirtualArrayForMoisture();
    CalculateVirtualRowNumber();
    CalculateMoisture();


}





//laskee painotuskertoimen alemmalle arvolle
float CalculateWeight(float value, float lowerValue, float higherValue)
{
    float weight;
    float difference = higherValue - lowerValue;

    //pc.printf("difference: %f", difference);
    //pc.printf("\n");


    weight = (difference - (value - lowerValue))/difference;

    //weight = sqrt(weight * weight); //otetaan itsesarvo

    //pc.printf("weight: %f", weight);
    //pc.printf("\n");

    return weight;
}

//laskee kosteusanturin resistanssin raakadatasta
void CalculateResistanceMoist()
{

    UoutMoist =rawDataMoist*Uin; //muutetaan volteiksi. pelkkä raakaData*Uin, jos sarjavastus on plusnavan puolella ja lämpötilasensori miinus puolella.

    resistorMoist = resistor2/(Uin/UoutMoist -1);

    pc.printf("moisture sensor resistance: %.2f ohm", resistorMoist);
    pc.printf("\n");

    //muutetaan kilo ohmeiksi taulukkoa varten
    resistorMoist = resistorMoist/1000;

    pc.printf("moisture sensor resistance: %.2f kOhm", resistorMoist);
    pc.printf("\n");


}






//etsii oikeat sarakkeet lämpötilan perusteella
void CheckColumNumber()
{
    higherTemp = 0;
    columHigher = 0;
    //if(temperature < 0)
    while(temperature > higherTemp)
    {
        columHigher++;

        higherTemp = higherTemp + 5;

    }
    columLower = columHigher -1;
    lowerTemp =  higherTemp - 5;
}


//etsii oikeat rivit resistanssin perusteella
void CheckRowNumber()
{
    rowLower = 14;
    while (resistorMoist > moistureArray[rowLower][columHigher] )
    {
        rowLower--;
    }

    rowHigher = rowLower +1;

   higherResistance  = moistureArray[rowLower][columHigher];
   lowerResistance  = moistureArray[rowHigher][columHigher];

}


//laskee vieruaalisen taulukon resistansseille mitatussa lämpötilassa kahden lämpötilan pohjalta
void CalculateVirtualArrayForMoisture()
{
    CheckColumNumber();
    weightTempLower = CalculateWeight(temperature, lowerTemp, higherTemp);
    weightTempHigher = 1 - weightTempLower;

    for(int i = 0; i < 15; i++)
    {
        virtualArrayForMoisture[i] = moistureArray[i][columLower] * weightTempLower + moistureArray[i][columHigher] * weightTempHigher;

        //pc.printf("virtualArray %d = %.2f", i, virtualArrayForMoisture[i]);
        //pc.printf("\n");

    }
}



void CalculateVirtualRowNumber()
{
    rowLower = 14;
    while (resistorMoist > virtualArrayForMoisture[rowLower]  ) //moistureArray[rowLower][columHigher]
    {
        rowLower--;
    }

    rowHigher = rowLower +1;

   higherResistance  = virtualArrayForMoisture[rowHigher];
   lowerResistance  = virtualArrayForMoisture[rowLower];

      //pc.printf("ResHi: %.2f", higherResistance);
      //pc.printf("\n");
      //pc.printf("ResLo: %.2f", lowerResistance);
      //pc.printf("\n");


   virtualWeightResistanceLow = CalculateWeight(resistorMoist, lowerResistance, higherResistance);

       //pc.printf("virtualWeightLo: %.2f", virtualWeightResistanceLow);
       //pc.printf("\n");

   virtualWeightResistanceHigh = 1 - virtualWeightResistanceLow;

       //pc.printf("virtualWeightHi: %.2f", virtualWeightResistanceHigh);
       //pc.printf("\n");

    virtualRowNumber =  rowLower * virtualWeightResistanceLow + rowHigher * virtualWeightResistanceHigh;

      //pc.printf("virtual row number: %.2f", virtualRowNumber);
      //pc.printf("\n");
      //pc.printf("\n");

}



//laskee kosteuden rivinumeron perusteella, pitää vielä painottaa
void CalculateMoisture()
{
    //moisture = (rowLower*5 + 20)* weightTempLower + (rowHigher*5 + 20)* (1- weightTempLower) ;

   moisture = virtualRowNumber * 5 + 20;

   //pc.printf("moisture: %.2f", moisture);
   //pc.printf("\n");
   //pc.printf("\n");
   //pc.printf("\n");
   //pc.printf("\n");


}

