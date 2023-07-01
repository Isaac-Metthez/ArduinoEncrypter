#ifndef ComIO
#define ComIO

namespace	communication
{
  template<typename  T = bool>
  class Input
  {
    protected:
      void (*_setValue)(T);
    public:
      void set(T value){(*_setValue)(value);}
      Input(void (*setValue)(T) ):_setValue(setValue){}
  };
  
  template<typename T = bool>
  class Output
  {
    protected:
      T (*_getValue)();
    public:
      T get(){return (*_getValue)();}
      Output(T (*getValue)()):_getValue(getValue){}
  };
  
  typedef Input<bool> digitalInput;
  typedef Input<int> analogInput;
  typedef Output<bool> digitalOutput;
  typedef Output<int> analogOutput;
}
#endif