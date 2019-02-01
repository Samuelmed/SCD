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

#include <iostream>
#include <iomanip>
#include <random>
#include "HoareMonitor.h"

using namespace std;
using namespace HM;

const int FUMADORES = 3;

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

// ---------------------------------------------------------------------------

class Estanco : public HoareMonitor{
  private:

    int ingrediente ;

    CondVar colaFumadores[FUMADORES],
            colaEstanquero;
  public:
    Estanco();
    void obtenerIngrendiente(int i);
    void ponerIngrediente(int ingre);
    void esperarRecogidaIngrediente();
};

// --------------------------------------------------------------------------

Estanco::Estanco(){
  ingrediente=-1;
  for(int i=0; i<FUMADORES;i++){
    colaFumadores[i] = newCondVar();
  }
  colaEstanquero = newCondVar();
}

// --------------------------------------------------------------------------

void Estanco::obtenerIngrendiente(int i){
  while(ingrediente!=i){
    colaFumadores[i].wait();
  }

  ingrediente=-1;
  colaEstanquero.signal();
}

// --------------------------------------------------------------------------

void Estanco::ponerIngrediente(int ingre){
  ingrediente=ingre;
  cout << "\nPuesto el ingrediente " << ingre;
  colaFumadores[ingre].signal();
}

// --------------------------------------------------------------------------

void Estanco::esperarRecogidaIngrediente(){
  while(ingrediente!=-1){
    colaEstanquero.wait();
  }

}

// --------------------------------------------------------------------------

void fumar (int num_fumador){
  // calcular milisegundos aleatorios de duración de la acción de fumar)
  chrono::milliseconds duracion_fumar( aleatorio<20,200>() );

  // informa de que comienza a fumar

   cout << "\nFumador " << num_fumador << "  :"
         << " empieza a fumar (" << duracion_fumar.count() << " milisegundos)" << endl;

  // espera bloqueada un tiempo igual a ''duracion_fumar' milisegundos
  this_thread::sleep_for( duracion_fumar );

  // informa de que ha terminado de fumar

   cout << " \nFumador " << num_fumador << "  : termina de fumar, comienza espera de ingrediente." << endl;
}

//--------------------------------------------------------------------------
// Producir un ingrediente aleatorios
int ProducirIngrediente(){
    return aleatorio<0,FUMADORES-1>();
}

// -------------------------------------------------------------------------

void funcion_hebra_estanquero(MRef<Estanco> monitor){
  int ingre;
  while(true){
    ingre = ProducirIngrediente();
    monitor->ponerIngrediente(ingre);
    monitor->esperarRecogidaIngrediente();

  }
}

// --------------------------------------------------------------------------

void funcion_hebra_fumador(MRef<Estanco> monitor, int i){
  while(true){
    monitor->obtenerIngrendiente(i);
    fumar(i);
  }
}

// --------------------------------------------------------------------------



// --------------------------------------------------------------------------

int main(){

  MRef<Estanco> monitor = Create<Estanco>(  );

  thread estanquero, fumadores[FUMADORES];

  estanquero = thread(funcion_hebra_estanquero, monitor);

  for (int i=0; i<FUMADORES; i++){
    fumadores[i] = thread(funcion_hebra_fumador,monitor,i);
  }

  estanquero.join () ;
  for ( int i =0 ; i<FUMADORES ; i++) {
 fumadores[i].join() ;
  }
}
