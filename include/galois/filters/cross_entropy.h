#ifndef _GALOIS_CROSSENTROPY_H_
#define _GALOIS_CROSSENTROPY_H_

#include "galois/base.h"

namespace gs {

    template<typename T>
    class CrossEntropy : public BFilter<T> {
    private:
        SP_Signal<T> in_signal = nullptr;
        SP_Signal<T> out_signal = nullptr;

        // for optimization
        SP_NArray<T> softmax_output = nullptr;
    public:
        CrossEntropy() {}
        CrossEntropy(const CrossEntropy&) = delete;
        CrossEntropy& operator=(const CrossEntropy&) = delete;

        SP_Filter<T> share() override;
        void reopaque() override { softmax_output->reopaque(); }

        void install_signals(const vector<SP_Signal<T>> &in_signals, const vector<SP_Signal<T>> &out_signals) override;
        void set_dims(size_t batch_size) override;

        void forward() override;
        void backward() override;
    };

}

#endif
