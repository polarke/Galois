# include "galois/narray.h"

namespace gs
{
    template<typename T>
    NArray<T>::NArray(size_t m) : dims{m}, size{m} {
        CHECK(m > 0, "m should be positive");
        data = new T[get_size()];
    }

    template<typename T>
    NArray<T>::NArray(size_t m, size_t n) : dims{m, n}, size{m*n} {
        CHECK(m > 0, "m should be positive");
        CHECK(n > 0, "n should be positive");
        data = new T[get_size()];
    }

    template<typename T>
    NArray<T>::NArray(size_t m, size_t n, size_t o) : dims{m, n, o}, size{m*n*o} {
        CHECK(m > 0, "m should be positive");
        CHECK(n > 0, "n should be positive");
        CHECK(o > 0, "o should be positive");
        data = new T[get_size()];
    }

    template<typename T>
    NArray<T>::NArray(size_t m, size_t n, size_t o, size_t k) : dims{m, n, o, k}, size{m*n*o*k} {
        CHECK(m > 0, "m should be positive");
        CHECK(n > 0, "n should be positive");
        CHECK(o > 0, "o should be positive");
        CHECK(k > 0, "k should be positive");
        data = new T[get_size()];
    }

    template<typename T>
    NArray<T>::NArray(vector<size_t> nums) : dims{nums} {
        for (auto m : nums) {
            CHECK(m > 0, "each dimension should be positive");
        }
        size = 1;
        for (auto d : dims) {
            size *= d;
        }
        data = new T[get_size()];
    }

    template<typename T>
    NArray<T>::~NArray() {
        if (data) {
            delete[] data;
        }
    }

    template<typename T>
    void NArray<T>::copy_from(const SP_NArray<T> other) {
        auto other_dims = other->get_dims();
        CHECK(other_dims == this->dims, "the dimension should be equal");

        auto other_ptr = other->get_data();
        for (size_t i = 0; i < this->get_size(); i++) {
            this->data[i] = other_ptr[i];
        }
        setclear();
    }

    template<typename T>
    void NArray<T>::copy_from(const vector<size_t> &idxs, const SP_NArray<T> dataset) {
        auto dataset_dims = dataset->get_dims();
        CHECK(idxs.size() == this->dims[0], "first dimension should be equal to batch size");
        CHECK((dataset->get_size() / dataset->get_dims()[0]) == this->get_size() / this->dims[0], "rest dimensions should be equal")

        int batch_size = idxs.size();
        int stride = this->get_size() / batch_size;
        auto dataset_ptr = dataset->get_data();
        for (int i = 0; i < batch_size; i++) {
            CHECK(idxs[i] < dataset_dims[0], "invalid index");
            for (int j = 0; j < stride; j++) {
                this->data[i*stride + j] = dataset_ptr[idxs[i]*stride + j];
            }
        }
        setclear();
    }

    template<typename T>
    void NArray<T>::copy_from(const vector<size_t> &idx0s, size_t idx1, const SP_NArray<T> dataset) {
        auto dataset_dims = dataset->get_dims();
        CHECK(idx0s.size() == this->dims[0], "first dimension should be equal to batch size");
        CHECK(dataset_dims.size() == this->dims.size()+1, "number of dimensions should be equal");
        for (size_t i = 2; i < this->dims.size(); i++) {
            CHECK(dataset_dims[i] == this->dims[i-1], "dimensions should be equal");
        }
        CHECK(idx1 >= 0 && idx1 < this->dims[1], "invalid index");

        int batch_size = this->dims[0];
        int stride = this->get_size() / batch_size;
        int dataset_stride = dataset->get_size() / dataset_dims[0];
        auto dataset_ptr = dataset->get_data();
        for (int i = 0; i < batch_size; i++) {
            CHECK(idx0s[i] >= 0 && idx0s[i] < dataset_dims[0], "invalid index");
            for (int j = 0; j < stride; j++) {
                this->data[i*stride + j] = dataset_ptr[idx0s[i]*dataset_stride + idx1*stride + j];
            }
        }
        setclear();
    }

    template<typename T>
    void NArray<T>::copy_from(const size_t start_from, const size_t copy_size, const SP_NArray<T> dataset) {
        auto dataset_dims = dataset->get_dims();
        CHECK(copy_size == this->dims[0], "the size of copy should be equal to batch size");
        CHECK(dataset_dims.size() == this->dims.size(), "number of dimensions should be equal");
        for (size_t i = 1; i < this->dims.size(); i++) {
            CHECK(dataset_dims[i] == this->dims[i], "dimensions should be equal");
        }
        CHECK(start_from >= 0 && start_from+copy_size-1 < dataset_dims[0], "offset is not valid");

        int batch_size = copy_size;
        int stride = this->get_size() / batch_size;
        auto dataset_ptr = dataset->get_data();
        for (int i = 0; i < batch_size; i++) {
            for (int j = 0; j < stride; j++) {
                this->data[i*stride + j] = dataset_ptr[(start_from+i)*stride + j];
            }
        }
        setclear();
    }


    template<typename T>
    void NArray<T>::normalize_for(int dim) {
        // currently, only two dimensional array are supported
        CHECK(dim == NARRAY_DIM_ZERO || dim == NARRAY_DIM_ONE, "only support upto 2 dimensional array");
        CHECK(this->dims.size() == 2, "only support upto 2 dimensional array");

        if (dim == NARRAY_DIM_ZERO) {
            for (size_t i = 0; i < this->dims[0]; i++) {
                T sum = 0;
                for (size_t j = 0; j < this->dims[1]; j++) {
                    sum += this->data[i*this->dims[1] + j];
                }
                for (size_t j = 0; j < this->dims[1]; j++) {
                    this->data[i*this->dims[1] + j] /= sum;
                }
            }
        }
        if (dim == NARRAY_DIM_ONE) {
            for (size_t i = 0; i < this->dims[1]; i++) {
                T sum = 0;
                for (size_t j = 0; j < this->dims[0]; j++) {
                    sum += this->data[j*this->dims[1] + i];
                }
                for (size_t j = 0; j < this->dims[1]; j++) {
                    this->data[i*this->dims[1] + j] /= sum;
                }
            }
        }
    }

    template class NArray<float>;
    template class NArray<double>;

}
