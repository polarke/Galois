#include "galois/narray.h"
#include "galois/narray_functors.h"
#include "galois/filters/tanh.h"
#include <cmath>

namespace gs {

    template<typename T>
    SP_Filter<T> Tanh<T>::share() {
        return make_shared<Tanh<T>>();
    }

    template<typename T>
    void Tanh<T>::install_signals(const vector<SP_Signal<T>> &in_signals, const vector<SP_Signal<T>> &out_signals) {
        CHECK(in_signals.size() == 1, "need only 1 in signal");
        CHECK(out_signals.size() == 1, "need only 1 out signal");

        in_signal = in_signals[0];
        out_signal = out_signals[0];
    }

    template<typename T>
    void Tanh<T>::set_dims(size_t batch_size) {
        CHECK(!in_signal->empty(), "in signal should be initialized");
        auto in_dims = in_signal->get_data_dims();
        if (out_signal->empty()) {
            out_signal->set_data_dims(in_dims);
        } else {
            CHECK(in_dims == out_signal->get_data_dims(), "in signal and out signal should have the same dimensions");
        }
    }

    template<typename T>
    void Tanh<T>::forward() {
        auto in_data = in_signal->get_data();
        CHECK(!in_data->opaque(), "in_data should not be opaque");
        auto out_data = out_signal->get_data();
        CHECK(out_data->opaque(), "This Tanh could not work in parallel with the other filters, please use GeneralTanh instead");

        MAP(out_data, [](T x){ return tanh(x); }, in_data);
    }

    template<typename T>
    void Tanh<T>::backward() {
        auto in_grad = in_signal->get_grad();
        auto out_data = out_signal->get_data();
        auto out_grad = out_signal->get_grad();
        CHECK(!out_grad->opaque() && !out_data->opaque(), "these should not be opaque");

        MAP(in_grad, [](T dy, T y){return dy*(1-y*y);}, out_grad, out_data);
    }

    template class Tanh<float>;
    template class Tanh<double>;

}
