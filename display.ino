/*
#-------------------------#
|         DISPLAY         |
|          NOKIA          |
|      Ítalo  Coelho      |
#-------------------------#
*/
 
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


#define BACKLIGHT_PIN D2   // Pino que controla a luz de fundo
#define CLK_PIN D3 
// CLK, DIN, DC, CE, RST
Adafruit_PCD8544 display = Adafruit_PCD8544(8,9,10,11,12);
  
void setupDisplay() 
{
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);
   
  display.begin();          //Inicializa o Display
   
  display.setContrast(56);  //Define o contraste
}
 
void loopDisplay() 
{
  display.clearDisplay();                 //Limpa a tela
 
  // display.drawBitmap( 0, 0, img, 84, 25, BLACK);    //Exibe imagem Bitmap
   
  display.display();                      //Exibe no display
   
  delay(5000);
   
  display.clearDisplay();
 
  display.setTextSize(1);                 //Tamanho da fonte
  display.setTextColor(BLACK);            //Texto Preto
  display.setTextColor(WHITE, BLACK);     //Texto Branco com fundo Preto
 
  display.setCursor(0,0);                 //Posição do Cursor
 
  display.print("ITALO");                 //Escreve o Texto
 
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("Display Nokia 5110");
 
  display.display();
 
  //Pisca a luz de fundo
  static bool backLight = LOW;
  backLight = !backLight;
  digitalWrite(BACKLIGHT_PIN, backLight);
   
  delay(5000);
}