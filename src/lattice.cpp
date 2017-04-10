#include "lattice.h"
#include "error.h"
#include <iostream>
#include <random>
#include <ctime>
#include <string>
#include <string.h>

using namespace std;

//////////// PROTECTED ////////////

vector<string> Lattice::split_string(string str, char const sep[]) // La fonction split string qui n'est pas implémentée...
{
  vector<size_t> many_pos;
  vector<string> split_string;
  for (size_t pos=0; pos<str.size()-1; pos++)
    if (str.compare(pos, 1, sep)==0)
      many_pos.push_back(pos);

  if (many_pos.empty())
    many_pos.push_back(str.size());

  if (many_pos.back() < str.size())
    many_pos.push_back(str.size());
  
  split_string.push_back(simplify_string(str.substr(0, many_pos[0])));
  for(int i=0; i<many_pos.size()-1; i++)
    split_string.push_back(simplify_string(str.substr(many_pos[i]+1, many_pos[i+1]-many_pos[i]-1)));

  return split_string;
}

string Lattice::simplify_string(string str) // La fonction simplify string qui n'est pas non plsu implémentée
{
  for (int i=0; i< str.size(); i++)
    if (str.compare(i, i+1, " ")==0) str.erase(i,1);
  return str;
}

void Lattice::add_to_layer(int layer, Neuron *neuron)
{
  if (layer >= layers.size()) layers.resize(layer+1);
  nb_layers = layer +1;
  layers[layer].insert(neuron);
}

void Lattice::fill_output()
{
  for (auto & out : output)
    out.second = neurons_ptrs.at(out.first)->get_output();
}


void Lattice::fill_input_neuron(Neuron* n_ptr)
{
  double input;
  int nb_parents = 0;
  for (auto const &parent_ptr : get_parents(n_ptr))
  {
    //cout << n_ptr->get_name() << " " << n_ptr  << " " << parent_ptr->get_name() << " " << parent_ptr << endl;
    input += parent_ptr->get_output()*get_weight(n_ptr, parent_ptr);
    nb_parents++;
  }
  input /= nb_parents;
  n_ptr->set_input(input);
}

void Lattice::switch_weights()
{
  switch(WEIGHTS_MAP)
  {
    case FIRST:
      WEIGHTS_MAP = SECOND;
      break;
    case SECOND:
      WEIGHTS_MAP = FIRST;
      break;
    default:
      throw WeightsMapsAccessError();
      break;
  }
}

void Lattice::update_weight(Neuron* n_ptr1, Neuron* n_ptr2, double value)
{
  switch(WEIGHTS_MAP)
  {
    case FIRST:
      update_weight_first(n_ptr1, n_ptr2, value);
      break;
    case SECOND:
      update_weight_second(n_ptr1, n_ptr2, value);
      break;
    default:
      throw WeightsMapsAccessError(n_ptr1->get_name(), n_ptr2->get_name());
      break;
  }
}

void Lattice::update_weight_first(Neuron* n_ptr1, Neuron* n_ptr2, double value)
{
  try{weights_first.at(ReciprocicalLink_t (n_ptr1, n_ptr2)) += value;}
  catch(...) {throw WeightsMapsAccessError(n_ptr1->get_name(), n_ptr2->get_name());}
}

void Lattice::update_weight_second(Neuron* n_ptr1, Neuron* n_ptr2, double value)
{
  try{weights_second.at(ReciprocicalLink_t (n_ptr1, n_ptr2)) += value;}
  catch(...){throw WeightsMapsAccessError(n_ptr1->get_name(), n_ptr2->get_name());}
}

double Lattice::get_weight(Neuron* n_ptr1, Neuron* n_ptr2)
{
  switch(WEIGHTS_MAP)
  {
    case FIRST:
      return get_weight_first(n_ptr1, n_ptr2);
      break;
    case SECOND:
      return get_weight_second(n_ptr1, n_ptr2);
      break;
    default:
      throw WeightsMapsAccessError();
      return 0;
  }
}

double Lattice::get_weight_first(Neuron* n_ptr1, Neuron* n_ptr2)
{
  try{return weights_first.at(ReciprocicalLink_t(n_ptr1, n_ptr2));}
  catch(...){throw WeightsMapsAccessError(n_ptr1->get_name(), n_ptr2->get_name());}
}


double Lattice::get_weight_second(Neuron* n_ptr1, Neuron* n_ptr2)
{
  try{return weights_second.at(ReciprocicalLink_t(n_ptr1, n_ptr2));}
  catch(...){throw WeightsMapsAccessError(n_ptr1->get_name(), n_ptr2->get_name());}
}

Neuron* Lattice::get_neuron_ptr(const string& name)
{
  if (neurons_ptrs.count(name)>0)
    return neurons_ptrs[name];
  else
  {
    throw NameError(name);
    return NULL;
  }
}

const set<Neuron*>& Lattice::get_children(Neuron * n_ptr)
{
  if (forward_links.count(n_ptr)>0)
    return forward_links[n_ptr];
  else
  {
    throw PointerError(n_ptr);
    return forward_links.begin()->second;
  }
}

const set<Neuron*>& Lattice::get_parents(Neuron * n_ptr)
{
  if (backward_links.count(n_ptr)>0)
    return backward_links[n_ptr];
  else
  {
    throw PointerError(n_ptr);
    return backward_links.begin()->second;
  }
}

double Lattice::get_expected_value(string name)
{
  if (expected_values.count(name)>0)
    return expected_values[name];
  else
  {
    throw NameError(name);
    return 0;
  }
}
  
int Lattice::get_min_neurons_per_layer()
{
  int min = layers[0].size();
  for (auto const& layer : layers)
    if (layer.size()<min) min=layer.size();
  return min;
}

/////////// PUBLIC //////////

Lattice::Lattice(ifstream &_argfile, double _learning_rate)  : learning_rate(_learning_rate), argfile(_argfile) {}

void Lattice::init()
{
  char buffer [max_length_line];
  string strbuffer(buffer);
  srand(time(NULL));
  int counter=0;

  for(; counter<max_lines_argfile; counter++) //Commentaires et première ligne
  {
    argfile.getline(buffer,max_length_line);
    strbuffer.assign(buffer);
    if (strbuffer.empty()) continue;
    if (!strbuffer.substr(0,1).compare("#")) continue;
    if (strbuffer.compare("NEURONS - NAME : TYPE, LAYER")==0) break;
    else throw ArgfileError("Before 'NEURONS - NAME : TYPE, LAYER'");
  }
  if (counter == max_lines_argfile)
    throw ArgfileError("Can't find 'NEURONS - NAME : TYPE, LAYER' flag");

  vector<string> splitstr1, splitstr2;
  int layer;

  for(; counter<max_lines_argfile; counter++) //Déclaration des neurones
  {
    argfile.getline(buffer,max_length_line);
    strbuffer.assign(buffer);
    if (strbuffer.empty()) continue;
    if (!strbuffer.substr(0,1).compare("#")) continue;
    if (!strbuffer.compare("BACKWARD_LINKS - NAME : PARENTS' NAMES")) break;
    splitstr1 = split_string(strbuffer, ":");
    if (splitstr1.size() != 2) throw ArgfileError("Missing ':' in 'BACKWARD_LINKS - NAME : PARENTS' NAMES' section");
    splitstr2 = split_string(splitstr1[1], ",");
    if (splitstr2.size() != 2) throw ArgfileError("Missing ',' in 'BACKWARD_LINKS - NAME : PARENTS' NAMES' section");
    if (splitstr2[0].compare("SIGMOID") == 0)
      neurons_ptrs[splitstr1[0]] = new NeurSigmoid(splitstr1[0], 1);
    else 
    {
      if (splitstr2[0].compare("LINEAR") == 0)
        neurons_ptrs[splitstr1[0]] = new NeurLinear(splitstr1[0], 1);
      else 
      {
        if (splitstr2[0].compare("HEAVISIDE") == 0)
          neurons_ptrs[splitstr1[0]] = new NeurHeaviside(splitstr1[0]);
        else
        {
          if (splitstr2[0].compare("GAUSSIAN") == 0)
            neurons_ptrs[splitstr1[0]] = new NeurGaussian(splitstr1[0], 1, 0);
          else
            throw ArgfileError("Type of neuron '" + splitstr1[0] + "' not understood");
        } 
      }
    }
    if (get_neuron_ptr(splitstr1[0]) == NULL)
      throw MemoryError(string("Neuron named ") + splitstr1[0]);
    try{layer = stoi(splitstr2[1]);}
    catch(...){throw ArgfileError(string("Layer of ") + splitstr1[0] + string(" not understood"));}
    add_to_layer(layer, neurons_ptrs[splitstr1[0]]);
  }
  if (counter == max_lines_argfile)
    throw ArgfileError("Can't find 'BACKWARD_LINKS - NAME : PARENTS' NAMES' flag");
  
  size_t pos;

  for(; counter<max_lines_argfile; counter++) //Création des liens entre neurones
  {
    if (argfile.eof()) break;
    argfile.getline(buffer,max_length_line);
    strbuffer.assign(buffer);
    if (strbuffer.empty()) continue;
    if (!strbuffer.substr(0,1).compare("#")) continue;
    splitstr1 = split_string(strbuffer, ":");
    if (splitstr1.size() != 2) throw ArgfileError("Missing ':' in 'BACKWARD_LINKS - NAME : PARENTS' NAMES' section");
    splitstr2 = split_string(splitstr1[1], ",");
    for (auto const& name : splitstr2)
    {
      backward_links[get_neuron_ptr(splitstr1[0])].insert(neurons_ptrs[name]);
      forward_links[get_neuron_ptr(name)].insert(neurons_ptrs[splitstr1[0]]);
    }
  }
  
  if (counter == max_lines_argfile)
    throw ArgfileError("Can't find EOF");

  for (auto const& nm_ptr : neurons_ptrs)
  {
    if (layers.front().find(nm_ptr.second) != layers.front().end())
      input[nm_ptr.first];
    if (layers.back().find(nm_ptr.second) != layers.back().end())
      output[nm_ptr.first];
  }

  double value;
  pair<map<ReciprocicalLink_t, double>::iterator, bool> inserted;
  for (auto const& n_links_ptr : backward_links)
    for (auto const& parent_ptr : n_links_ptr.second)
    {
      value = (double) 2*rand()/RAND_MAX-1;
      inserted = weights_first.insert(pair<ReciprocicalLink_t, double>(ReciprocicalLink_t(n_links_ptr.first, parent_ptr), value));
      if(!inserted.second)
        throw WeightsMapsCreationError(n_links_ptr.first->get_name(), parent_ptr->get_name());
    }
  weights_second = map<ReciprocicalLink_t, double, ReciprocicalLink_Compare_t> (weights_first.begin(), weights_first.end());
}

Lattice::~Lattice()
{
  for (auto const & nm_ptr : neurons_ptrs)
    delete nm_ptr.second;
}

void Lattice::set_input(string name, double value)
{
  if (input.find(name) != input.end())
    input[name] = value;
  else
    throw NameError(name);
}

void Lattice::set_expected_value(string name, double value)
{
  if (input.find(name) != input.end())
    input[name] = value;
  else
    throw NameError(name);
}

double Lattice::get_output(string name)
{
  if (output.find(name) != output.end())
    return output[name];
  else
  {
    throw NameError(name);
    return 0;
  }
}

set<string> Lattice::get_input_names()
{
  set<string> keys;
  for (auto const &key : input)
    keys.insert(key.first);
  return keys;
}

set<string> Lattice::get_output_names()
{
  set<string> keys;
  for (auto const &key : output)
    keys.insert(key.first);
  return keys;
}

void Lattice::evaluate()
{
  for(auto const& w : weights_first)
    cout << w.first.get_n_ptr1()->get_name() << " (" << w.first.get_n_ptr1() << ") - "
         << w.first.get_n_ptr2()->get_name() << " (" << w.first.get_n_ptr2() << ")" 
         << " " << w.second << " " << weights_first.count(w.first) << endl;
  for(auto const& neuron : input)
    get_neuron_ptr(neuron.first)->set_input(neuron.second);
  vector<set<Neuron*>>::iterator it = layers.begin(); 
  for(it++; it!=layers.end(); it++)
    for (auto const& n_ptr : *it)
    {
      fill_input_neuron(n_ptr);
      n_ptr->evaluate();
    }
  fill_output();
}

void Lattice::update_weights()
{
  for(auto const &nm_ptr : neurons_ptrs)
    nm_ptr.second->differentiate();

  for(auto const &n_ptr : layers.back())
    for(auto const &parent_ptr : get_parents(n_ptr))
    {
      n_ptr->set_delta((n_ptr->get_output() - get_expected_value(n_ptr->get_name())) *
                n_ptr->get_der());
      update_weight(n_ptr, parent_ptr, - learning_rate * parent_ptr->get_output() * n_ptr->get_delta());
    }

  double factor;
  for(auto it = layers.end()-2; it>=layers.begin(); it--)
    for(auto const &n_ptr : *it)
    {
      factor = 0;
      for(auto const &child_ptr : get_children(n_ptr))
        factor += child_ptr->get_delta()*get_weight(n_ptr, child_ptr);
      for(auto &parent_ptr: get_parents(n_ptr))
        update_weight(n_ptr, parent_ptr, - learning_rate * parent_ptr->get_output() * factor * n_ptr->get_der());
    }
  switch_weights();
}


void Lattice::represent(ofstream& outfile)
{
  int min_n_per_l = get_min_neurons_per_layer();
  outfile << "\%\%\% AUTO GENERATED BY THE LATTICE REPRESENT FUNCTION" << endl
          << "\\documentclass[10pt]{standalone}" << endl << endl 
          << "\\usepackage{tikz}" << endl << endl
          << "\\tikzstyle{Neuron} = [draw, circle, fill opacity = 1, fill = white, minimum size = " << std_size << "pt, node distance = " << floor(3.5*std_size) << "pt]" << endl
          << "\\begin{document}" << endl
          << "\\begin{tikzpicture}" << endl;

  string precedent_name;
  for(int layer_nb=0; layer_nb< nb_layers; layer_nb++)
  {
    auto const &layer = layers[layer_nb];
    outfile << "\\node (top" << layer_nb << ") at (" << 5*layer_nb*std_size << "pt," << floor((double) (layer.size()-min_n_per_l)/2*4.5*std_size) << "pt) {};" << endl;
    precedent_name = string("top") + to_string(layer_nb);
    for(auto ite = layer.begin(); ite != layer.end(); ite++)
    {
      outfile << "\\node [Neuron, below of =" << precedent_name << "] (" << (*ite)->get_name() << ") {" << (*ite)->get_name() << "};" << endl;
      precedent_name.replace(0, string::npos, (*ite)->get_name());
    }
    outfile << endl ;
  }
  outfile << endl ;


  for(auto const &link : backward_links)
  {
    for(auto const &parent : link.second)
      outfile << "\\path (" << link.first->get_name() << ") edge (" << parent->get_name() << ");" << endl;
    outfile << endl;
  }

  outfile << "\\end{tikzpicture}" << endl
          << "\\end{document}";

}
