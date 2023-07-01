#pragma once

namespace com
{
    template<typename  T = bool>
    class Input
    {
        private:
            T& value;
        public:
            void set(T _value) { value = _value; }
            Input(T& _value) : value(_value) {}
    };

    template<typename T = bool>
    class Output
    {
        private:
            T& value;
        public:
            T get() { return value; }
            Output(T& _value) : value(_value) {}
    };

    typedef Input<bool> digitalInput;
    typedef Input<int> analogInput;
    typedef Output<bool> digitalOutput;
    typedef Output<int> analogOutput;
}