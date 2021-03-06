#ifndef _GALOIS_BASE_H_
#define _GALOIS_BASE_H_

# include "galois/narray.h"
# include "galois/utils.h"
# include <iostream>
# include <random>
# include <vector>
# include <set>

using namespace std;

namespace gs
{
    enum SignalType { InnerSignal, InputSignal, OutputSignal };

    // for performance reason, polymorphism is not used
    template<typename T>
    class Signal
    {
    private:
        const SignalType type;

        SP_NArray<T> data = nullptr;
        // only for inner signal
        SP_NArray<T> grad = nullptr;

        // only for output signal
        SP_NArray<T> target = nullptr;
        shared_ptr<T> loss = nullptr;

    public:
        Signal() = delete;
        explicit Signal(SignalType type) : type(type) {};
        Signal(const Signal& other) = delete;
        Signal& operator=(const Signal&) = delete;

        bool empty() {
            return (data == nullptr) &&
                   (grad == nullptr) &&
                   (target == nullptr) &&
                   (loss == nullptr);
        }

        SignalType      get_type()      { return type;  }
        SP_NArray<T>    get_data()      { return data;  }
        SP_NArray<T>    get_grad()      { return grad;  }
        SP_NArray<T>    get_target()    { return target;}
        shared_ptr<T>   get_loss()      { return loss;  }

        void reopaque() {
            if (data)   { data->reopaque(); }
            if (grad)   { grad->reopaque(); }
            if (target) { target->reopaque(); }
        }

        // set dims for data (and grad)
        void set_data_dims(size_t m)                                { set_data_dims({m}); }
        void set_data_dims(size_t m, size_t n)                      { set_data_dims({m,n}); }
        void set_data_dims(size_t m, size_t n, size_t o)            { set_data_dims({m,n,o}); }
        void set_data_dims(size_t m, size_t n, size_t o, size_t k)  { set_data_dims({m,n,o,k}); }
        void set_data_dims(initializer_list<size_t> nums) {
            set_data_dims(vector<size_t>(nums));
        }
        void set_data_dims(vector<size_t> nums) {
            CHECK(!data, "data should be nullptr before initialization");
            data = make_shared<NArray<T>>(nums);
            if (type == InnerSignal) {
                CHECK(!grad, "grad should be nullptr before initialization");
                grad = make_shared<NArray<T>>(nums);
            }
        }
        vector<size_t> get_data_dims() {
            CHECK(data, "data should be non-empty");
            return data->get_dims();
        }
        // set dims for target
        void set_target_dims(size_t m)                         { set_target_dims({m}); }
        void set_target_dims(size_t m, size_t n)                  { set_target_dims({m,n}); }
        void set_target_dims(size_t m, size_t n, size_t o)           { set_target_dims({m,n,o}); }
        void set_target_dims(size_t m, size_t n, size_t o, size_t k)    { set_target_dims({m,n,o,k}); }
        void set_target_dims(initializer_list<size_t> nums) {
            set_target_dims(vector<size_t>(nums));
        }
        void set_target_dims(vector<size_t> nums) {
            CHECK(type == OutputSignal, "only OutputSignal could set target");
            CHECK(!target, "target should be nullptr before initialization");
            target = make_shared<NArray<T>>(nums);
        }
        vector<size_t> get_target_dims() {
            CHECK(target, "target should be non-empty");
            return target->get_dims();
        }
        // initialize loss
        void initialize_loss() {
            CHECK(type == OutputSignal, "only OutputSignal could set loss");
            CHECK(!loss, "loss should be nullptr before initialization");
            loss = make_shared<T>(0);
        }
    };
    template<typename T>
    using SP_Signal = shared_ptr<Signal<T>>;


    // Filter does forward/backward propagation
    template<typename T>
    class Filter
    {
    public:
        virtual void forward() = 0;
        virtual void backward() = 0;
        virtual void install_signals(const vector<SP_Signal<T>> &in_signals, const vector<SP_Signal<T>> &out_signals) = 0;
        virtual void set_dims(size_t batch_size) = 0;
        virtual void reopaque() = 0;    // each filter only opaques its inner signal
        virtual shared_ptr<Filter<T>> share() = 0;  // return a SP_Filter<T>
        virtual shared_ptr<Filter<T>> clone() = 0;  // return a SP_Filter<T>
    };
    template<typename T>
    using SP_Filter = shared_ptr<Filter<T>>;

    template<typename T>
    class BFilter : public Filter<T> {
        shared_ptr<Filter<T>> clone() override { return this->share(); }
    };

    template<typename T>
    class PFilter : public Filter<T>
    {
    private:
        bool params_fixed = false;
    public:
        virtual vector<SP_NArray<T>> get_params() = 0;
        virtual vector<SP_NArray<T>> get_grads() = 0;
        void fix_params() { params_fixed = true; }
        bool is_params_fixed() { return params_fixed; }
    };
    template<typename T>
    using SP_PFilter = shared_ptr<PFilter<T>>;

    template<typename T>
    class GFilter : public Filter<T>
    {
    private:
        bool params_fixed = false;
    public:
        virtual set<SP_PFilter<T>> get_pfilters() = 0;
        void fix_params() {
            for (auto sp : get_pfilters()) {
                sp->fix_params();
            }
            params_fixed = true;
        }
        bool is_params_fixed() { return params_fixed; }
    };

}

#endif
