//projektityö Sulautettu
//Ryhmä 4, TVT18SPO
//Janne, Heikkinen, Juha Lallukka & Miika Röyttä






#include "mbed.h"
//#include "L3G4200D.h"

Timer timer;
Serial pc(USBTX,USBRX);
//L3G4200D gyro(D14, D15);
//int g[3]={0, 0, 0};
//float kulma[3] = {0.0, 0.0, 0.0};

AnalogIn AnalogToDigital(A0); //

//lämpötilan mittaus
float lampotilaRaaka; // 3.3V = 1.0 raakadatana, viidellä voltilla tarkempi mittaustulos, mutta yli 3.3V:n arvot virheellisiä
float lampotila; // celsius-asteina

float Uout; //mitattu jännite
float Uin; //käyttöjännite
int R1; //sarjavastus ohmeina, kokoluokkaa 1 kOhm
float Rt; //lämpötila-anturin resistanssi ohmeina

//kosteuden mittaus
float kosteusRaaka;
float kosteus;


float R2; //sarjavastus ohmeina, kokoluokkaa 50kOhm
float Rk; //kosteusanturin resistanssi ohmeina
float kerroin; //vaihtelee 118.05 & 97.254 välillä (0 & 50 *C)
float potenssi; // vaihtelee -0.143 & -0.185 välillä (0 & 50 *C)

void KosteusMittaus(); //kaavamuotoa: kosteus = kerroin * Rk ^ potenssi






int main() 
{
    lampotilaRaaka = 0;

    Uin = 3.3; //käyttöjännite
    R1 = 1000; //lämpötilan sarjavastus ohmeina



//timer.start();
//timer.reset();


	while(true)
	{
               lampotilaRaaka = AnalogToDigital;
               Uout = Uin - lampotilaRaaka*Uin; //muutetaan volteiksi. pelkkä lampotilaRaaka*Uin, jos sarvavastus on plusnavan puolella ja lämpötilasensori miinus puolella.

               Rt = R1/(Uin/Uout -1);

               lampotila = 0.1291 * Rt -107.99;

               //Tulosteet
               pc.printf("%f ", lampotilaRaaka);

               pc.printf("%f ", Uout);

               pc.printf("%f ", Rt);

               pc.printf("%f ", lampotila);

               //rivin vaihto
               pc.printf("\r\n");

               wait(0.02);  //Ei tarvi odottaa jos pikavauhtia etsitään


	}

        void KosteusMittaus()
        {
            if(lampotila < 0)
            {

            }


        }
}

