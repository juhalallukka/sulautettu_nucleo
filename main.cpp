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

double Uin; //käyttöjännite


//*******************LÄMPÖTILA MUUTTUJAT**********************************
long double rawDataTemp; // 3.3V = 1.0 raakadatana, viidellä voltilla tarkempi mittaustulos, mutta yli 3.3V:n arvot virheellisiä
double temperature; // celsius-asteina

double UoutTemp; //mitattu jännite, lämpötila

int resistor1; //sarjavastus ohmeina, kokoluokkaa 1 kOhm
double resistorTemp; //lämpötila-anturin resistanssi ohmeina







//****************KOSTEUS MUUTTUJAT***********************
double moistureAvg1; // painotuksella
double moistureAvg2; //minuutin keskiarvo
double moistureAvg2Sum;
double moistureAvg2Array[18];
int avgCounter;

double moisture; //kosteus mitä lasketaan
long double rawDataMoist;
long double UoutMoist; //mitattu jännite

double resistor2; //sarjavastus ohmeina, kokoluokkaa 50kOhm
long double resistorMoist; //kosteusanturin resistanssi ohmeina


double virtualArrayForMoisture[15]; //lasketaan anturin datasheetissa olevan taulukon pohjalta

int columHigher; //sarakkeet
int columLower;

int rowHigher; //rivit
int rowLower;



double lowerResistance;
double higherResistance;

int lowerTemp;
int higherTemp;

int lowerMoist;
int higherMoist;

double weightTempLower; //painotuskertoimet
double weightTempHigher;

double weightMoistureLower;

double virtualRowNumber;
bool vRNcalculated;
double virtualWeightResistanceLow;
double virtualWeightResistanceHigh;





//******************KOSTEUS FUNKTIOT************************
double CalculateWeight(double value, double lowerValue, double higherValue); //laskee painotuskertoimen lowerValue muuttujalle
void CheckColumNumber();

void CalculateResistanceMoist();
void CalculateVirtualArrayForMoisture();
void CalculateVirtualRowNumber();
void CalculateMoisture();
void CalculateMoistureAverage();

void MeasureMoisture(); //pääfunktio joka nostaa pinnin HIGH tilaan ja ajaa muut tarvittavat funktiot









int main() 
{
    rawDataTemp = 0;
    rawDataMoist = 0;
    resistorMoist = 0;
    resistorTemp = 0;
    UoutMoist = 0;
    UoutTemp = 0;
    avgCounter = 0;

    Uin = 3.3; //käyttöjännite

    resistor1 = 1000; //lämpötilan sarjavastus ohmeina
    resistor2 = 830000; //kosteuden sarjavastus ohmeina
    //resistor2 = 830; //kosteuden sarjavastus kohmeina

    PowerToMoisture = 0;

    //pc.printf("kosteusresistorin alustus: %f", resistorMoist);

//timer.start();
//timer.reset();


	while(true)
	{
               //rawDataMoist = AnalogToDigitalMoist;

               rawDataTemp = AnalogToDigitalTemp;
               UoutTemp = Uin - rawDataTemp*Uin; //muutetaan volteiksi. pelkkä lampotilaRaaka*Uin, jos sarvavastus on plusnavan puolella ja lämpötilasensori miinus puolella.

               resistorTemp = resistor1/(Uin/UoutTemp -1);

               temperature = 0.1291 * resistorTemp -107.99; // tämä vaikutti tarkimmalta pikavertailulla kahden muun mittarin kanssa
               //temperature = -4*(10^(-5))* resistorTemp^2 + 0.2125 * resistorTemp - 146.78; // tämä ei toimi
                //temperature = (-0.00004) * (resistorTemp * resistorTemp) + (0.2125 * resistorTemp) - 146.78;


               MeasureMoisture();

                //debuggausta
               //temperature = -10.0;
               //moistureAvg2 =80;

//************************Tulosteet Raspberrylle********************
             /*  if(temperature < -99.9 )
               {

                   pc.printf("-99.9"); //lämpötila

               }else

                   if(temperature > -99.9 && temperature <= -10)
                   {

                       pc.printf("%.1lf", temperature); //lämpötila

                   }else

                   if(temperature < 0 && temperature > -10)
                   {

                       pc.printf("%+5.1lf", temperature); //lämpötila

                   }else






                if(temperature >= 0 && temperature < 10)
                {

                    pc.printf("%.1lf", temperature); //lämpötila

                }else

                    if(temperature >= 10 && temperature < 100)
                    {

                        pc.printf("+%.1lf", temperature); //lämpötila

                    }else

                        if(temperature > 99.9)
                        {
                            //Tulosteet Raspberrylle
                            pc.printf("99.9"); //lämpötila

                        }else

*/



               pc.printf("%+5.1lf", temperature); //LÄMPÖTILA




                if(moistureAvg2 < 20)
                {
                    pc.printf("m20.0"); //
                    //pc.printf("\n");
                }
                else
                if(moistureAvg2 > 90)
                {
                    pc.printf("m90.0"); //
                    //pc.printf("\n");
                }
                else
                {
                    pc.printf("m%.1lf", moistureAvg2);
                    //pc.printf("\n");
                }

               //Debuggausta
               //pc.printf("%f ", rawDataTemp);

               //pc.printf("%f ", UoutTemp);

               //pc.printf("temperature sensor resistance %f ", resistorTemp);
               //pc.printf("\n");



               //pc.printf("\n");

               //pc.printf("moisture: %f ", moisture);
               //pc.printf("\n");

               //pc.printf("moistureAvg: %f ", moistureAvg1);
               //pc.printf("\n");



               //rivin vaihto
               //pc.printf("\r\n");

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
    wait(0.001);

    rawDataMoist = AnalogToDigitalMoist;

    PowerToMoisture = 0;


    CalculateResistanceMoist();
    CalculateVirtualArrayForMoisture();
    CalculateVirtualRowNumber();
    CalculateMoisture();
    CalculateMoistureAverage();



}





//laskee painotuskertoimen alemmalle arvolle
double CalculateWeight(double value, double lowerValue, double higherValue)
{
    double weight;
    double difference = higherValue - lowerValue;

    //pc.printf("difference: %f", difference);
    //pc.printf("\n");


    if(difference == 0)
    {
        weight = 1;
    }else
    if(difference != 0)
    {
        weight = (difference - (value - lowerValue))/difference;
    }


    //pc.printf("weight: %f", weight);
    //pc.printf("\n");

    return weight;
}

//laskee kosteusanturin resistanssin raakadatasta
void CalculateResistanceMoist()
{

    UoutMoist = Uin - rawDataMoist*Uin; //muutetaan volteiksi. pelkkä raakaData*Uin, jos sarjavastus on plusnavan puolella ja lämpötilasensori miinus puolella.

    // U = RI -> Rx = Ux/Ix -> Rx = Ux/(U1/R1)
    resistorMoist = rawDataMoist/((1-rawDataMoist)/resistor2);  //3.3V on supistettu pois ja jännitteen tilalla käytetään raakadataa (1 = 3.3V)



    //pc.printf("moisture sensor resistance: %f ohm", resistorMoist);
    //pc.printf("\n");

    //muutetaan kilo-ohmeiksi taulukkoa varten
    resistorMoist = resistorMoist/1000;

    //pc.printf("moisture sensor resistance: %f kOhm", resistorMoist);
    //pc.printf("\n");


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
     vRNcalculated = false;


    if(resistorMoist >= virtualArrayForMoisture[0] && vRNcalculated == false)
    {
        virtualRowNumber = 0;
        //pc.printf("virtual row number pitäs olla 0 mutta on %.f", virtualRowNumber);

        vRNcalculated = true;
    }//else

    if(resistorMoist <= virtualArrayForMoisture[14] && vRNcalculated == false)
    {
        virtualRowNumber = 14;
        //pc.printf("virtual row number pitäs olla 14 mutta on %.f", virtualRowNumber);
        vRNcalculated = true;
    }//else

    if(vRNcalculated == false)
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
    }

      //pc.printf("virtual row number: %.2f", virtualRowNumber);
      //pc.printf("\n");
      //pc.printf("\n");

      vRNcalculated = false;

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

void CalculateMoistureAverage()
{
    //painotettu keskiarvo
    moistureAvg1 =  moistureAvg1 * 0.95 + moisture * 0.05;

     //kolmen minuutin keskiarvo
             moistureAvg2Sum = moistureAvg2Sum - moistureAvg2Array[avgCounter];

            moistureAvg2Array[avgCounter] = moisture;
             //pc.printf("lisättiin paikaan %d", avgCounter);
             //pc.printf("\n");


            moistureAvg2Sum = moistureAvg2Sum + moisture;


            if(avgCounter <= 16)
            {
                 avgCounter++;
            }else

        if(avgCounter >=17)
        {



           //moistureAvg2Sum = moistureAvg2Sum + moisture - moistureAvg2Array[0];

            avgCounter = 0;

           // moistureAvg2Array[avgCounter] = moisture;
            // pc.printf("lisättiin paikaan %d", avgCounter);
           //  pc.printf("\n");


        }



            moistureAvg2 = moistureAvg2Sum / 18;
}

