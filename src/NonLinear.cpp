#include "NonLinear.h"
#include <valarray>
#include "setting.h"
#include "GenMatrix.h"

nonlinear::nonlinear() {
    TotalNodeNumber = totalNodeNumber;
    TotalElementNumber = totalElementNumber;

    NodeData = new double*[TotalNodeNumber]();
    ElementData = new int*[TotalElementNumber]();
    // AkimaData = new double*[NyuNum]();
    // NyuData   = new double*[NyuNum]();
    for(int i = 0; i< TotalNodeNumber; i++){
        NodeData[i] = new double[3]();
    }
    for(int i = 0; i < TotalElementNumber; i++){
        ElementData[i] = new int[4]();
    }
    // for(int i = 0; i < NyuNum; i++){
    //     AkimaData[i] = new double[4]();
    //     NyuData[i] = new double[2]();
    // }
    SheildS = 0.0;
}

nonlinear::~nonlinear() {
    for(int i = 0; i < TotalNodeNumber; i++){
        delete[] NodeData[i];
    }
    delete[] NodeData;
    for(int i = 0; i < TotalElementNumber; i++){
        delete[] ElementData[i];
    }
    delete[] ElementData;
    // for(int i = 0; i < NyuNum; i++){
    //     delete[] AkimaData[i];
    //     delete[] NyuData[i];
    // }
    // delete[] AkimaData;
    // delete[] NyuData;
}

void nonlinear::NonLinear(double** node, int** element, double* A, double* b, double* Bx, double* By){
    //cout << "start nonlinear::NonLinear-----------------------------------------------" << endl;
    double* DelA   = new double[TotalNodeNumber]();
    double* BB     = new double[TotalElementNumber]();
    double* delta = new double[TotalElementNumber]();
    double (*S)[3][3] = new double[TotalElementNumber][3][3];

    for (int i=0; i < TotalElementNumber; i++){
        BB[i] = 0.0;
    }

    StoreMesh(node,element);
    //ReadNyuData(AkimaFile,NyuFile);
    
    std::vector<int> BoundNode;
    GetBoundNode(TotalNodeNumber, NodeData, BoundNode);
    //cout <<  "固定境界接点数 : " << BoundNode.size() << endl;

    CalDelta(delta);
    CalS(S,delta);

    int count= 0;
    for(int i = 0;i < TotalElementNumber; i++){
        if (ElementData[i][0] == ironNumber){
            SheildS += delta[i];
            count += 1;
        }
    }
    // cout << "count : " << count << endl;
    // cout << "S : " << SheildS << endl;

    //NR法//////////////////////////////////////////////////////////////////////////////////////
    for(int i = 0; i < MaxNrLoop; i++) {
        //cout << "NrLoop :: " << i << endl;
        GenMatrix H(TotalNodeNumber);
        GenMatrix L(H.getNumRow());
        
        for (int i = 0; i < TotalNodeNumber; i++){
            b[i] = 0.0;
        }
        for (int i=0; i < TotalNodeNumber; i++){
            DelA[i] = 0.0;
        }
        for (int i = 0; i < TotalElementNumber; i++) {
            Bx[i] = 0.0;
            By[i] = 0.0;
        }

        int coilelementnum = 0;
        for(int ele = 0; ele < TotalElementNumber; ele++){
            int Material = ElementData[ele][0];
            int e[3] = {ElementData[ele][1], ElementData[ele][2], ElementData[ele][3]};
            
            double Nyu = GetNyu(i,Material);

            if(Material == coilNumber){
                coilelementnum += 1;
                for (int i = 0; i < 3; i++) {
                    b[e[i]] += delta[ele] * Jo / 3.0;
                }
            }

            for(int i = 0; i < 3; i++) {
                for(int j = 0; j < 3; j++){
                    H.add(e[i], e[j], ((Nyu * S[ele][i][j])));
                }
            }
        } 
        
        // cout << "coilelementnum" << coilnodenum << endl;
        H.setBoundaryCondition(b, BoundNode, 0.0);
        double Norm = 0.;
        for(int i = 0; i < TotalNodeNumber; i++) {
            Norm += b[i]*b[i];
        }
        
        double Bnorm      = sqrt(Norm);
        double ICCG_conv  = iccg_conv;
        double* ICCG_r    = new double[TotalNodeNumber]();

        GenMatrix::icdcmp(H, L, 1.05);
        
        GenMatrix::iccgSolv(H, L, b, DelA, ICCG_r, Bnorm*ICCG_conv, H.getNumRow());
        delete[] ICCG_r;

        for (int i=0; i < TotalNodeNumber; i++) {
            A[i] += DelA[i]; 
        }

        CalB(A, Bx, By);
        
        double tmp1 = 0;
        for (int i = 0; i < TotalNodeNumber; i++) {
            if (sqrt(DelA[i]*DelA[i]) > tmp1) {
                tmp1 = sqrt(DelA[i]*DelA[i]);
            }
        }
        //cout << "DelA Max  :: " << tmp1 << endl;

        double tmp = 0;
        for (int i = 0; i < TotalNodeNumber; i++) {
            if (A[i] > tmp) {
                tmp = A[i];
            }
        }
        //cout << "A Max     :: " << tmp << endl;

        for (int i=0; i < TotalElementNumber; i++) {
            BB[i] = Bx[i]*Bx[i] + By[i]*By[i];
        }
        if(Judge(DelA) == 1){
            //cout << Judge(DelA);
            break;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    delete[] BB;
    delete[] S;
    delete[] delta;

    //cout << "end nonlinear::NonLinear-----------------------------------------------" << endl;
}

double nonlinear::RetS(){
    return SheildS;
}

void nonlinear::StoreMesh(double** node, int** element) {
    for(int i = 0; i < totalNodeNumber; i++) {
        for(int j = 0; j < 3; j++) {
            NodeData[i][j] = node[i][j]; 
        }
    }
    for(int i = 0; i < totalElementNumber; i++) {
        for(int j = 0 ; j < 4; j++){
            ElementData[i][j] = element[i][j];
        }
    }
}

// void nonlinear::ReadNyuData(const char* AkimaFn, const char* NyuFn){
//     FILE* fp = fopen(AkimaFn,"r");
  
//     if(fp == NULL) {
//         cout << "AkimaFn が読み込めませんでした." << endl;
//     }
//     else {
//         for(int i = 0; i < NyuNum; i++) {
//             fscanf(fp, "%lf,%lf,%lf,%lf", &AkimaData[i][0], &AkimaData[i][1], &AkimaData[i][2], &AkimaData[i][3]);
//         }
//     }
//     fclose(fp);

//     fp = fopen(NyuFn, "r");
//     if(fp == NULL) {
//         cout << "NyuFn が読み込めませんでした." << endl;
//     }
//     else{
//         for(int i = 0; i < NyuNum; i++) {
//             fscanf(fp, "%lf,%lf", &NyuData[i][0], &NyuData[i][1]);
//         }
//     }
//     fclose(fp);
// }

void nonlinear::PrintArray(){
    cout << "print NodeData" << endl;
    for(int i = 0; i < 5; i++) { 
        cout << NodeData[i][0] << "," << NodeData[i][1] << "," << NodeData[i][2] << endl;
    }
    cout << endl;

    cout << "print ElementData" << endl;
    for(int i = 0; i < 5; i++) { 
        cout << ElementData[i][0] << "," << ElementData[i][1] << "," << ElementData[i][2] << "," << ElementData[i][3] << endl;
    }
    cout << endl;
}

void nonlinear::GetBoundNode(int totalnodenumber, double** node, vector<int>& BounNode){
    for(int i = 0; i < totalnodenumber; i++) {
        double x = node[i][0];
        double y = node[i][1];
        if(x < 0 + 1e-8 || x > 800*m - 1e-8 || y > 800*m - 1e-8) {
            BounNode.push_back(i);
        }
    }
}

void nonlinear::CalDelta(double* delta){
    for(int ele = 0; ele < TotalElementNumber; ele++) {
        int Material = ElementData[ele][0];
        int e[3] = {ElementData[ele][1], ElementData[ele][2], ElementData[ele][3]};

        double c[3] = {0, 0, 0};
        double d[3] = {0, 0, 0};
        for(int i = 0; i < 3; i++) {
            int j = (i+1) % 3;
            int k = (i+2) % 3;
            c[i] = NodeData[e[j]][1] - NodeData[e[k]][1];
            d[i] = NodeData[e[k]][0] - NodeData[e[j]][0];
        }

        delta[ele] = 0.5 * (c[0]*d[1] - c[1]*d[0]);
        if(delta[ele] < 0.0 || delta[ele] == 0.0) {
            printf("要素面積がゼロです.\n");
            exit(EXIT_FAILURE);
        }
    }
}

void nonlinear::CalS(double (*S)[3][3], double* delta){
    for(int ele = 0; ele < TotalElementNumber; ele++){
        // cout << ele << endl;
        
        int Material = ElementData[ele][0];
        int e[3] = {ElementData[ele][1], ElementData[ele][2], ElementData[ele][3]};

        double c[3] = {0, 0, 0};
        double d[3] = {0, 0, 0};

        for(int i = 0; i < 3; i++) {
            int j = (i+1) % 3;
            int k = (i+2) % 3;
            c[i] = NodeData[e[j]][1] - NodeData[e[k]][1];
            d[i] = NodeData[e[k]][0] - NodeData[e[j]][0];
        }
        
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                S[ele][i][j] = 0.25 * (c[i]*c[j] + d[i]*d[j]) / delta[ele];
            }
        }
    }
}

void nonlinear::CalB(double* A, double* Bx, double* By){
    for(int ele = 0; ele < TotalElementNumber; ele++){
        int Material = ElementData[ele][0];
        int e[3] = {ElementData[ele][1], ElementData[ele][2], ElementData[ele][3]};
        double c[3] = {0, 0, 0};
        double d[3] = {0, 0, 0};
        double delta = 0.;
        for(int i = 0; i < 3; i++ ){
            int j = (i + 1) % 3;
            int k = (i + 2) % 3;
            c[i] = NodeData[e[j]][1] - NodeData[e[k]][1];
            d[i] = NodeData[e[k]][0] - NodeData[e[j]][0];
        }
        delta = 0.5 * (c[0]*d[1] - c[1]*d[0]); //面積算出
        for(int i = 0; i < 3; i++) {
            Bx[ele] += 0.5*(d[i]*A[e[i]])/delta;
            By[ele] -= 0.5*(c[i]*A[e[i]])/delta;
        }
    }
}

double nonlinear::CalU(double (*S)[3][3], int i, int ele , double* A ,int e[3]){
    double U = 0;
    for (int k = 0; k < 3; k++) {
        U += S[ele][i][k] * A[e[k]];
    }
    return U;
}


double nonlinear::GetNyu(int i,int Material){
    if(Material == ironNumber) {
        return nyu_iron;
        // if (i == 0){
        //     return nyu_iron;
        // }
        // else{
        //     for(int i = 0; i < NyuNum; i++) {
        //         if((BB >= NyuData[i][0]) && (BB < NyuData[i+1][0])) {
        //             double del = BB - NyuData[i][0];
        //             double Nyu = AkimaData[i][0] + AkimaData[i][1] * del + AkimaData[i][2] * del * del + AkimaData[i][3] * del * del * del;
        //             return Nyu;
        //         }
        //     }
        //     return AkimaData[NyuNum-2][0] + AkimaData[NyuNum-2][1] * (BB - NyuData[NyuNum-1][0]);
        // }
    }
    else{
        return nyu_air;
    }
}

double nonlinear::GetDNyuDBB(int i, int Material, double BB){
    if(Material == ironNumber) {
        return 0;
        // if (i =0){
        //     return 0;
        // }
        // else{
        //     for(int i = 0; i < NyuNum; i++) {
        //         if((BB >= NyuData[i][0]) && (BB < NyuData[i+1][0])) {
        //             double del = BB - NyuData[i][0];
        //             return AkimaData[i][1] + 2 * AkimaData[i][2] * del + 3 * AkimaData[i][3] * del * del;
        //         }
        //     }
        //     return AkimaData[NyuNum-2][1];
        // }
    }
    else{
        return 0;
    }
}

int nonlinear::Judge(double* DelA){
    for(int j = 0; j < TotalNodeNumber; j++){
        if (sqrt(DelA[j] * DelA[j]) > NrConv){
            return 0;
        }
    }
    return 1;
}



