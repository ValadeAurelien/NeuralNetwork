#include "lattice.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <error.h>
#include <ctime>

using namespace std;
#define SAFE_CALL(call) try{call;} \
  catch(Error& error ){\
    system("tput setaf 1"); \
    cout << "ERROR : " << error.what() << endl; \
    system("tput sgr0"); \
    return -1;}


int main(int argc, char *argv[])
{
  if (argc != 2) 
  {
    cout << "Missing argfile path" << endl;
    return -1;
  }
  ifstream argfile (argv[1], ifstream::in);
  Lattice *Latt = new Lattice(argfile, 1);
  SAFE_CALL(Latt->init());
  argfile.close();
  set<string> input_names = Latt->get_input_names();
  set<string> output_names = Latt->get_output_names();

  for (auto const& name : input_names)
    Latt->set_input(name, rand()/RAND_MAX);

  SAFE_CALL(Latt->evaluate());

  
  for (auto const& name : output_names)
    cout << name << " " << Latt->get_output(name) << endl;

  ofstream outfile ("outfile.tex", ofstream::out);
  Latt->represent(outfile);
  outfile.close();
  return 0;
}

