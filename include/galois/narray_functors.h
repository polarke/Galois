#ifndef _GALOIS_NARRAY_FUNCTORS_H_
#define _GALOIS_NARRAY_FUNCTORS_H_

#include <cassert>
#include <vector>
#if defined __APPLE__ && __MACH__
#include <Accelerate/Accelerate.h>
#elif defined __linux__
#include <cblas.h>
#else
// system not supported
#endif

namespace gs
{

    template<typename T>
    int COUNT_EQUAL(const SP_NArray<T> X, const SP_NArray<T> Y) {
        CHECK(X != nullptr && Y != nullptr, "X and Y should not be null");
        CHECK(!X->opaque() && !Y->opaque(), "X and Y should not be opaque");
        auto X_dims = X->get_dims();
        auto Y_dims = Y->get_dims();
        CHECK(X_dims == Y_dims, "dims should match");

        int equal = 0;
        auto X_ptr = X->get_data();
        auto Y_ptr = Y->get_data();
        for (size_t i = 0; i < X->get_size(); i++) {
            if (X_ptr[i] == Y_ptr[i]) {
                equal += 1;
            }
        }
        return equal;
    }

    template<typename T>
    void SUM_POSITIVE_VALUE (T *res, const SP_NArray<T> A) {
        T sum = 0;
        auto A_ptr = A->get_data();
        auto A_size = A->get_size();
        for (int i = 0; i < A_size; i++) {
            sum += A_ptr[i];
        }
        *res = sum;
    }

    // currently, only two dimensional array are supported
    // b[n] +> Y[m,n]
    template<typename T>
    void ADD_TO_ROW (const SP_NArray<T> Y, const SP_NArray<T> b) {
        auto Y_dims = Y->get_dims();
        auto b_dims = b->get_dims();
        assert(Y_dims.size() == 2);
        assert(b_dims.size() == 1);
        assert(Y_dims[1] == b_dims[0]);

        auto m = Y_dims[0];
        auto n = Y_dims[1];
        auto Y_ptr = Y->get_data();
        auto b_ptr = b->get_data();
        if (Y->opaque()) {
            for (size_t i = 0; i < m; i++) {
                for (size_t j = 0; j < n; j++) {
                    Y_ptr[i*n+j] = b_ptr[j];
                }
            }
            Y->setclear();
        } else {
            for (size_t i = 0; i < m; i++) {
                for (size_t j = 0; j < n; j++) {
                    Y_ptr[i*n+j] += b_ptr[j];
                }
            }
        }
    }

    // currently, only two dimensional array are supported
    // Y[m,n] +> b[n]
    template<typename T>
    void SUM_TO_ROW (const SP_NArray<T> b, const SP_NArray<T> X) {
        auto b_dims = b->get_dims();
        auto X_dims = X->get_dims();
        assert(b_dims.size() == 1);
        assert(X_dims.size() == 2);
        assert(b_dims[0] == X_dims[1]);

        auto m = X_dims[0];
        auto n = X_dims[1];
        auto X_ptr = X->get_data();
        auto b_ptr = b->get_data();
        if (b->opaque()) {
            for (size_t j = 0; j < n; j++) {
                b_ptr[j] = X_ptr[j];
            }
            b->setclear();
        } else {
            for (size_t j = 0; j < n; j++) {
                b_ptr[j] += X_ptr[j];
            }
        }
        for (size_t i = 1; i < m; i++) {
            for (size_t j = 0; j < n; j++) {
                b_ptr[j] += X_ptr[i*n + j];
            }
        }
    }

    // currently, only two dimensional array are supported
    // X[m,n] -> Y[m]
    template<typename T>
    void MAXIDX_EACH_ROW(const SP_NArray<T> Y, const SP_NArray<T> X) {
        CHECK(Y->opaque(), "Y should be opaque (not set before)");
        auto X_dims = X->get_dims();
        auto Y_dims = Y->get_dims();
        assert(X_dims.size() == 2 && Y_dims.size() == 1);

        auto m = X_dims[0];
        auto n = X_dims[1];
        assert(m == Y_dims[0]);
        auto X_ptr = X->get_data();
        auto Y_ptr = Y->get_data();
        for (size_t i = 0; i < m; i++) {
            int maxidx = 0;
            T maxval = X_ptr[i*n+0];
            for (size_t j = 1; j < n; j++) {
                auto val = X_ptr[i*n+j];
                if (val > maxval) {
                    maxidx = j;
                    maxval = val;
                }
            }
            Y_ptr[i] = T(maxidx);
        }
        Y->setclear();
    }

    // currently, only two dimensional array are supported
    // X[m,n] -> Y[k,n] with k <= m
    template<typename T>
    void TAKE_ROWS(const SP_NArray<T> Y, const SP_NArray<T> indexs, const SP_NArray<T> X) {
        auto X_dims = X->get_dims();
        auto Y_dims = Y->get_dims();
        auto indexs_dims = indexs->get_dims();
        assert(X_dims.size() == 2 && Y_dims.size() == 2 && indexs_dims.size() == 1);

        auto m = X_dims[0];
        auto n = X_dims[1];
        assert(n == Y_dims[1]);
        auto k = indexs_dims[0];
        assert(k == Y_dims[0]);
        auto X_ptr = X->get_data();
        auto Y_ptr = Y->get_data();
        auto indexs_ptr = indexs->get_data();

        if (Y->opaque()) {
            for (size_t i = 0; i < k; i++) {
                size_t idx = size_t(indexs_ptr[i]);
                assert(idx >= 0 && idx < m);
                for (size_t j = 0; j < n; j++) {
                    Y_ptr[i*n+j] = X_ptr[idx*n+j];
                }
            }
            Y->setclear();
        } else {
            for (size_t i = 0; i < k; i++) {
                size_t idx = size_t(indexs_ptr[i]);
                assert(idx >= 0 && idx < m);
                for (size_t j = 0; j < n; j++) {
                    Y_ptr[i*n+j] += X_ptr[idx*n+j];
                }
            }
        }
    }

    // currently, only two dimensional array are supported
    // Y[k,n] -> X[m,n] with k <= m
    template<typename T>
    void PUT_ROWS(const SP_NArray<T> X, const SP_NArray<T> indexs, const SP_NArray<T> Y) {
        auto X_dims = X->get_dims();
        auto Y_dims = Y->get_dims();
        auto indexs_dims = indexs->get_dims();
        assert(X_dims.size() == 2 && Y_dims.size() == 2 && indexs_dims.size() == 1);

        auto m = X_dims[0];
        auto n = X_dims[1];
        assert(n == Y_dims[1]);
        auto k = indexs_dims[0];
        assert(k == Y_dims[0]);
        auto X_ptr = X->get_data();
        auto Y_ptr = Y->get_data();
        auto indexs_ptr = indexs->get_data();

        if (X->opaque()) {
            X->fill(T(0.0));
            X->setclear();
        }
        for (size_t i = 0; i < k; i++) {
            size_t idx = size_t(indexs_ptr[i]);
            assert(idx >= 0 && idx < m);
            for (size_t j = 0; j < n; j++) {
                X_ptr[idx*n+j] += Y_ptr[i*n+j];
            }
        }
    }

    inline void _GEMM(const CBLAS_ORDER _order,
                      const CBLAS_TRANSPOSE _tranA, const CBLAS_TRANSPOSE _tranB,
                      const int _M, const int _N, const int _K,
                      const float _alpha,
                      const float *_A, const int _lda,
                      const float *_B, const int _ldb,
                      const float _beta,
                      float *_C, const int _ldc) {
        cblas_sgemm(_order, _tranA, _tranB, _M, _N, _K, _alpha, _A, _lda, _B, _ldb, _beta, _C, _ldc);
    }

    inline void _GEMM(const CBLAS_ORDER _order,
                      const CBLAS_TRANSPOSE _tranA, const CBLAS_TRANSPOSE _tranB,
                      const int _M, const int _N, const int _K,
                      const double _alpha,
                      const double *_A, const int _lda,
                      const double *_B, const int _ldb,
                      const double _beta,
                      double *_C, const int _ldc) {
        cblas_dgemm(_order, _tranA, _tranB, _M, _N, _K, _alpha, _A, _lda, _B, _ldb, _beta, _C, _ldc);
    }

    template<typename L>
    void GEMM (const char tA, const char tB,
               const L alpha, const SP_NArray<L> A, const SP_NArray<L> B,
               const L beta, const SP_NArray<L> C) {
        assert(tA == 'T' || tA == 'N');
        assert(tB == 'T' || tB == 'N');
        int A0 = A->get_dims()[0];
        int A1 = A->get_size() / A0;
        int B0 = B->get_dims()[0];
        int B1 = B->get_size() / B0;
        int C0 = C->get_dims()[0];
        int C1 = C->get_size() / C0;
        auto t_A = tA=='T' ? CblasTrans : CblasNoTrans;
        auto t_B = tB=='T' ? CblasTrans : CblasNoTrans;
        auto M   = tA=='T' ? A1 : A0;
        auto K_A = tA=='T' ? A0 : A1;
        auto K_B = tB=='T' ? B1 : B0;
        auto N   = tB=='T' ? B0 : B1;
        assert(K_A == K_B);
        assert(C0 == M && C1 == N);
        auto K   = K_A;
        auto lda = A1;
        auto ldb = B1;
        auto ldc = C1;
        _GEMM(CblasRowMajor,
              t_A, t_B,
              M, N, K,
              alpha,
              A->get_data(), lda,
              B->get_data(), ldb,
              beta,
              C->get_data(), ldc);
    }

    template<typename L>
    void GEMM (const SP_NArray<L> Y,
               const char tA, const char tB,
               const SP_NArray<L> A, const SP_NArray<L> B) {
        if (Y->opaque()) {
            GEMM(tA, tB, static_cast<L>(1.0), A, B, static_cast<L>(0.0), Y);
            Y->setclear();
        } else {
            GEMM(tA, tB, static_cast<L>(1.0), A, B, static_cast<L>(1.0), Y);
        }
    }

    template<typename T, typename FUNC>
    void _MAP (const SP_NArray<T> Y,
               const FUNC& f,
               const SP_NArray<T> X,
               const bool overwrite) {
        assert(Y->get_dims() == X->get_dims());
        auto Y_ptr = Y->get_data();
        auto X_ptr = X->get_data();
        if (overwrite) {
            for (size_t i = 0; i < Y->get_size(); i++) {
                Y_ptr[i] = f(X_ptr[i]);
            }
        } else {
            for (size_t i = 0; i < Y->get_size(); i++) {
                Y_ptr[i] += f(X_ptr[i]);
            }
        }
    }

    template<typename T, typename FUNC>
    void _MAP (const SP_NArray<T> Y,
               const FUNC& f,
               const SP_NArray<T> X, const SP_NArray<T> Z,
               const bool overwrite) {
        assert(Y->get_dims() == X->get_dims());
        assert(Y->get_dims() == Z->get_dims());
        auto Y_ptr = Y->get_data();
        auto X_ptr = X->get_data();
        auto Z_ptr = Z->get_data();
        if (overwrite) {
            for (size_t i = 0; i < Y->get_size(); i++) {
                Y_ptr[i] = f(X_ptr[i], Z_ptr[i]);
            }
        } else {
            for (size_t i = 0; i < Y->get_size(); i++) {
                Y_ptr[i] += f(X_ptr[i], Z_ptr[i]);
            }
        }
    }

    template<typename T, typename FUNC>
    void MAP (const SP_NArray<T> Y, const FUNC& f, const SP_NArray<T> X) {
        if (Y->opaque()) {
            _MAP(Y, f, X, true);
            Y->setclear();
        } else {
            _MAP(Y, f, X, false);
        }
    }

    template<typename T, typename FUNC>
    void MAP (const SP_NArray<T> Y,
              const FUNC& f,
              const SP_NArray<T> X, const SP_NArray<T> Z) {
        if (Y->opaque()) {
            _MAP(Y, f, X, Z, true);
            Y->setclear();
        } else {
            _MAP(Y, f, X, Z, false);
        }
    }

    // currently, only two dimensional array are supported
    // X[m][n] -> Y[m]
    template<typename T, typename FUNC>
    void _PROJ_MAP (const SP_NArray<T> Y,
                    const FUNC& f,
                    const SP_NArray<T> X,
                    const SP_NArray<T> idx,
                    const bool overwrite) {
        assert(X->get_dims().size() == 2);
        assert(Y->get_dims().size() == 1);
        int m = X->get_dims()[0];
        int n = X->get_dims()[1];
        assert(m == Y->get_dims()[0]);
        assert(idx->get_dims().size() == 1);
        assert(m == idx->get_dims()[0]);

        auto Y_ptr = Y->get_data();
        auto X_ptr = X->get_data();
        auto idx_ptr = idx->get_data();
        if (overwrite) {
            for (int i = 0; i < m; i++) {
                int j = idx_ptr[i];
                assert(j < n);
                Y_ptr[i] = f(X_ptr[i*n + j]);
            }
        } else {
            for (int i = 0; i < m; i++) {
                int j = idx_ptr[i];
                assert(j < n);
                Y_ptr[i] += f(X_ptr[i*n + j]);
            }
        }
    }

    // currently, only two dimensional array are supported
    // X[m][n] -> Y[m]
    template<typename T, typename FUNC>
    void PROJ_MAP (const SP_NArray<T> Y,
                   const FUNC& f,
                   const SP_NArray<T> X,
                   const SP_NArray<T> idx) {
        if (Y->opaque()) {
            _PROJ_MAP(Y, f, X, idx, true);
            Y->setclear();
        } else {
            _PROJ_MAP(Y, f, X, idx, false);
        }
    }

    // currently, only two dimensional array are supported
    // X[m][n] -> Y[m] -> T
    template<typename T, typename FUNC>
    void PROJ_MAP_SUM (T *res,
                       const FUNC& f,
                       const SP_NArray<T> X,
                       const SP_NArray<T> idx) {
        assert(X->get_dims().size() == 2);
        auto m = X->get_dims()[0];
        auto n = X->get_dims()[1];
        assert(idx->get_dims().size() == 1);
        assert(m == idx->get_dims()[0]);

        T sum = 0;
        auto X_ptr = X->get_data();
        auto idx_ptr = idx->get_data();
        for (size_t i = 0; i < m; i++) {
            size_t j = idx_ptr[i];
            assert(j < n);
            sum += f(X_ptr[i*n + j]);
        }
        *res = sum;
    }

    // currently, only two dimensional array are supported
    // to be fixed
    template<typename T, typename FUNC>
    void _SUB_MAP (const SP_NArray<T> Y,
                   const FUNC& f,
                   const SP_NArray<T> X,
                   const SP_NArray<T> a, const SP_NArray<T> b,
                   const bool overwrite) {
        assert(Y->get_dims() == X->get_dims());
        assert(Y->get_dims().size() == 2);
        auto m = Y->get_dims()[0];
        auto n = Y->get_dims()[1];
        auto Y_ptr = Y->get_data();
        auto X_ptr = X->get_data();

        if (a == nullptr) {
            if (b == nullptr) {
                assert(m == n);
                if (overwrite) {
                    for (size_t i = 0; i < m; i++) {
                        Y_ptr[i*n + i] = f(X_ptr[i*n + i]);
                    }
                } else {
                    for (size_t i = 0; i < m; i++) {
                        Y_ptr[i*n + i] += f(X_ptr[i*n + i]);
                    }
                }
            } else {
                assert(b->get_dims().size() == 1);
                assert(b->get_size() == m);
                auto b_ptr = b->get_data();
                if (overwrite) {
                    for (size_t i = 0; i < m; i++) {
                        auto j = static_cast<int>(b_ptr[i]);
                        Y_ptr[i*n + j] = f(X_ptr[i*n + j]);
                    }
                } else {
                    for (size_t i = 0; i < m; i++) {
                        auto j = static_cast<int>(b_ptr[i]);
                        Y_ptr[i*n + j] += f(X_ptr[i*n + j]);
                    }
                }
            }
        } else {
            assert(a->get_dims().size() == 1);
            auto a_ptr = a->get_data();
            if (b == nullptr) {
                assert(a->get_size() == n);
                if (overwrite) {
                    for (size_t j = 0; j < n; j++) {
                        auto i = static_cast<int>(a_ptr[j]);
                        Y_ptr[i*n + j] = f(X_ptr[i*n + j]);
                    }
                } else {
                    for (size_t j = 0; j < n; j++) {
                        auto i = static_cast<int>(a_ptr[j]);
                        Y_ptr[i*n + j] += f(X_ptr[i*n + j]);
                    }
                }
            } else {
                assert(b->get_dims().size() == 1);
                assert(a->get_size() == b->get_size());
                auto size = a->get_size();
                auto b_ptr = b->get_data();
                if (overwrite) {
                    for (size_t k = 0; k < size; k++) {
                        auto i = static_cast<int>(a_ptr[k]);
                        auto j = static_cast<int>(b_ptr[k]);
                        Y_ptr[i*n + j] = f(X_ptr[i*n + j]);
                    }
                } else {
                    for (size_t k = 0; k < size; k++) {
                        auto i = static_cast<int>(a_ptr[k]);
                        auto j = static_cast<int>(b_ptr[k]);
                        Y_ptr[i*n + j] += f(X_ptr[i*n + j]);
                    }
                }
            }
        }
    }

    // currently, only two dimensional array are supported
    template<typename T, typename FUNC>
    void SUB_MAP (const SP_NArray<T> Y,
                  const FUNC& f,
                  const SP_NArray<T> X,
                  const SP_NArray<T> a, const SP_NArray<T> b) {
//        if (Y->opaque()) {
//            _SUB_MAP(Y, f, X, a, b, true);
//            Y->setclear();
//        } else {
//            _SUB_MAP(Y, f, X, a, b, false);
//        }
        CHECK(!Y->opaque(), "Submap currently only supports non-opaque output array");
        _SUB_MAP(Y, f, X, a, b, false);
    }

}

#endif