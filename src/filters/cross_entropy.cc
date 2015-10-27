#include "galois/filters/cross_entropy.h"
#include <cmath>
#include <cassert>

namespace gs {
    
    template<typename T>
    void CrossEntropy<T>::install_signals(const vector<SP_Signal<T>> &in_signals, const vector<SP_Signal<T>> &out_signals) {
        CHECK(in_signal == nullptr, "in_signal should not be initialized before");
        CHECK(out_signal == nullptr, "out_signal should not be initialized before");
        CHECK(in_signals.size() == 1, "only need 1 in signal");
        CHECK(out_signals.size() == 1, "only need 1 out signal");
        
        in_signal = in_signals[0];
        out_signal = out_signals[0];
        CHECK(out_signal->get_type() == OutputSignal, "OutputSignal is needed");
    }
    
    template<typename T>
    void CrossEntropy<T>::set_dims(int batch_size) {
        CHECK(!in_signal->empty(), "in signal should be empty");
        auto in_dims = in_signal->get_data_dims();
        
        CHECK(out_signal->get_type() == OutputSignal, "OutputSignal is needed");
        CHECK(out_signal->empty(), "out signal should be empty");
        out_signal->set_data_dims(in_dims);
        out_signal->set_target_dims(batch_size);
        out_signal->set_extra_dims(batch_size);
        out_signal->initialize_loss();
    }

    template<typename T>
    void CrossEntropy<T>::forward() {
        auto in_data = in_signal->get_data();
        auto out_data = out_signal->get_data();
        CHECK(out_data->opaque(), "out data should be opaque");
        
        // softmax function
        MAP(out_data, [](T x){return exp(x);}, in_data);
        out_data->normalize_for(NARRAY_DIM_ZERO);
        
        auto target = out_signal->get_target();
        auto losses = out_signal->get_extra();
        PROJ_MAP(losses, [](T x){return -log(x);}, out_data, target);
        auto loss = out_signal->get_loss();
        SUM_POSITIVE_VALUE(losses, loss.get());
        *loss /= target->get_size();
    }
    
    template<typename T>
    void CrossEntropy<T>::backward() {
        auto in_grad = in_signal->get_grad();
        auto out_data = out_signal->get_data();
        auto target = out_signal->get_target();
        int batch_size = in_signal->get_data_dims()[0];
        MAP(in_grad, [batch_size](T y){return y/static_cast<T>(batch_size);}, out_data);
        SUB_MAP(in_grad, [batch_size](T y){return -1/static_cast<T>(batch_size);}, in_grad, SP_NArray<T>(nullptr), target);
    }

    template class CrossEntropy<float>;
    template class CrossEntropy<double>;

}
