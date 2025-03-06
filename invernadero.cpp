#include "StateMachineLib.h"
#include "AsyncTaskLib.h"
#define LDR_PIN A0             //REVIZAR---------------------------

#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
float  hum;
float temp;
int luminosidad = 0;

//pines
const int ledPin = 13;

//void compTempFunct(void);
//AsyncTask readTempTask(2500, true, compTempFunct);

void readHum(void);
AsyncTask readHumTask(3200, true, readHum);

void readTem(void);
AsyncTask readTemTask(2500, true, readTem);

void readLuz(void);
AsyncTask readLuzTask(1600, true, readLuz);

void pasoLuz(void);

void ambiEstado(void);

void alarmaEstado(void);

void luzEstado(void);

void controlLed(void);

void lecturaLuz(void);

void PasarEstRes(void);
AsyncTask TimeEstRes(3000, false, PasarEstRes);

void PasarEstFor(void);
AsyncTask TimeEstFor(3000, false, PasarEstFor);

void encenderLed(void);
AsyncTask TimeEnLed(3000, false, encenderLed);

void apagarLed(void);
AsyncTask TimeApLed(3000, false, apagarLed);

// State Alias
enum State  
{
	Monitoreo_Ambiental = 0,
	Monitoreo_Luz = 1,
	Alarma = 2
};

// Input Alias
// la enumeracion de entrada se hace coon imput
enum Input
{
  // estas son las entradas|
	Reset = 0,
	Forward = 1,
	Backward = 2,
  Unknown = 3, 

};

// Create new StateMachine

StateMachine stateMachine(4, 9);

// Stores last user input
Input input;

// Setup the State Machine
// aqui se configura la maquina de estado
void setupStateMachine()
{
	// addicionar transisiones
	stateMachine.AddTransition(Monitoreo_Ambiental, 	Monitoreo_Luz, []() { return input == Forward; });

	stateMachine.AddTransition(	Monitoreo_Luz, Monitoreo_Ambiental, []() { return input == Forward; });
	stateMachine.AddTransition(Monitoreo_Luz, Alarma, []() { return input ==Backward; });
	stateMachine.AddTransition(Alarma, Monitoreo_Ambiental, []() { return input == Forward; });

	stateMachine.AddTransition(Monitoreo_Ambiental, Alarma, []() { return input == Reset; });


	// Add acciones
  // funciones que realiza cada funcion o cada flecha
	stateMachine.SetOnEntering(Monitoreo_Ambiental, ambiEstado);
	stateMachine.SetOnEntering(Monitoreo_Luz, luzEstado);
	stateMachine.SetOnEntering(Alarma, alarmaEstado);


	stateMachine.SetOnLeaving(Monitoreo_Ambiental, []() {Serial.println("Leaving Monitoreo_Ambiental"); });
	stateMachine.SetOnLeaving(Monitoreo_Luz, []() {Serial.println("Leaving Monitoreo_Luz"); });
	stateMachine.SetOnLeaving(Alarma, []() {Serial.println("Leaving Alarma"); });

}

void setup() 
{
	Serial.begin(9600);

  dht.begin();
  
  TimeEnLed.SetIntervalMillis(300);
  //timeout3.Start();

  TimeApLed.SetIntervalMillis(700);
  //7timeout4.Start();

  pinMode(ledPin, OUTPUT);
	Serial.println("Starting State Machine...");
	setupStateMachine();	
	Serial.println("Start Machine Started");

	// Initial state
	stateMachine.SetState(Monitoreo_Ambiental, false, true);
}

void loop() 
{
	// Update State Machine
	stateMachine.Update();
}

// Auxiliar function that reads the user input
int readInput()
{
	 Input currentInput = Input::Unknown;
	if (Serial.available())
	{
		char incomingChar = Serial.read();

		switch (incomingChar)
		{
			case 'R': currentInput = Input::Reset; 	break;  // sing_H
			case 'A': currentInput = Input::Backward; break; // sing_LD
			case 'D': currentInput = Input::Forward; break; //sing_T
			default: break;
		}
	}

	return currentInput;
}

//void compTempfunct(void) {
//float hum = dht.readHumidity();
//float temp = dht.readTemperature();

// Temp = DHT.Temperature();
//if((Temperature>24) && ( humedad>70)) {
//  input = Reset;
//}
void readHum(void){
  hum = dht.readHumidity(); 
} 

void readTem(void){
  temp = dht.readTemperature();
}

void PasarEstRes(void){
 input = Forward;

}

void PasarEstFor(void){
  input = Forward;
}

void pasoLuz(void){
  TimeEstFor.SetIntervalMillis(5000);
  TimeEstFor.Start();
}

//FUNCIONES DENTRO DEL ESTADO MONITOREO AMBIENTAL
void ambiEstado(void){
  static bool timerStarted = false;  // Para evitar reiniciar el temporizador en cada llamada

  if (!timerStarted) {
    pasoLuz();
    timerStarted = true;
  }

  if (TimeEstFor.IsExpired()) {
    timerStarted = false;  // Reiniciar el estado para futuras llamadas
  }
  
  else 
    if ((temp>24) && ( hum>70)){
        input = Reset;
    }
  
}

void encenderLed(void) {
  digitalWrite(ledPin, HIGH);
}

void apagarLed(void){
  digitalWrite(ledPin, LOW);
}
void controlLed(void) {
  static bool ledOn = false;
  bool startLuzEn = false;

  if (!startLuzEn) {
    TimeEnLed.Start();
    startLuzEn = true;
  }

  if (TimeEnLed.IsExpired()) {
    encenderLed();
    ledOn = true;
    TimeApLed.Start();  // Inicia el segundo temporizador
  }

  if (ledOn && TimeApLed.IsExpired()) {
    apagarLed();
    ledOn = false;
    startLuzEn = false;
  }
}


//FUNCIONES PARA EL ESTADO DE ALARMA----------------------------------------------------
void alarmaEstado(void) {
  static bool timerStarted = false;  // Variable para evitar reiniciar el temporizador
  controlLed();
  
  if (!timerStarted) {
    TimeEstFor.SetIntervalMillis(6000);
    TimeEstFor.Start();
    timerStarted = true;
  }

  if (TimeEstFor.IsExpired()) {
    timerStarted = false;  // Permite que el temporizador se pueda reiniciar en futuras llamadas
  }
}


//FUNCIONES PARA EL ESTADO DE LUZ-------------------------------------------------------

void readLuz(void){
  int luminosidad = analogRead(LDR_PIN);
}

void luzEstado(void){

  static bool timerStarted = false;  // Variable para evitar reiniciar el temporizador

  if (!timerStarted) {
    TimeEstFor.SetIntervalMillis(3000);
    TimeEstFor.Start();
    timerStarted = true;
  }

  if (TimeEstFor.IsExpired()) {
    timerStarted = false;  // Permite que el temporizador se pueda reiniciar en futuras llamadas
  }

  if (luminosidad >= 500){
      input = Backward;
    }
}

