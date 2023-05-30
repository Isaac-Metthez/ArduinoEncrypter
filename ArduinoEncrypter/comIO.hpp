#ifndef ComIO
#define ComIO
#include "encryptedCom.hpp"
namespace	communication
{
  template<typename  T = bool>
  class Input
  {
    protected:
      friend class EncryptedCom;
      void (*_setValue)(T);
      void set(T value){(*_setValue)(value);}
    public:
      Input(void (*setValue)(T) ):_setValue(setValue){}
  };
  
  template<typename T = bool>
  class Output
  {
    protected:
      friend class EncryptedCom;
      T (*_getValue)();
      T get(){return (*_getValue)();}
    public:
      Output(T (*getValue)()):_getValue(getValue){}
  };
  
  typedef Input<bool> digitalInput;
  typedef Input<int> analogInput;
  typedef Output<bool> digitalOutput;
  typedef Output<int> analogOutput;
}
#endif