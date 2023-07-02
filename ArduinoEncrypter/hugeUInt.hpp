#ifndef HUGEINT
#define HUGEINT

namespace	utils
{
  template<unsigned int S>
  class HugeUint
  {
    protected:
      unsigned int _value[S];
    public:
      HugeUint()
      {
        reset();
      };
      HugeUint(const unsigned int *value)
      {
        memcpy(_value, value, S * sizeof(unsigned int));
      };
      void reset()
      {
        memset(_value, 0 , S * sizeof(unsigned int));
      };
      bool operator==(const HugeUint& compared) const
      {
        bool result = true;
        for (unsigned int i = 0; i < S ; ++i)
          result &= _value[i] == compared._value[i];
        return result; 
      };
      HugeUint& operator++()
      {
        for (unsigned int i = 0; i < S ; ++i)
          if (++_value[i] != 0)
            break;
        return *this;
      };
      unsigned int& operator[](unsigned int index)
      {
        return _value[index];
      }
      
      unsigned int* getArray()
      {
        return _value;
      }
  };
  typedef HugeUint<3> Huge3Uint;
}
#endif