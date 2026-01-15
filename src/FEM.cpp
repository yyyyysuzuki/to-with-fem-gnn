#include "FEM.h"
#include "NonLinear.h"
#include "toolbox.h"
#include <valarray>
#include <set>
#include "nlohmann/json.hpp"
#include "setting.h"
#include "fstream"

using json = nlohmann::json;

fem::fem() {
    TotalNodeNumber = totalNodeNumber;
    TotalElementNumber = totalElementNumber;

    GaussianData = new double*[GaussianDiv]();
    NodeData     = new double*[TotalNodeNumber]();
    ElementData  = new int*[TotalElementNumber]();

    SheildS = 0.0;
    ABSB    = 0.0;

    for(int i = 0; i< GaussianDiv; i++){
        GaussianData[i] = new double[3]();
    }
    for(int i = 0; i< TotalNodeNumber; i++){
        NodeData[i] = new double[3]();
    }
    for(int i = 0; i < TotalElementNumber; i++){
        ElementData[i] = new int[4]();
    }
}

fem::~fem() {
    for(int i = 0; i < GaussianDiv; i++){
        delete[] GaussianData[i];
    }
    delete[] GaussianData;
    for(int i = 0; i < TotalNodeNumber; i++){
        delete[] NodeData[i];
    }
    delete[] NodeData;
    for(int i = 0; i < TotalElementNumber; i++){
        delete[] ElementData[i];
    }
    delete[] ElementData;
}

void fem::FEM(bool reval,string pcfg,int seed,string ind_name,double* GaussianW) {
    //cout << "start ind--------------------------------------------" << endl;
    //cout << "   Ind name : " << ind_name << endl;
    read_mesh1(Msh1File,NodeData,ElementData,TotalNodeNumber,TotalElementNumber);
    read_GaussianData(GaussinaFile);
    //CheckMaterial();
    UpdateMaterial(GaussianW);
    //CheckMaterial();
    double* A    = new double[TotalNodeNumber]();
    double* b    = new double[TotalNodeNumber]();
    double* Bx   = new double[TotalElementNumber]();
    double* By   = new double[TotalElementNumber]();
    nonlinear nl;
    nl.NonLinear(NodeData, ElementData,A, b,  Bx, By);
    vec_a.assign(A, A + TotalNodeNumber);
    
    if (reval){
        SheildS = nl.RetS();
        CalTargetBB(Bx,By);
        double F = b_factor * ABSB + SheildS;
        std::ostringstream oss;
        oss << "../results/" << pcfg << "_" << seed << "/best.csv";
        std::string filename = oss.str();
        std::ofstream ofs(filename);
        ofs << "id,fitness,S,absb\n";
        ofs << ind_name << ","
            << F << ","
            << SheildS << ","
            << ABSB << "\n";
        std::ostringstream path0;
        path0 << "../results/" << pcfg << "_" << seed << "/reval.vtk";
        std::string filename0 = path0.str();
        Paraview_Bvector_Acontour(filename0.c_str(), A, Bx, By);
        std::ostringstream path1;
        path1 << "../results/" << pcfg << "_" << seed << "/material.vtk";
        std::string filename1 = path1.str();
        Paraview_MaterialConfig(filename1.c_str());
    }

    delete[] A;
    delete[] b;
    delete[] Bx;
    delete[] By;
}

std::vector<double>& fem::RetA(){
    return vec_a;
}

double fem::RetABSB(){
    return ABSB;
}

double fem::RetS(){
    return SheildS;
}

int fem::RetN(){
    return TotalNodeNumber;
}

void fem::CalTargetBB(double* Bx,double* By){
    vector<int> target = {2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2019,2020};
    ABSB = 0.0;
    for (int i : target){
        double BB = 0.0;
        BB = sqrt(Bx[i-1]*Bx[i-1]+By[i-1]*By[i-1]);
        cout << "BB : " << BB << endl;
        ABSB += BB;
        std::vector<std::string> header = {
            "id",
            "fitness",
            "S",
            "absb"
        };

    }
    cout << "ABSB : " << ABSB << endl;
}

void  fem::Paraview_MaterialConfig(const char* fn){
    FILE*  fp = fopen(fn,"w");
    if(fp == NULL) {
        cout << "error at opening " << fn << endl;
    }
    else{
        fprintf(fp, "# vtk DataFile Version 2.0\n");
        fprintf(fp, "Title Data\n");
        fprintf(fp, "ASCII\n");
        fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");
        fprintf(fp, "POINTS %d float\n",TotalNodeNumber);
        for(int i=0; i<TotalNodeNumber; i++) {
            fprintf(fp, "%.16lf %.16lf %.16lf\n",NodeData[i][0], NodeData[i][1], NodeData[i][2]);
        }
        fprintf(fp, "CELLS %d %d\n", TotalElementNumber, 4*TotalElementNumber);
         for(int i=0; i<TotalElementNumber; i++) {
            fprintf(fp, "%d ", 3);
            fprintf(fp, "%d %d %d\n",ElementData[i][1], ElementData[i][2], ElementData[i][3]);
        }
        fprintf(fp, "CELL_TYPES %d\n", TotalElementNumber);
        for(int i = 0; i < TotalElementNumber; i++) {
            fprintf(fp, "5\n");
        }
        fprintf(fp, "CELL_DATA %d\n", TotalElementNumber);
        fprintf(fp, "SCALARS Magnetic_Flux_Density int\n");
        fprintf(fp, "LOOKUP_TABLE default\n");
        for(int ele = 0; ele < TotalElementNumber; ele++) {
            if(ElementData[ele][0] == airNumber){
                //Air
                fprintf(fp, "%d\n", 0);
            }
            else if(ElementData[ele][0] == ironNumber){
                fprintf(fp, "%d\n", 1);
            }
            else if(ElementData[ele][0] == magnetNumber){
                fprintf(fp, "%d\n", 2);
            }
            else{
                fprintf(fp, "%d\n",3);
            }
        }

    }
    fclose(fp);
}

void fem::read_GaussianData(const char* fn_gaussian){
    FILE* fp = fopen(fn_gaussian,"r");
    double xx = 0.1;
    if(fp == NULL) {
        printf("GaussianDataが読み込めませんでした.\n");
    }
    else {
        for(int i = 0; i < GaussianDiv; i++) {
            fscanf(fp, "%lf,%lf,%lf", &GaussianData[i][0], &GaussianData[i][1], &GaussianData[i][2]);

        }
    }
    fclose(fp);
}

void fem::read_mesh1(const char* filename, double** node, int** element, int numnode, int numele){
    int n = 0;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    int tmp0 = 0;
    int tmp1 = 0;
    int m = 0;
    int tmp2 = 0;
    int a0 = 0;
    int a1 = 0;
    int a2 = 0;
    char buf[10];

    FILE* fp;
    fp = fopen(filename, "r");
    if(fp == NULL){
        cout << "fail open file in read_mesh1" << endl;
        exit(1);
    }

    //2行読み飛ばし
    fgets(buf, sizeof(buf), fp);
    fscanf(fp, "%d\n", &n);

    for(int i=0;i<numnode;i++){
        if(fscanf(fp, "%d %lf %lf %lf\n", &n, &x, &y, &z) >0){
            node[i][0] = x;
            node[i][1] = y;
            node[i][2] = z;
        }
    }

    //3行読み飛ばし
    fgets(buf, sizeof(buf), fp);
    fgets(buf, sizeof(buf), fp);
    fscanf(fp, "%d\n", &n);
    int coiln = 0;
    int airn = 0;
    for(int i=0;i<numele;i++){
        if(fscanf(fp, "%d %d %d %d %d %d %d %d\n", &n, &tmp0, &m, &tmp1, &tmp2, &a0, &a1, &a2) >0){
            element[i][0] = m;
            element[i][1] = a0-1;
            element[i][2] = a1-1;
            element[i][3] = a2-1;
            if(element[i][0] == 1){
                coiln += 1;
            }
            else{
                airn += 1;
            }
        }
    }
    //cout << "coiln  " << coiln << "airn  " << airn << endl;
    fclose(fp);
}

void fem::CheckMaterial(){
    int AirNum    = 0;
    int IronNum   = 0;
    int CoilNum   = 0;
    int TmpNum    = 0;

    for(int i = 0; i  < TotalElementNumber; i++){
        int MaterialNum = ElementData[i][0];
        
        if(MaterialNum == airNumber){
            AirNum ++;
        }
        else if(MaterialNum == ironNumber){
            IronNum ++;
        }
        else if(MaterialNum == coilNumber){
            CoilNum ++;
        }
        else{
            TmpNum ++;
        }
    }
    cout << "Material Info----------------------------------------" << endl;
    cout << "   Air     : " << AirNum << endl;
    cout << "   Iron    : " << IronNum << endl;
    cout << "   Coil    : " << CoilNum << endl;
    cout << "   etc     : " << TmpNum << endl;
    cout << "-----------------------------------------------------" << endl; 
}

void fem::UpdateMaterial(double* GaussianW){
    for (int i = 0; i < TotalElementNumber; i++) {
        double SigmaG  = 0.0;
        double f       = 0.0;
        double Gx      = 0.0;
        double Gy      = 0.0;

        Cal_Gravity(i, Gx , Gy);

        if ((Gx > 0 && Gx < p5_x && Gy > p8_y && Gy < p6_y)||(Gx > p7_x && Gx < p4_x && Gy > p7_y && Gy < p8_y)){
            for (int j = 0; j < GaussianDiv; j++) {
                SigmaG += Cal_G(GaussianData[j][2], GaussianData[j][0], GaussianData[j][1], Gx, Gy);
            }
            for (int j = 0; j < GaussianDiv; j++) {
                f += GaussianW[j] * Cal_G(GaussianData[j][2], GaussianData[j][0], GaussianData[j][1], Gx, Gy) / SigmaG ;
            }
            //cout << f << endl;
            
            if((f >= 0.0)) {
                ElementData[i][0] = ironNumber;
            }
            else if((f < 0.0)){
                ElementData[i][0] = airNumber;
            }
        }
    }
}

string fem::generateFilePath_ver2(const std::string& dir_path, const std::string& ind_name, const std::string& in, const std::string& file_type) {
    return dir_path + ind_name + "_" + in + "." + file_type;
}

bool fem::isPointInTriangle(int i, double x, double y) {
    // 三角形の頂点座標
    double x1 = NodeData[ElementData[i][1]][0]; 
    double y1 = NodeData[ElementData[i][1]][1];
    double x2 = NodeData[ElementData[i][2]][0];
    double y2 = NodeData[ElementData[i][2]][1];
    double x3 = NodeData[ElementData[i][3]][0];
    double y3 = NodeData[ElementData[i][3]][1];

    // バリセンター座標の分母
    double denom = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);

    // 各バリセンター座標の計算
    double lambda1 = ((y2 - y3) * (x - x3) + (x3 - x2) * (y - y3)) / denom;
    double lambda2 = ((y3 - y1) * (x - x3) + (x1 - x3) * (y - y3)) / denom;
    double lambda3 = 1.0 - lambda1 - lambda2;

    // バリセンター座標がすべて0以上か確認
    return (lambda1 >= 0) && (lambda2 >= 0) && (lambda3 >= 0);
}

double fem::Cal_G(double sigma , double myux, double myuy, double x, double y) {
    return 1/(2 * PI * sigma)*exp(-pow(sqrt(pow(x-myux,2) + pow(y-myuy,2)), 2)/(2*pow(sigma,2)));
}

void fem::Cal_Gravity(int i, double& Gx, double& Gy){
    int node1       = ElementData[i][1];
    int node2       = ElementData[i][2];
    int node3       = ElementData[i][3];
    Gx = (NodeData[node1][0] + NodeData[node2][0] + NodeData[node3][0]) / 3;
    Gy = (NodeData[node1][1] + NodeData[node2][1] + NodeData[node3][1]) / 3;
}

void fem::VisualWrapper(const std::string& ind_name,double* A, double* Bx, double* By){
    string path1 = generateFilePath_ver2("../result/",ind_name,"Bvector_contour","vtk");
    string path2 = generateFilePath_ver2("../result/",ind_name,"MaterialConfig","vtk");
    //string path3 = generateFilePath_ver2("./CalRes/",ind_name,"Bvector","vtk");
    // string path4 = generatePath("./DebRes/BvectorComp.vtk",wt);
    // string path5 = generatePath("./CalRes/answer.csv",wt);
    // string path6 = generatePath("./CalRes/A_contor.csv",wt);
    // string path7 = generatePath("./CalRes/A.csv",wt);

    Paraview_Bvector_Acontour(path1.c_str(), A, Bx, By);
    Paraview_MaterialConfig(path2.c_str());
    //Paraview_Bvector(path3.c_str(), Bx, By);
    //Paraview_Bvector_Comp(path4.c_str(), Ln_BxBy_Iwata, Bx, By);
    //GraphR_vector(path5.c_str(), Bx, By);
    //A_contor(path6.c_str(), A);
    //CSV_A(path7.c_str(), A);
}

void fem::Paraview_Bvector_Acontour(const char* fn, double* A, double* Bx, double* By) { //Aの等高線
    FILE* fp = fopen(fn, "w"); 
    if(fp == NULL) {
        cout << "error at opening" << fn << endl;
    }
    else{
        fprintf(fp,"# vtk DataFile Version 2.0\n");
        fprintf(fp, "Title Data\n");
        fprintf(fp, "ASCII\n");
        fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");
        fprintf(fp, "POINTS %d float\n",TotalNodeNumber);
        for(int i=0; i<TotalNodeNumber;i++) {
             fprintf(fp, "%.16lf %.16lf %.16lf\n",NodeData[i][0], NodeData[i][1], NodeData[i][2]);
        }
        fprintf(fp, "CELLS %d %d \n", TotalElementNumber, 4*TotalElementNumber);
        for(int i=0; i<TotalElementNumber;i++) {
            fprintf(fp, "%d ", 3);
            fprintf(fp, "%d %d %d\n",ElementData[i][1], ElementData[i][2], ElementData[i][3]);
        }
        fprintf(fp, "CELL_TYPES %d\n", TotalElementNumber);
        for(int i = 0; i < TotalElementNumber; i++){
            fprintf(fp, "5\n");
        }

        fprintf(fp, "POINT_DATA %d\n", TotalNodeNumber);
        fprintf(fp, "SCALARS A float\n");
        fprintf(fp, "LOOKUP_TABLE default\n");
        for(int i=0; i<TotalNodeNumber;i++) {
            fprintf(fp, "%.16lf\n", A[i]);
        }

        fprintf(fp, "CELL_DATA %d\n", TotalElementNumber);
        fprintf(fp, "VECTORS BxBy float\n");
        for(int ele = 0; ele < TotalElementNumber; ele++){
            fprintf(fp, "%.16lf %.16lf %.16lf\n", Bx[ele], By[ele], 0.0);
        }
    }
    fclose(fp);
}

void fem::Paraview_Bvector(const char* fn, double* Bx, double* By){
    FILE*  fp = fopen(fn,"w");
    if(fp == NULL) {
        cout << "error at opening " << fn << endl;
    }
    else{
        fprintf(fp, "# vtk DataFile Version 2.0\n");
        fprintf(fp, "Title Data\n");
        fprintf(fp, "ASCII\n");
        fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");
        fprintf(fp, "POINTS %d float\n",TotalNodeNumber);
        for(int i=0; i<TotalNodeNumber; i++) {
            fprintf(fp, "%.16lf %.16lf %.16lf\n",NodeData[i][0], NodeData[i][1], NodeData[i][2]);
        }
        fprintf(fp, "CELLS %d %d\n", TotalElementNumber, 4*TotalElementNumber);
        for(int i=0; i<TotalElementNumber; i++) {
            fprintf(fp, "%d ", 3);
            fprintf(fp, "%d %d %d\n",ElementData[i][1], ElementData[i][2], ElementData[i][3]);
        }
        fprintf(fp, "CELL_TYPES %d\n", TotalElementNumber);
        for(int i = 0; i < TotalElementNumber; i++) {
            fprintf(fp, "5\n");
        }
        fprintf(fp, "CELL_DATA %d\n", TotalElementNumber);
        fprintf(fp, "VECTORS BxBy float\n");
        for(int ele = 0; ele < TotalElementNumber; ele++) {
            fprintf(fp, "%.16lf %.16lf %.16lf\n",Bx[ele], By[ele], 0.0);
        }
    }
    fclose(fp);
}

void fem::write_Weight(const char* fn,double* GaussianW){
    FILE* fp = fopen(fn, "w");
    if(fp == NULL) {
        cout << fn << " が開けませんでした" << endl;
    }
    else {
        for(int i = 0; i < GaussianDiv; i++){
            fprintf(fp, "%.16lf\n",GaussianW[i]);
        }
        cout << fn << " に書き込みました" << endl;
    }
    fclose(fp);
}

void fem::write_Json(string ind_name, double* A, double* Bx, double*By){

    vector<vector<int>> elementVector(TotalElementNumber,vector<int>(4));
    vector<vector<double>> nodeVector(TotalNodeNumber,vector<double>(6));
    for(int i = 0;i < TotalElementNumber;i++){
        for(int j = 0;j < 4;j++){
            elementVector[i][j] = ElementData[i][j];
        }
    }
    for(int i = 0; i < TotalNodeNumber;i++){
        for(int j = 0; j < 3;j++){
            nodeVector[i][j] = NodeData[i][j];
            nodeVector[i][3] = A[i];
            nodeVector[i][4] = Bx[i];
            nodeVector[i][5] = By[i]; 
        }
    }

    json data;
    data["Element"]    = elementVector;
    data["Node"]       = nodeVector;

    string filename = generateFilePath_ver2("../result/", ind_name, "mesh_A_Bx_By" , "json");
    ofstream outFile(filename);
    if(outFile.is_open()) {
        outFile << data.dump(4);
        outFile.close();
        cout << filename << "に書き込みました" << endl;
    }else {
        cout << "JSONファイルを開けませんでした" << endl;
    }

}
