#ifndef _GALOIS_NARRAY_H_
#define _GALOIS_NARRAY_H_


#include <vector>

using namespace std;

namespace gs
{
    
    template<typename T>
    class NArray
    {
    public:
        NArray(int);
        NArray(int, int);
        NArray(int, int, int);
        NArray(int, int, int, int);
        NArray(initializer_list<int>);
        NArray(vector<int>);
        NArray(const NArray& other) = delete;
        NArray& operator=(const NArray&) = delete;
        ~NArray();
        
        vector<int> get_dims() { return dims; }
        int get_size() { return size; }
        
    private:
        vector<int> dims;
        int size;
        T *data;
    };
    template<typename T>
    using SP_NArray = shared_ptr<NArray<T>>;

}

#endif
