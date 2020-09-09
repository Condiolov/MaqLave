#include <avr/wdt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
//LiquidCrystal_I2C lcd(0x3F, 16, 2);//outro lcd

bool pressioned = false; //Botão epressionado
String letra, executar, proximo;// LETRA é a soma das letras, EXECUTAR letra atual, PROXIMO letra
int num = 0, tfeita = 1, pode = 0, rapido = 0, altura;
// NUM quantidade de letra, Tarefa FEITA controle, PODE trocar a agua?, RAPIDO é programações predefinida.

int sensor   = 5; // altura da agua
int agua     = 2; // libera a agua 110v
int btnBATER = 8; // Botão B
int btnMOLHO = 9; // Botão M
int btnENXAG = 10;// Botão E
int btnCENTR = 11;// Botão C
int btnAPAGA = 7; // Botão X
int bomba    = 6; // Bomba drenar 110v
int SOM      = 12;// Alto falante beep
int motorA   = 4; // Liga e desliga a energia
int motorB   = 3; // comutação 12-34 ou 13-24 / 0-> bate  1-> Centrifuga

int tmpMOTOR = 5; // Tempo de bater a roupa 5 min
int tmpMOLHO = 5; // Tempo de molho 5 min
int tmpCENRTRIFUGACAO = 5; // Tempo de Centrifugar 5 min

// Inicio do programa!
void setup()
{
  wdt_disable(); // desabilita o cao de guarda!!

  // SAIDAS
  pinMode(btnAPAGA, INPUT);
  pinMode(btnBATER, INPUT);
  pinMode(btnMOLHO, INPUT);
  pinMode(btnENXAG, INPUT);
  pinMode(btnCENTR, INPUT);
  pinMode(sensor,   INPUT);

  // ENTRADAS
  pinMode(bomba,  OUTPUT);
  pinMode(SOM,    OUTPUT);
  pinMode(motorA, OUTPUT);
  pinMode(motorB, OUTPUT);
  pinMode(agua,   OUTPUT);

  // motor totalmente desligado no inicio!!
  digitalWrite(motorA, LOW);
  digitalWrite(motorB, LOW);

  lcd.begin();            // inicia a biblioteca LCD
  lcd.setBacklight(HIGH); // luz do LCD acessa!

  lcd.setCursor(0, 0); // Primeira linha
  lcd.print("Seja Bem Vindo!!");
  lcd.setCursor(0, 1); // Segunda linha
  lcd.print("      MaqLave1.3" );

  // Musiquinha de Inicio!!
  tone(12, 600, 500);
  delay(300);
  tone(12, 900, 500);
  delay(300);
  tone(12, 700, 500);
  delay(300);
  tone(12, 600, 500);
  delay(300);
  tone(12, 900, 500);

  delay(3000); // tempo de 3 segundos...
  lcd.clear(); // apaga a tela atual
}

// LOOOP do programa, rotina da maquina de lavar!
void loop() {

  lcd.setCursor(0, 1);
  lcd.print("   Aguardando..."); // Mensagem de aguardando comando!

  leitura();   // Leir os Botões assim que forem precionados!!
  principal(); // Função que distribui as tarefas!
}

//----- ini --- Função que distribui as Tarefas..
void principal() {

  executar = letra.substring(0, 1); // primeira letra
  letra    =  letra.substring(1);   // letra começa apartir da segunda letra
  num      = letra.length();        // tamanho da letra,

  // verifica cada letra e chama sua função especifica!
  if (executar == "B")
    bater();

  if (executar == "M")
    molho();

  if (executar == "E")
    enxague();

  if (executar == "C")
    centrifugar();

  // verifica se a tarefa foi realizada
  if (num == 0 && tfeita == 0) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tarefa Feita!!:)");

    tone(12, 900, 500);
    delay(500);
    tone(12, 1000, 500);
    delay(300);
    tone(12, 2000, 500);
    delay(200);
    tone(12, 800, 500);
    delay(200);
    tone(12, 2000, 500);
    delay(500);
    tone(12, 800, 500);
    delay(300);
    tone(12, 900, 500);
    delay(200);
    tone(12, 900, 500);
    delay(200);

    num = 0;
    tfeita = 1;
  }

}//----- fim --- Função que distribui as Tarefas..

//----- ini --- BATER a roupa --------------------
void bater() {

  altura = digitalRead(sensor); // Verifica a altura da agua vazio 0 | cheio 1

  if (altura == 1) {

    digitalWrite(motorA, HIGH); //apenas liga o motor.
    digitalWrite(motorB, LOW);  //bate roupa

    long inicio = millis(); // pega o tempo atual
    while (((millis() - inicio) / 1000) < 60 * tmpMOTOR) { //bate roupa durante tempo determindado
      lcd.setCursor(0, 0);
      lcd.print("Batendo a roupa!");

      int minutos = (millis() - inicio) / 1000 / 60 - tmpMOTOR;
      int segundos = 59 + ( minutos * 60 - ((millis() - inicio) / 1000 - 60 * tmpMOTOR));

      lcd.setCursor(0, 1);
      lcd.print (minutos + 1);
      lcd.print (" min ");
      lcd.print (segundos);
      lcd.print (" seg  ");

      leitura(); // faz a leitura dos botões
    }

    proximo = letra.substring(0, 1); // verifica se é pra continuar batendo e nao desligar o motor
    if (proximo != "B") { // É pra desligar o motor?
      digitalWrite(motorA, LOW);
      digitalWrite(motorB, LOW);
    }
  }

  if (altura == 0) {  // Maquina Vazia
    digitalWrite(agua, HIGH); // Libera a Água

    long inicio = millis();
    while (digitalRead(sensor) == LOW) { // inicia contagem do tempo até encher de Agua

      lcd.setCursor(0, 0);
      lcd.print("Enchendo de Agua");
      lcd.setCursor(0, 1);

      int minutos = (millis() - inicio) / 1000 / 60;
      int segundos = ((millis() - inicio) / 1000 - minutos * 60);

      lcd.print (minutos);
      lcd.print (" min ");
      lcd.print (segundos);
      lcd.print (" seg  ");

      leitura();
    }
    digitalWrite(agua, LOW);    // Maquina cheia de agua desliga a agua
    bater(); // encheu de água chama a funçao novamente para ser realizada!
  }
} //----- fim --- BATER a roupa -------------------

//----- ini --- roupa de MOLHO --------------------
void molho() {

  altura = digitalRead(sensor);

  if (altura == 1) {

    long inicio = millis();
    while (((millis() - inicio) / 1000) < 60 * tmpMOLHO) { //Tempo a ficar de molho

      lcd.setCursor(0, 0);
      lcd.print("Roupa de molho! ");

      int minutos = (millis() - inicio) / 1000 / 60 - tmpMOLHO;
      int segundos = 59 + ( minutos * 60 - ((millis() - inicio) / 1000 - 60 * tmpMOLHO));

      lcd.setCursor(0, 1);
      lcd.print (minutos + 1);
      lcd.print (" min ");
      lcd.print (segundos);
      lcd.print (" seg  ");

      leitura();
    }
  }

  if (altura == 0) {          // roupa de molho sem agua?
    digitalWrite(agua, HIGH); // liga a agua!

    long inicio = millis();
    while (digitalRead(sensor) == LOW) {

      lcd.setCursor(0, 0);
      lcd.print("Enchendo de Agua");
      lcd.setCursor(0, 1);
      int minutos = (millis() - inicio) / 1000 / 60;
      int segundos = ((millis() - inicio) / 1000 - minutos * 60);

      lcd.print (minutos);
      lcd.print (" min ");
      lcd.print (segundos);
      lcd.print (" seg  ");

      leitura();

    }
    digitalWrite(agua, LOW);    // Maquina cheia de agua desliga a agua
    molho();

  }
}//----- fim --- roupa de MOLHO --------------------

//----- ini --- ENXAGUE da roupa --------------------
void enxague() {

  altura = digitalRead(sensor);

  if (altura == 0 && pode == 1) {
    pode = 0;
    digitalWrite(bomba, HIGH);

    long inicio = millis();
    while (((millis() - inicio) / 1000) < 60 * 1) {

      lcd.setCursor(0, 0);
      lcd.print("Trocando a Agua...");


      int minutos = (millis() - inicio) / 1000 / 60 - 1;
      int segundos = 59 + ( minutos * 60 - ((millis() - inicio) / 1000 - 60 * 1));

      lcd.setCursor(0, 1);
      lcd.print (minutos + 1);

      lcd.print (" min ");
      lcd.print (segundos);
      lcd.print (" seg  ");

      leitura();

    }

    digitalWrite(bomba, LOW);


  }

  if (altura == 1) {
    pode = 1;
    digitalWrite(bomba, HIGH);
    long inicio = millis();
    while (digitalRead(sensor) == 1) {

      lcd.setCursor(0, 0);
      lcd.print("Drenando a Agua...");
      lcd.setCursor(0, 1);
      int minutos = (millis() - inicio) / 1000 / 60;
      int segundos = ((millis() - inicio) / 1000 - minutos * 60);

      lcd.print (minutos);
      lcd.print (" min ");
      lcd.print (segundos);
      lcd.print (" seg  ");

      leitura();

    }
    enxague();

  }


}





void centrifugar() {

  sensor = digitalRead(sensor);

  if (altura == 1)
    enxague();

  if (altura == 0) {
    digitalWrite(motorB, HIGH);
    digitalWrite(motorA, HIGH);


    digitalWrite(bomba, HIGH);

    long inicio = millis();
    while (((millis() - inicio) / 1000) < 60 * tmpCENRTRIFUGACAO / 2) {
      lcd.setCursor(0, 0);
      lcd.print("Centrifugando.............");

      int minutos = (millis() - inicio) / 1000 / 60 - tmpCENRTRIFUGACAO;
      int segundos = 59 + ( minutos * 60 - ((millis() - inicio) / 1000 - 60 * tmpCENRTRIFUGACAO));

      lcd.setCursor(0, 1);
      lcd.print (minutos + 1);
      lcd.print (" min ");
      lcd.print (segundos);
      lcd.print (" seg  ");

      leitura();

    }


    digitalWrite(bomba, LOW);

    while (((millis() - inicio) / 1000) < 60 * tmpCENRTRIFUGACAO / 2) {
      lcd.setCursor(0, 0);
      lcd.print("Centrifugando.............");

      int minutos = (millis() - inicio) / 1000 / 60 - tmpCENRTRIFUGACAO;
      int segundos = 59 + ( minutos * 60 - ((millis() - inicio) / 1000 - 60 * tmpCENRTRIFUGACAO));

      lcd.setCursor(0, 1);
      lcd.print (minutos + 1);
      lcd.print (" min ");
      lcd.print (segundos);
      lcd.print (" seg  ");

      leitura();

    }

    proximo = letra.substring(0, 1);
    if (proximo != "C") {
      digitalWrite(motorA, LOW);
      long inicio = millis();
      while (((millis() - inicio) / 1000) < 60 * 1) {
        lcd.setCursor(0, 0);
        lcd.print ("Desligando Motor.");
      }
      digitalWrite(motorB, LOW);
    }

  }
}



void programacao() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Programacao:");
  lcd.setCursor(0, 1);



  lcd.print(executar);
  lcd.print(">");
  lcd.print(letra.substring(0));



  for (int j = 0; j < 20; j++) {
    delay(100);
    leitura();
  }

}


void leitura() {
  int botao07 = digitalRead(btnAPAGA);
  int botao08 = digitalRead(btnBATER);
  int botao09 = digitalRead(btnMOLHO);
  int botao10 = digitalRead(btnENXAG);
  int botao11 = digitalRead(btnCENTR);


  if ((botao08 == LOW) && (botao09 == LOW) && (botao10 == LOW) && (botao11 == LOW) && (botao07 == LOW))
    pressioned = false;



  if (!pressioned) {
    if (botao08 == HIGH) {
      letra.concat("B");
      tone(12, 1000, 100);
      pressioned = true;
      rapido = 0;
    }

    if (botao09 == HIGH) {
      letra.concat("M");
      tone(12, 1000, 100);
      pressioned = true;
      rapido = 0;
    }

    if (botao10 == HIGH) {
      letra.concat("EB");
      tone(12, 1000, 100);
      pressioned = true;
      rapido = 0;
    }

    if (botao11 == HIGH) {
      letra.concat("C");
      tone(12, 1000, 100);
      pressioned = true;
      rapido = 0;
    }

    if (botao07 == HIGH) {
      //letra.concat("M");
      letra = letra.substring(0, letra.length() - 1);


      tone(12, 3000, 100);
      pressioned = true;
      if (letra == "")
        rapido++;
    }


    if (letra == "EEB" || letra ==  "EE" || letra == "EC" || letra ==  "EM")
      credito();







    if (pressioned) {
      tfeita = 0;


      if (rapido > 1) //Programação Rapida
        programas();
      else
        programacao();
    }

  }

  delay(100);
}



void programas() {
  lcd.clear();
  long ini = millis();
  String temp, opcao;
  while (((millis() - ini) / 1000) < 7) {
    lcd.setCursor(2, 0);
    lcd.print("Opcao");

    for (int j = 0; j < 10; j++) {
      delay(100);
      leitura();
    }
    temp.concat(">");//letra.concat("M");
    lcd.print (temp);
    //lcd.print ("  ");




    if (rapido == 2) {
      lcd.setCursor(0, 0);
      lcd.print("1");
      lcd.setCursor(0, 1);
      opcao = "BMBMBEBMBC" ;
      lcd.print(opcao);
      rapido = 3;
    }
    if (rapido == 4) {
      lcd.setCursor(0, 0);
      lcd.print("2");
      lcd.setCursor(0, 1);
      opcao = "BBMBBMBEBMBC" ;
      lcd.print(opcao);
      rapido = 5;
    }
    if (rapido == 6) {
      lcd.setCursor(0, 0);
      lcd.print("3");
      lcd.setCursor(0, 1);
      opcao = "BBMMBBMMBBEBBMMBBC" ;
      lcd.print(opcao);
      rapido = -1;
    }

  }

  if (rapido > 1)
    letra = opcao;
}

void credito() {
  if (letra == "EEB" ) { //EXE
    lcd.setCursor(0, 0);
    lcd.print("Desenvolvido:");
    lcd.setCursor(0, 1);
    lcd.print("Thiago Conde..");
  }
  if (letra == "EE" ) { //EXEX
    lcd.setCursor(0, 0);
    lcd.print("VERSAO FIRMWARE:");
    lcd.setCursor(0, 1);
    lcd.print("MaqLave1.3 jun15");
  }
  if (letra == "EM" ) { //EXB
    lcd.setCursor(0, 0);
    lcd.print("Agradecimento:");
    lcd.setCursor(0, 1);
    lcd.print("Romelia e Toledo");
    //letra = "EB";
  }
  if (letra == "EC" ) { //EXB
    lcd.setCursor(0, 0);
    lcd.print("Tudo que te vier,");
    lcd.setCursor(0, 1);
    lcd.print("faz c tua forca");
    //letra = "EB";
  }
}
