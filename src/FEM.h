#include "magic.h"
#include "GenMatrix.h"

class fem{
    private:
        int TotalNodeNumber;
        int TotalElementNumber;

        double** GaussianData;

        double** NodeData;
        int** ElementData;

        double SheildS;
        double ABSB;

        std::vector<double> vec_a;

        void Cal_Gravity(int i, double& Gx, double& Gy);
        double Cal_G(double sigma, double myux, double myuy, double x, double y);
        bool isPointInTriangle(int i, double x, double y);

        void Paraview_Bvector_Acontour(const char* fn, double* A, double* Bx, double* By);
        void Paraview_MaterialConfig(const char* fn);
        void Paraview_Bvector(const char* fn, double* Bx, double* By);

        void write_Json(string ind_name,double* A,double* Bx,double* By);
        void write_Weight(const char* fn,double* GaussianW);

        void CalTargetBB(double* Bx,double* By);
    
    public:
        fem();
        ~fem();
        void FEM(string ind_name,double* GaussianW);
        // void Paraview_MaterialConfig(const char* fn);

        void read_GaussianData(const char* fn_gaussian);
        void read_mesh1(const char* fn, double** node, int** element, int numnode, int numele);
        void CheckMaterial();
        void UpdateMaterial(double* GaussianW);

        void VisualWrapper(const std::string& ind_name,double* A, double* Bx, double* By);
        string generateFilePath_ver2(const string& dir_path,const string& ind_name,const string& in, const string& file_type);

        std::vector<double>& RetA();
        double RetS();
        int RetN();
        double RetABSB();
    };