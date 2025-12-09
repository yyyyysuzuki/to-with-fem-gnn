#include "magic.h"

class toolbox{
    private:

    public:
    void write_csv(const char* filename, int* x, int nummatrix);//int型一次元配列の書き出し(.csv)
    void write_csv(const char* filename, double* x, int nummatrix);//double型一次元配列の書き出し(.csv)
    void write_csv(const char* filename, int** x, int numrow, int numcolumn);//int型二次元配列の書き出し(.csv)
    void write_csv(const char* filename, double** x, int numrow, int numcolumn);//double型二次元配列の書き出し(.csv)
    void write_csv(const char* filename, vector<int> x, int nummatrix);//int型vectorの書き出し(.csv)
    void write_csv(const char* filename, vector<double> x, int nummatrix);//double型vectorの書き出し(.csv)

    void read_csv(const char* filename, int* x, int nummatrix);//int型一次元配列の読み込み(.csv)
    void read_csv(const char* filename, double* x, int nummatrix);//double型一次元配列の読み込み(.csv)
    void read_csv(const char* filename, int** x, int numrow, int numcolumn);//int型二次元配列の読み込み(.csv)
    void read_csv(const char* filename, double** x, int numrow, int numcolumn);//double型二次元配列の読み込み(.csv)

    void write_vtk(const char* filename, double** node, int** element, int* material, int numnode, int numele);//paraview用書き出し(材料分布)
    void write_vtk(const char* filename, double** node, int** mat_ele, int numnode, int numele);
    void write_vtk(const char* filename, double** node, int** element, double* potential, int numnode, int numele);//paraview用書き出し(コンター)
    void write_vtk(const char* filename, double** node, int** element, double* B_x, double* B_y, int numnode, int numele);//paraview用書き出し(ベクトル)
    void write_vtk_scalar(const char* filename, double** node, int** element, double* B, int numnode, int numele);//paraview用書き出し(スカラ)
    void write_vtk_vector(const char* filename, double** node, int** element, double* B_x, double* B_y, int numnode, int numele);//paraview(point, vector)

    void read_mesh1(const char* filename, double** node, int* material, int** element, int numnode, int numele);//メッシュデータの読み取り(.mesh1)

    void pol_to_rec(double r, double deg, double &x, double &y);//(r,θ[deg])から(x,y)へ変換(格納有)
    void pol_to_rec(double r, double deg);//(r,θ[deg])から(x,y)へ変換(格納無)
    void rec_to_pol(double x, double y, double &r, double &deg);//(x,y)から(r,θ[deg])へ変換(格納有)

    void CG_Method(double** K, double* b, double* a, int TotalNumber);//CG法実行

    void cout_method(int** K, int numrow, int numcolumn);//int型二次元配列を表示
    void cout_method(double** K, int numrow, int numcolumn);//double型二次元配列を表示
};