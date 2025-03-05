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
AsyncTask readLuzTask(3200, true, readLuz);


void Timeout(void);
AsyncTask TimeoutTask(5000, false, Timeout);

void returAmb(void);

void pasoLuz(void);

void AmbiCambEstado(void);

void controlLed(void);

void lecturaLuz(void);

void regresoEstCA(void);
//Instancias de TimeoutTask:

void Timeout(void);
AsyncTask timeout1(3000, false, Timeout);

void PasaEstFor(void);
AsyncTask timeout2(3000, false, PasarEstFor);

void encenderLed(void);
AsyncTask timeout3(3000, false, encenderLed);

void apagarLed(void);
AsyncTask timeout4(3000, false, apagarLed);

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
	stateMachine.SetOnEntering(Monitoreo_Ambiental, outputMonitoreo_Ambiental);
	stateMachine.SetOnEntering(Monitoreo_Luz, outputMonitoreo_Luz);
	stateMachine.SetOnEntering(Alarma, outputAlarma);


	stateMachine.SetOnLeaving(Monitoreo_Ambiental, []() {Serial.println("Leaving Monitoreo_Ambiental"); });
	stateMachine.SetOnLeaving(Monitoreo_Luz, []() {Serial.println("Leaving Monitoreo_Luz"); });
	stateMachine.SetOnLeaving(Alarma, []() {Serial.println("Leaving Alarma"); });

}

void setup() 
{
	Serial.begin(9600);

  
  timeout3.SetIntervalMillis(300);
  //timeout3.Start();

  timeout4.SetIntervalMillis(700);
  /7timeout4.Start();

  pinMode(ledPin, OUTPUT);
	Serial.println("Starting State Machine...");
	setupStateMachine();	
	Serial.println("Start Machine Started");

	// Initial state
	stateMachine.SetState(Monitoreo_Ambiental, false, true);
}

void loop() 
{
	// Read user input
	input = static_cast<Input>(readInput());

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
  hum = dht.Humidity(); 
} 

void readTem(void){
  temp = dht.Temperature();
}

void Timeout(void){
 input = Forward;

}

void returAmb(void){
  timeout1.SetIntervalMillis(3000);
  timeout1.Start();
}

void PasaEstFor(void){
  input = Forward;
}

void pasoLuz(void){
  timeout2.SetIntervalMillis(5000);
  timeout2.Start();
}

void AmbiCambEstado(void){
  static bool timerStarted = false;  // Para evitar reiniciar el temporizador en cada llamada

  if (!timerStarted) {
    pasoLuz();
    timerStarted = true;
  }

  if (timeout2.IsExpired()) {
    input = Forward;
    timerStarted = false;  // Reiniciar el estado para futuras llamadas
  }
  
  else 
    if ((temp>24) && ( hum>70)){
        input = Reset;
        controlLed();
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

  if (timeout3.IsExpired()) {
    encenderLed();
    ledOn = true;
    timeout4.Start();  // Inicia el segundo temporizador
  }

  if (ledOn && timeout4.IsExpired()) {
    apagarLed();
    ledOn = false;
  }
}


//FUNCIONES PARA EL ESTADO DE ALARMA----------------------------------------------------
void regresoEstCA(void) {
  static bool timerStarted = false;  // Variable para evitar reiniciar el temporizador

  if (!timerStarted) {
    timeout2.SetIntervalMillis(6000);
    timeout2.Start();
    timerStarted = true;
  }

  if (timeout2.IsExpired()) {
    input = Reset;
    timerStarted = false;  // Permite que el temporizador se pueda reiniciar en futuras llamadas
  }
}


//FUNCIONES PARA EL ESTADO DE LUZ-------------------------------------------------------
void lecturaLuz(void){
  int luminosidad = analogRead(LDR_PIN);
}

void regresoEstCAdeLuz(void){

  static bool timerStarted = false;  // Variable para evitar reiniciar el temporizador

  if (!timerStarted) {
    timeout1.SetIntervalMillis(3000);
    timeout1.Start();
    timerStarted = true;
  }

  if (timeout1.IsExpired()) {
    timerStarted = false;  // Permite que el temporizador se pueda reiniciar en futuras llamadas
  }

  if (luminosidad >= 500){
      input = Forward;
    }
}

// Auxiliar output functions that show the state debug

/*"""void Funct_AMB_Init(void){
  serial.println("Funct_AMB_Init ")
  readTempTask.Start();
}"""
"""void Funct_AMB_Fin(void){
  serial.println("Funct_AMB_Fin ")
  readTempTask.Stop();
}"""
"""{
	Serial.println("	Monitoreo_Ambiental   	Monitoreo_Luz    Alarma   ");
	Serial.println("         X                                       ");
	Serial.println();
}

void outputMonitoreo_Luz()
{
	Serial.println("	Monitoreo_Ambiental   	Monitoreo_Luz    Alarma   ");
	Serial.println("                                 X                  ");
	Serial.println();
}

void outputAlarma()
{
	Serial.println("	Monitoreo_Ambiental   	Monitoreo_Luz    Alarma   ");
	Serial.println("                                                  X   ");
	Serial.println();
}"""*/
