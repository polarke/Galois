#include "galois/base.h"

namespace gs {
    
    template<typename T>
    class Linear : public PFilter<T> {
    public:
        int in_size;
        int out_size;
        NArray<T> *w;
        NArray<T> *b;
        NArray<T> *dw;
        NArray<T> *db;
        
        Linear() = delete;
        Linear(int in_size, int out_size);
        void set_dims(SP_Signal<T> in_signal, SP_Signal<T> out_signal, int batch_size);
        void set_dims(const vector<SP_Signal<T>> &in_signals,
                      const vector<SP_Signal<T>> &out_signals,
                      int batch_size) override;
        
        void forward(const vector<SP_Signal<T>> &in_signals, const vector<SP_Signal<T>> &out_signals) override;
            
        void backward(const vector<SP_Signal<T>> &in_signals, const vector<SP_Signal<T>> &out_signals) override {
            cout << "backward" << endl;
        }
    };
    
}