#include <string>


class Error
{
  protected :
    string spec;
  public :
    Error() {spec = string("An error has occured");} 
    Error(const string& _what) {spec = string("An error has occured (");} 
    const char* what(){return spec.c_str();}
};

class MemoryError : public Error
{
  public:
    MemoryError(const string& _name)
    {spec = string("OS couln't allocate memory (") + _name + string(")");}
};

class PointerError : public Error
{
  public:
    PointerError(const Neuron* _ptr)
    {
      char buffer [50];
      sprintf(buffer, "This neuron pointer is unknown : %p",_ptr);
      spec = string(buffer);
    }
};

class NameError : public Error
{
  public:
    NameError(const string& _name) {spec = string("This neuron name is unknown : ") + _name;}
};

class LinkError : public Error
{
  public :
    LinkError(const string& _name1, const string& _name2)
    {spec = string("There is no link between ") + _name1 + string(" and ") + _name2;}
};

class WeightsMapsAccessError : public Error
{
  public :
    WeightsMapsAccessError()
    {spec = string("Can't access weights maps");}

    WeightsMapsAccessError(const string& _name1, const string& _name2)
    {spec = string("Can't access link between ") + _name1 + string(" and ") + _name2;}
};

class WeightsMapsCreationError : public Error
{
  public :
    WeightsMapsCreationError()
    {spec = string("Can't create weights maps");}

    WeightsMapsCreationError(const string& _name1, const string& _name2)
    {spec = string("Can't create link between ") + _name1 + string(" and ") + _name2;}
};

class ArgfileError : public Error
{
  public : 
    ArgfileError()
    {spec = string("Can't read argfile");}

    ArgfileError(string _why)
    {spec = string("Can't read argfile (") + _why + string(")");}

    ArgfileError(char*& _why)
    {spec = string("Can't read argfile (") + string(_why) + string(")");}
};



