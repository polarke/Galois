#ifndef _GALOIS_EMBEDDING_H_
#define _GALOIS_EMBEDDING_H_

#include "galois/base.h"

namespace gs {

    template<typename T>
    class Embedding : public PFilter<T>
    {
    private:
        SP_Signal<T> in_signal = nullptr;
        SP_Signal<T> out_signal = nullptr;
        size_t in_size = 0;
        size_t out_size = 0;

        SP_NArray<T> w = nullptr;
        SP_NArray<T> dw = nullptr;

    public:
        Embedding(const bool for_share_or_clone) {}
        Embedding(const Embedding&) = delete;
        Embedding& operator=(const Embedding&) = delete;
        Embedding(size_t in_size, size_t out_size);

        SP_Filter<T> share() override;
        SP_Filter<T> clone() override;

        void install_signals(const vector<SP_Signal<T>> &in_signals, const vector<SP_Signal<T>> &out_signals) override;
        void set_dims(size_t batch_size) override;
        void reopaque() override;

        vector<SP_NArray<T>> get_params() override;
        vector<SP_NArray<T>> get_grads() override;

        void forward() override;
        void backward() override;
    };

}

#endif
