#include "magic.h"
#include "GenMatrix.h"


class nonlinear{
    private:
        int TotalNodeNumber;
        int TotalElementNumber;

        double** NodeData;
        int** ElementData;

        double** AkimaData;
        double** NyuData;

        double SheildS;

    public:
        nonlinear();
        ~nonlinear();
        void NonLinear(double** node, int** element, double* A, double* b, double* Bx, double* By);
        void StoreMesh(double** node, int** element);
        //void ReadNyuData(const char* AkimaFn, const char* NyuFn);
        void GetBoundNode(int NodeData, double** NodaData, vector<int>& BoundNode);
        void PrintArray();
        void CalB(double* A,double* Bx, double* By);
        void CalDelta(double* delta);
        void CalS(double (*S)[3][3], double* delta);

        double GetNyu(int i, int Material);
        double GetDNyuDBB(int i, int Material, double BB);
        double CalU(double (*S)[3][3], int i, int ele, double* A, int e[3]);
        double RetS();

        int Judge(double* DelA);
};