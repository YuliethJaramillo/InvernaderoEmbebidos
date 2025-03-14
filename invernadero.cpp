#include "StateMachineLib.h"
#include "AsyncTaskLib.h"
#define LDR_PIN 34             //REVIZAR---------------------------

#include "DHT.h"

#define DHTPIN 13
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
float  hum;
float temp;
int luminosidad = 0;

//pines
const int ledPin = 4;

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

void encenderLedd(void);
AsyncTask TimeEnLed(3000, false, encenderLedd);

void apagarLed(void);

void apagarLedd(void);
AsyncTask TimeApLed(3000, false, apagarLedd);

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

StateMachine stateMachine(3, 5);

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
  
  TimeApLed.SetIntervalMillis(700);
  

  pinMode(ledPin, OUTPUT);
	Serial.println("Starting State Machine...");
	setupStateMachine();	
	Serial.println("Start Machine Started");
    readHumTask.Start();
    readTemTask.Start();
    readLuzTask.Start();
  input = Unknown;

	// Initial state
	stateMachine.SetState(Monitoreo_Ambiental, false, true);
}

	// Update State Machine
void loop() {
    readHumTask.Update();
    readTemTask.Update();
    readLuzTask.Update();
    stateMachine.Update();
    TimeEnLed.Update();
    TimeApLed.Update();
    TimeEstFor.Update();

    Serial.print("Estado actual: ");
    Serial.println(stateMachine.GetState());
    Serial.print("Valor de input: ");
    Serial.println(input);
    
    if (stateMachine.GetState() == Monitoreo_Ambiental) {

        Serial.println("monitoreo ambiente de loop");
        ambiEstado(); // Evaluar continuamente la temperatura y humedad
    }
    if (stateMachine.GetState() == Alarma) {

        Serial.println("monitoreo alarma de loop");
        alarmaEstado(); // Evaluar continuamente la temperatura y humedad
    }
    if (stateMachine.GetState() == Monitoreo_Luz) {
        Serial.println("monitoreo alarma de loop");
        luzEstado(); // Evaluar continuamente la temperatura y humedad
    }

}



void readHum(void){
  hum = dht.readHumidity(); 
  Serial.print("humedad: ");
  Serial.println(hum);
} 

void readTem(void){
  temp = dht.readTemperature();
  Serial.print("temperatura: ");
  Serial.println(temp);
}

void PasarEstRes(void){
 input = Reset;

}

void PasarEstFor(void){
  Serial.println("Temporizador expirado, cambiando input a Forward");
  input = Forward;
}

void pasoLuz(void){
  TimeEstFor.SetIntervalMillis(5000);
  TimeEstFor.Start();
}

//FUNCIONES DENTRO DEL ESTADO MONITOREO AMBIENTAL
static bool timerStarted = false;
void ambiEstado(void) {

  Serial.println("estado ambiente");
  Serial.println(temp);
  Serial.println(hum);
  

  /*if (!timerStarted) {
    Serial.println("Iniciando temporizador luz");
    pasoLuz();
    timerStarted = true;
  }*/

  if ((temp > 18) && (hum > 70)) {
    Serial.println("Llegando a Alarma");
    input = Reset;
  }

  /*if (TimeEstFor.IsExpired()) {
    Serial.println("Completo temporizador luz");
    timerStarted = false;  // Reiniciar el temporizador
  }*/
}


void encenderLed(void) {
  digitalWrite(ledPin, HIGH);
}

void encenderLedd(void){};
void apagarLedd(void){};

void apagarLed(void){
  digitalWrite(ledPin, LOW);
}
  // Declarar fuera de la función

void controlLed(void) {
    static bool ledOn = false;

    if (!TimeEnLed.IsActive() && !TimeApLed.IsActive()) {
        Serial.println("Iniciando temporizador de encendido...");
        TimeEnLed.Start();
    }

    if (TimeEnLed.IsExpired() && !ledOn) {
        Serial.println("Encendiendo LED");
        encenderLed();
        ledOn = true;
        TimeApLed.Start();
        TimeEnLed.Stop();
    }

    if (TimeApLed.IsExpired() && ledOn) {
        TimeEnLed.Stop();
        Serial.println("Apagando LED");
        apagarLed();
        ledOn = false;
        TimeEnLed.Start();
        TimeApLed.Stop();
    }
}





//FUNCIONES PARA EL ESTADO DE ALARMA----------------------------------------------------
void alarmaEstado(void) {

  Serial.println("estado alarma");
  static bool timerStarted = false;  // Variable para evitar reiniciar el temporizador
  controlLed();
  
  if (!timerStarted) {
    Serial.println("iniciando temporizador para regresar a estado Alarma");
    TimeEstFor.SetIntervalMillis(6000);
    TimeEstFor.Start();
    timerStarted = true;
  }

  if (TimeEstFor.IsExpired()) {
   
    Serial.println("tiempo expirado");
    timerStarted = false;  // Permite que el temporizador se pueda reiniciar en futuras llamadas
  }
}


//FUNCIONES PARA EL ESTADO DE LUZ-------------------------------------------------------

void readLuz(void){
  luminosidad = analogRead(LDR_PIN);
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

