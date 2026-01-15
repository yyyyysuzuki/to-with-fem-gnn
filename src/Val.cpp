#include "FEM.h"
#include "NonLinear.h"
#include "setting.h"
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <chrono>
#include <tuple>

namespace py = pybind11;

py::array_t<double> Cal(bool reval, string pcfg, int seed,string ind_name,py::array_t<double> np_array){
    double* GaussianW = new double[GaussianDiv]();
    for(int i = 0; i < GaussianDiv; i++) {
        GaussianW[i] = np_array.mutable_at(i);
    }
    clock_t start = std::clock();
    fem FEM;
    FEM.FEM(reval,pcfg,seed,ind_name,GaussianW);
    std::clock_t end          = std::clock();
    double duration           = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::vector<double> vec_a = FEM.RetA();
    int N                     = FEM.RetN();
    py::array_t<double> result({N});
    auto r = result.mutable_unchecked<1>();

    for (int i = 0; i < N; i++){
        r(i) = 0;
        r(i) = vec_a[i];
    }
    
    delete[] GaussianW;

    return result;
}

PYBIND11_MODULE(Cal, m) {
    // 関数をPythonにバインディング
    m.def("Cal", &Cal);
}