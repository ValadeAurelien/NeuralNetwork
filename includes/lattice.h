#ifndef LATTICE_INCLUDED
#define LATTICE_INCLUDED

#include "neuron.h"

#include <map> // equivalent du dictionnaire python
#include <vector>
#include <set>
#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

using namespace std;

enum Weight_Map_t {FIRST, SECOND};

struct Neur_name_pt_t
{
  string name;
  Neuron * n_ptr;
};

class ReciprocicalLink_t
{
  private:
  Neuron *n_ptr1, *n_ptr2;
  
  public:

  ReciprocicalLink_t(Neuron * _n_ptr1, Neuron * _n_ptr2)
  {
    n_ptr1 = MAX(_n_ptr1, _n_ptr2);
    n_ptr2 = MIN(_n_ptr1, _n_ptr2);
  }

//  bool operator<(const ReciprocicalLink_t &link2) const
//  {
//    cout << n_ptr1->get_name() << " (" << n_ptr1 << ") " <<  n_ptr2->get_name() << " (" << n_ptr2 << ") " 
//         << link2.n_ptr1->get_name() << " (" << link2.n_ptr1 << ") " <<  link2.n_ptr2->get_name() << " (" << link2.n_ptr2 << ") " 
//         << (n_ptr1<link2.n_ptr1) << " " << (n_ptr2<link2.n_ptr2) << endl;
//
//    if (n_ptr1<link2.n_ptr1)
//      return true;
//    else if (n_ptr2<link2.n_ptr2)
//       return true;
//    else
//      return false;
//  }
  Neuron* get_n_ptr1() const { return n_ptr1; }
  Neuron* get_n_ptr2() const { return n_ptr2; }
  
};

struct ReciprocicalLink_Compare_t
{
  bool operator()(const ReciprocicalLink_t &l1, const ReciprocicalLink_t &l2) const
  {
    cout << l1.get_n_ptr1()->get_name() << " (" << l1.get_n_ptr1() << ") " << l1.get_n_ptr2()->get_name() << " (" << l1.get_n_ptr2() << ") " 
         << l2.get_n_ptr1()->get_name() << " (" << l2.get_n_ptr1() << ") " <<  l2.get_n_ptr2()->get_name() << " (" << l2.get_n_ptr2() << ") " 
         << (l1.get_n_ptr1()<l2.get_n_ptr1()) << " " << (l1.get_n_ptr2()<l2.get_n_ptr2()) ;

    bool c_1, c_2;
    if (l1.get_n_ptr1()<l2.get_n_ptr1())
      c_1 = true;
    else 
      c_1 = false;
    if (l1.get_n_ptr2()<l2.get_n_ptr2())
      c_2 = true;
    else
      c_2 = false;
    cout << " " << ( c_1 or c_2) << endl;
    return ( c_1 or c_2 );
  }
};

class Lattice
{
  protected :
  Weight_Map_t WEIGHTS_MAP = FIRST;
  const int max_lines_argfile = 100000,
            max_length_line = 16384,
            std_size = 20;
  int nb_neurons, nb_layers, max_neurons_per_layer;
  double learning_rate;
  map<string, Neuron*> neurons_ptrs;
  map<Neuron*, set<Neuron*>> backward_links, forward_links;
  vector<set<Neuron*>> layers;
  map<string, double> input, output, expected_values;
  map<ReciprocicalLink_t, double, ReciprocicalLink_Compare_t> weights_first, weights_second;
  ifstream &argfile;

  static vector<string> split_string(string str, char const sep[]);
  static string simplify_string(string str);
  
  void add_to_layer(int layer, Neuron * neuron);
  void fill_output();

  void fill_input_neuron(Neuron* n_ptr);
  
  void switch_weights();
  void update_weight(Neuron* n_ptr1, Neuron* n_ptr2, double value);
  void update_weight_first(Neuron* n_ptr1, Neuron* n_ptr2, double value);
  void update_weight_second(Neuron* n_ptr1, Neuron* n_ptr2, double value);
  double get_weight(Neuron* n_ptr1, Neuron* n_ptr2);
  double get_weight_first(Neuron* n_ptr1, Neuron* n_ptr2);
  double get_weight_second(Neuron* n_ptr1, Neuron* n_ptr2);

  Neuron* get_neuron_ptr(const string& name);
  const set<Neuron*> & get_children(Neuron * n_ptr);
  const set<Neuron*> & get_parents(Neuron * n_ptr);
  double get_expected_value(string name);

  int get_min_neurons_per_layer();

  public :

  Lattice(ifstream& _argfile, double _learning_rate);
  ~Lattice();

  void init();

  void set_input(string name, double value);
  void set_expected_value(string name, double value);
  double get_output(string name);
  set<string> get_input_names();
  set<string> get_output_names();

  void evaluate();
  void update_weights();

  void represent(ofstream& outfile);
};
  

#endif
