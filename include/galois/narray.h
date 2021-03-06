#ifndef _GALOIS_NARRAY_H_
#define _GALOIS_NARRAY_H_

#include "galois/utils.h"
#include <random>
#include <memory>
#include <iostream>
#include <vector>

using namespace std;

namespace gs
{
    template<typename T>
    class NArray;
    template<typename T>
    using SP_NArray = shared_ptr<NArray<T>>;

    const int   NARRAY_DIM_ZERO = 0;
    const int   NARRAY_DIM_ONE = 1;

    template<typename T>
    class NArray
    {
    public:
        static default_random_engine galois_rn_generator;

        explicit NArray(size_t m);
        explicit NArray(size_t m, size_t n);
        explicit NArray(size_t m, size_t n, size_t o);
        explicit NArray(size_t m, size_t n, size_t o, size_t k);
        explicit NArray(vector<size_t>);
        NArray() = delete;
        NArray(const NArray& other) = delete;
        NArray& operator=(const NArray&) = delete;
        ~NArray();

        vector<size_t> get_dims() { return dims; }
        size_t get_size() { return size; }
        T* get_data() { CHECK(data, "data should be non-empty"); return data; }
        bool opaque() { return data_opaque; }
        void reopaque() { data_opaque = true; }
        void setclear() { data_opaque = false; }

        void copy_from(const SP_NArray<T>);
        void copy_from(const vector<size_t> &, const SP_NArray<T>);
        void copy_from(const vector<size_t> &, size_t, const SP_NArray<T>);
        void copy_from(const size_t, const size_t, const SP_NArray<T>);
        void uniform(T lower, T upper) {
            // future : move random generator to a single file
            uniform_real_distribution<T> distribution(lower, upper);
            for (size_t i = 0; i < get_size(); i++) {
                data[i] = distribution(galois_rn_generator);
            }
            data_opaque = false;
        }
        void normalize_for(int dim);
        void fill(T x) {
            for (size_t i = 0; i < get_size(); i++) {
                data[i] = x;
            }
            data_opaque = false;
        }

    private:
        const vector<size_t> dims = {};
        size_t size = 0;
        T *data = nullptr;
        bool data_opaque = true;
    };
    template<typename T>
    default_random_engine NArray<T>::galois_rn_generator(0);

    template<typename T>
    ostream& operator<<(std::ostream &strm, const SP_NArray<T> M) {
        auto M_ptr = M->get_data();
        for (int i = 0; i < M->get_size(); i++) {
            strm << M_ptr[i] << '\t';
        }
        strm << endl;
        return strm;
    }

}

#endif
