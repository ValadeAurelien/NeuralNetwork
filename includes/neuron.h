#ifndef NEURON_INCLUDED
#define NEURON_INCLUDED


#include <string>
using namespace std;

//enum type_t{SIGMOID = 0, AUTRE = 1};

// ===================== Mother ===================== //
class Neuron
{
  public:
  Neuron(string _name);
  virtual void evaluate(){}
  virtual void differentiate(){}
  double get_output() const;
  string get_name() const;
  double get_der() const;
  void set_delta(double _delta);
  double get_delta() const;
  void set_input(double _input);


  protected:
  string name;
  double input, output, der, delta;
};

// -------------------- Children -------------------- //
class NeurSigmoid    : public Neuron
{
  public:
    NeurSigmoid(string _name, double _beta);
    void evaluate();
    void differentiate();
  private:
    double beta;
};

class NeurHeaviside  : public Neuron
{
  public:
    NeurHeaviside(string _name);
    void evaluate();
    void differentiate();
  private:

};

class NeurLinear     : public Neuron
{
  public:
    NeurLinear(string _name, double _x_min);
    void evaluate();
    void differentiate();
  private:
    double x_min;
    double der;
};

class NeurGaussian   : public Neuron
{
  public:
    NeurGaussian(string _name, double _sig, double _mu);
    void evaluate();
    void differentiate();
  private:
    double sig;
    double mu ;

};


#endif
