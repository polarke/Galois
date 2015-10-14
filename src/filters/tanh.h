#include "galois/base.h"

namespace gs {
    
    template<typename T>
    class Tanh : public BFilter<T> {
    public:
        void set_dims(SP_Signal<T> in_signal, SP_Signal<T> out_signal, int batch_size);
        void set_dims(const vector<SP_Signal<T>> &in_signals,
                      const vector<SP_Signal<T>> &out_signals,
                      int batch_size) override;

        void forward(const vector<SP_Signal<T>> &in_signals, const vector<SP_Signal<T>> &out_signals) override {
            cout << "forward" << endl;
        }
            
        void backward(const vector<SP_Signal<T>> &in_signals, const vector<SP_Signal<T>> &out_signals) override {
            cout << "backward" << endl;
        }
    };
    
}