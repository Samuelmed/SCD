// ----------------------------------------------------------------------------
// Sistemas Concurrentes Y Distribuidos
// Samuel Medina Gutiérrez
// Practica 2
// Problema de los fumadores
//
// archivo: Fumadores_su.cpp
// Ejemplo del Problema de los fumadores con semántica SU
//
// Historial:
// Creado en Noviembre de 2018
// ----------------------------------------------------------------------------


// En esta version del barbero voy a intentar implementarlo con tres barberos,
// Además si la sala de la barbería está llena, los clientes se van a dormir un rato

#include <iostream>
#include <iomanip>
#include <random>
#include "HoareMonitor.h"


using namespace std;
using namespace HM;

const int BARBEROS = 3;
const int CLIENTES = 12;

//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
  static default_random_engine generador( (random_device())() );
  static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
  return distribucion_uniforme( generador );
}

// ----------------------------------------------------------------------

void esperarFueraBarberia(int num_cliente){
  // calcular milisegundos aleatorios de duración de la acción de fumar)
  chrono::milliseconds duracion_espera( aleatorio<20,200>() );

  // informa de que comienza a fumar



  // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
  this_thread::sleep_for( duracion_espera );

  // informa de que ha terminado de fumar

   cout << " \nCliente " << num_cliente << "  : ha decidido ir a pelarse." << endl;
}

// ----------------------------------------------------------------------

void cortarPeloACliente(){
  // calcular milisegundos aleatorios de duración de la acción de fumar)
  chrono::milliseconds duracion_pelado( aleatorio<20,200>() );

  // informa de que comienza a fumar



  // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
  this_thread::sleep_for( duracion_pelado );

  // informa de que ha terminado de fumar

   cout << "\nTermina de pelarse." << endl;
}

// ----------------------------------------------------------------------

class Barberia : public HoareMonitor{
  private:
    CondVar colaSilla,
            colaSala,
            colaDormir;

  public:
    Barberia();
    void cortarPelo(int i);
    void siguienteCliente(int i);
    void finCliente();
};

// ----------------------------------------------------------------------

Barberia::Barberia(){
  colaSilla=newCondVar();
  colaSala=newCondVar();
  colaDormir=newCondVar();
}

// ----------------------------------------------------------------------

void Barberia::cortarPelo(int i){
  if(!colaSala.empty() || !colaSilla.empty()){
    colaSala.wait();
  }

  colaDormir.signal();
    cout << "\nEntra el cliente " << i;

  colaSilla.wait();
}


// ----------------------------------------------------------------------

void Barberia::siguienteCliente(){
  if(colaSala.empty()&&colaSilla.empty()){
    colaDormir.wait();
        cout << "\nA DESCANSAR";

  } else if(!colaSala.empty()){
    colaSala.signal();

  }
}

// ----------------------------------------------------------------------

void Barberia::finCliente(){
  colaSilla.signal();
}

// ----------------------------------------------------------------------

void funcion_cliente (MRef<Barberia> monitor, int i){
  while(true){
    monitor->cortarPelo(i);
    esperarFueraBarberia(i);
  }
}

// -----------------------------------------------------------------------

void funcion_barbero(MRef<Barberia> monitor){
  while(true){
    monitor->siguienteCliente();
    cortarPeloACliente();
    monitor->finCliente();
  }
}

// -----------------------------------------------------------------------


int main(){
  MRef<Barberia> monitor = Create<Barberia>(  );

  thread barbero, clientela[CLIENTES];

  barbero = thread(funcion_barbero, monitor);

  for (int i=0; i<CLIENTES; i++){
    clientela[i] = thread(funcion_cliente,monitor,i);
  }

  barbero.join () ;
  for ( int i =0 ; i<CLIENTES ; i++) {
    clientela[i].join() ;
  }
}
