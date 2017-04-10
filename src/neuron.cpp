
#include "neuron.h"
#include <cmath>
#include <string>
using namespace std;

// -------------------- Neuron -------------------- //
Neuron::Neuron(string _name)
{
  name = _name;
}

double Neuron::get_output() const {return output;}

string Neuron::get_name() const {return name;}

void Neuron::set_delta(double _delta){delta = _delta;}

double Neuron::get_delta() const {return delta;}

double Neuron::get_der() const {return der;}

void Neuron::set_input(double _input){input = _input;}

// -------------------- Neur_Sigmoid -------------------- //
NeurSigmoid::NeurSigmoid(string _name, double _beta) : Neuron(_name), beta(_beta)
{
}
//------
void NeurSigmoid::evaluate()
{
  output = 1/( 1+exp(- input*beta) );
}
//------
void NeurSigmoid::differentiate()
{
  der = beta*output*(1-output);
  //der = ( beta *exp( -input*beta ) ) / pox( 1+exp(- input*beta) , 2)
}
// -------------------- Neur_Heaviside -------------------- //
NeurHeaviside::NeurHeaviside(string _name) : Neuron(_name)
{
}
//------
void NeurHeaviside::evaluate()
{
  if (input==0) output = 0;
  else output = 0.5*( abs(input) + input ) / abs(input);
}
//------
void NeurHeaviside::differentiate()
{
  if (input==0) der = 1;
  else der = 0;
}

// -------------------- Neur_Linear -------------------- //
NeurLinear::NeurLinear(string _name, double _x_min) : Neuron(_name), x_min(_x_min)
{
}
//------
void NeurLinear::evaluate()
{
  output = 0.5*( abs(input + x_min) + (input + x_min) );
}
//------
void NeurLinear::differentiate()
{
  if (input < -x_min || input > x_min) der = 0;
  else der = 1/(2*x_min);
}

// -------------------- Neur_Gaussian -------------------- //
NeurGaussian::NeurGaussian(string _name, double _sig, double _mu) : Neuron(_name), sig(_sig), mu(_mu)
{
}
//------
void NeurGaussian::evaluate()
{
  output = 1/( sqrt(2*M_PI*sig) *exp(- pow((input - mu),2) / (2*pow(sig,2)) )) ;
}
//------
void NeurGaussian::differentiate()
{
  der = ((input - mu)/pow(sig,2)) * output;
}




