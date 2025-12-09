#include "toolbox.h"

//int型一次元配列の書き出し(.csv)
void toolbox::write_csv(const char* filename, int* x, int nummatrix){
    FILE* fp;
    fp = fopen(filename,"w");
    
    for(int i=0;i<nummatrix;i++){
        fprintf(fp ,"%d\n" , x[i]);
    }
    fclose(fp);
}

//double型一次元配列の書き出し(.csv)
void toolbox::write_csv(const char* filename, double* x, int nummatrix){
    FILE* fp;
    fp = fopen(filename,"w");
    
    for(int i=0;i<nummatrix;i++){
        fprintf(fp ,"%.16lf\n" , x[i]);
    }
    fclose(fp);
}

//int型二次元配列の書き出し(.csv)
void toolbox::write_csv(const char* filename, int** x, int numrow, int numcolumn){
    FILE* fp;
    fp = fopen(filename,"w");

    for(int i=0;i<numrow;i++){
        for(int j=0;j<numcolumn-1;j++){
            fprintf(fp ,"%d," , x[i][j]);
        }
        fprintf(fp ,"%d\n" , x[i][numcolumn-1]);
    }
    fclose(fp);
}

//double型二次元配列の書き出し(.csv)
void toolbox::write_csv(const char* filename, double** x, int numrow, int numcolumn){
    FILE* fp;
    fp = fopen(filename,"w");

    for(int i=0;i<numrow;i++){
        for(int j=0;j<numcolumn-1;j++){
            fprintf(fp ,"%.16lf," , x[i][j]);
        }
        fprintf(fp ,"%.16lf\n" , x[i][numcolumn-1]);
    }
    fclose(fp);
}

//int型vectorの書き出し(.csv)
void toolbox::write_csv(const char* filename, vector<int> x, int nummatrix){
    FILE* fp;
    fp = fopen(filename,"w");
    
    for(int i=0;i<nummatrix;i++){
        fprintf(fp ,"%d\n" , x[i]);
    }
    fclose(fp);
}

//double型vectorの書き出し(.csv)
void toolbox::write_csv(const char* filename, vector<double> x, int nummatrix){
    FILE* fp;
    fp = fopen(filename,"w");
    
    for(int i=0;i<nummatrix;i++){
        fprintf(fp ,"%.16lf\n" , x[i]);
    }
    fclose(fp);
}

//int型一次元配列の読み込み(.csv)
void toolbox::read_csv(const char* filename, int* x, int nummatrix){
    int tmp = 0;
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        cout << "fail open file in " << filename << endl;
        exit(1);
    }
    for(int i=0;i<nummatrix;i++){
        if(fscanf(fp, "%d", &tmp) >0){
            x[i] = tmp;
        }
    }
    fclose(fp);
}

//double型一次元配列の読み込み(.csv)
void toolbox::read_csv(const char* filename, double* x, int nummatrix){
    double tmp = 0.0;
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        cout << "fail open file in " << filename << endl;
        exit(1);
    }
    for(int i=0;i<nummatrix;i++){
        if(fscanf(fp, "%lf", &tmp) >0){
            x[i] = tmp;
        }
    }
    fclose(fp);
}

//int型二次元配列の読み取り(.csv)
void toolbox::read_csv(const char* filename, int** x, int numrow, int numcolumn){
    int tmp = 0;
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        cout << "fail open file in " << filename << endl;
        exit(1);
    }
    for(int i=0;i<numrow;i++){
        for(int j=0;j<numcolumn;j++){
            if(j<numcolumn-1){
                if(fscanf(fp, "%d,", &tmp) >0){
                    x[i][j] = tmp;
                }
            }
            else{
                if(fscanf(fp, "%d", &tmp) >0){
                    x[i][j] = tmp;
                }
            }
        }
    }
    fclose(fp);
}

//double型二次元配列の読み取り(.csv)
void toolbox::read_csv(const char* filename, double** x, int numrow, int numcolumn){
    double tmp = 0.0;
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        cout << "fail open file in " << filename << endl;
        exit(1);
    }
    for(int i=0;i<numrow;i++){
        for(int j=0;j<numcolumn;j++){
            if(j<numcolumn-1){
                if(fscanf(fp, "%lf,", &tmp) >0){
                    x[i][j] = tmp;
                }
            }
            else{
                if(fscanf(fp, "%lf", &tmp) >0){
                    x[i][j] = tmp;
                }
            }
        }
    }
    fclose(fp);
}

//paraview用書き出し(材料分布)
void toolbox::write_vtk(const char* filename, double** node, int** element, int* material, int numnode, int numele){
    FILE* fp;
    fp = fopen(filename,"w");
    fprintf(fp ,"# vtk DataFile Version 2.0\n");
    fprintf(fp, "material\n");
    fprintf(fp, "ASCII\n");
    fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");

    fprintf(fp, "POINTS %d float\n" , numnode);
    for(int i=0;i<numnode;i++){
        fprintf(fp ,"%.15lf %.15lf %.15lf\n" , node[i][0] , node[i][1] , node[i][2]);
    }

    fprintf(fp, "CELLS %d %d\n" , numele , 4*numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"3 %d %d %d\n" , element[i][0] , element[i][1] , element[i][2]);
    }

    fprintf(fp, "CELL_TYPES %d\n" , numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"5\n");
    }

    fprintf(fp, "CELL_DATA %d\n" , numele);
    fprintf(fp, "SCALARS cell_data int\n");
    fprintf(fp, "LOOKUP_TABLE default\n");
    for(int i=0;i<numele;i++){
        if(fabs(material[i]) > 30){
            fprintf(fp ,"%d\n" , 3);
        }
        else{
            fprintf(fp ,"%d\n" , material[i]);
        }
    }
    fclose(fp);
}

void toolbox::write_vtk(const char* filename, double** node, int** mat_ele, int numnode, int numele){
    FILE* fp;
    fp = fopen(filename,"w");
    fprintf(fp ,"# vtk DataFile Version 2.0\n");
    fprintf(fp, "material\n");
    fprintf(fp, "ASCII\n");
    fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");

    fprintf(fp, "POINTS %d float\n" , numnode);
    for(int i=0;i<numnode;i++){
        fprintf(fp ,"%.15lf %.15lf %.15lf\n" , node[i][0] , node[i][1] , node[i][2]);
    }

    fprintf(fp, "CELLS %d %d\n" , numele , 4*numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"3 %d %d %d\n" , mat_ele[i][1] , mat_ele[i][2] , mat_ele[i][3]);
    }

    fprintf(fp, "CELL_TYPES %d\n" , numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"5\n");
    }

    fprintf(fp, "CELL_DATA %d\n" , numele);
    fprintf(fp, "SCALARS cell_data int\n");
    fprintf(fp, "LOOKUP_TABLE default\n");
    for(int i=0;i<numele;i++){
        fprintf(fp ,"%d\n" , mat_ele[i][0]);
    }
    fclose(fp);
}

//paraview用書き出し(コンター)
void toolbox::write_vtk(const char* filename, double** node, int** element, double* potential, int numnode, int numele){
    FILE* fp;
    fp = fopen(filename,"w");
    fprintf(fp ,"# vtk DataFile Version 2.0\n");
    fprintf(fp, "material\n");
    fprintf(fp, "ASCII\n");
    fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");

    fprintf(fp, "POINTS %d float\n" , numnode);
    for(int i=0;i<numnode;i++){
        fprintf(fp ,"%.15lf %.15lf %.15lf\n" , node[i][0] , node[i][1] , node[i][2]);
    }

    fprintf(fp, "CELLS %d %d\n" , numele , 4*numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"3 %d %d %d\n" , element[i][0] , element[i][1] , element[i][2]);
    }

    fprintf(fp, "CELL_TYPES %d\n" , numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"5\n");
    }

    fprintf(fp, "POINT_DATA %d\n" , numnode);
    fprintf(fp, "SCALARS point_data float\n");
    fprintf(fp, "LOOKUP_TABLE default\n");
    for(int i=0;i<numnode;i++){
        fprintf(fp ,"%.15lf\n" , potential[i]);
    }
    fclose(fp);
}

//paraview用書き出し(ベクトル)
void toolbox::write_vtk(const char* filename, double** node, int** element, double* B_x, double* B_y, int numnode, int numele){
    FILE* fp;
    fp = fopen(filename,"w");
    fprintf(fp ,"# vtk DataFile Version 2.0\n");
    fprintf(fp, "material\n");
    fprintf(fp, "ASCII\n");
    fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");

    fprintf(fp, "POINTS %d float\n" , numnode);
    for(int i=0;i<numnode;i++){
        fprintf(fp ,"%.15lf %.15lf %.15lf\n" , node[i][0] , node[i][1] , node[i][2]);
    }

    fprintf(fp, "CELLS %d %d\n" , numele , 4*numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"3 %d %d %d\n" , element[i][0] , element[i][1] , element[i][2]);
    }

    fprintf(fp, "CELL_TYPES %d\n" , numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"5\n");
    }

    fprintf(fp, "CELL_DATA %d\n" , numele);
    fprintf(fp, "VECTORS cell_data float\n");
    for(int i=0;i<numele;i++){
        fprintf(fp ,"%.15lf %.15lf %.15lf\n" , B_x[i] , B_y[i] , 0.0);
    }
    fclose(fp);
}

//paraview用書き出し(スカラ)
void toolbox::write_vtk_scalar(const char* filename, double** node, int** element, double* B, int numnode, int numele){
    FILE* fp;
    fp = fopen(filename,"w");
    fprintf(fp ,"# vtk DataFile Version 2.0\n");
    fprintf(fp, "material\n");
    fprintf(fp, "ASCII\n");
    fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");

    fprintf(fp, "POINTS %d float\n" , numnode);
    for(int i=0;i<numnode;i++){
        fprintf(fp ,"%.15lf %.15lf %.15lf\n" , node[i][0] , node[i][1] , node[i][2]);
    }

    fprintf(fp, "CELLS %d %d\n" , numele , 4*numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"3 %d %d %d\n" , element[i][0] , element[i][1] , element[i][2]);
    }

    fprintf(fp, "CELL_TYPES %d\n" , numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"5\n");
    }

    fprintf(fp, "CELL_DATA %d\n" , numele);
    fprintf(fp, "SCALARS cell_data float\n");
    fprintf(fp, "LOOKUP_TABLE default\n");
    for(int i=0;i<numele;i++){
        fprintf(fp ,"%.15lf\n" , B[i]);
    }
    fclose(fp);
}

void toolbox::write_vtk_vector(const char* filename, double** node, int** element, double* B_x, double* B_y, int numnode, int numele){
    FILE* fp;
    fp = fopen(filename,"w");
    fprintf(fp ,"# vtk DataFile Version 2.0\n");
    fprintf(fp, "material\n");
    fprintf(fp, "ASCII\n");
    fprintf(fp, "DATASET UNSTRUCTURED_GRID\n");

    fprintf(fp, "POINTS %d float\n" , numnode);
    for(int i=0;i<numnode;i++){
        fprintf(fp ,"%.15lf %.15lf %.15lf\n" , node[i][0] , node[i][1] , node[i][2]);
    }

    fprintf(fp, "CELLS %d %d\n" , numele , 4*numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"3 %d %d %d\n" , element[i][0] , element[i][1] , element[i][2]);
    }

    fprintf(fp, "CELL_TYPES %d\n" , numele);
    for(int i=0;i<numele;i++){
        fprintf(fp ,"5\n");
    }

    fprintf(fp, "POINT_DATA %d\n" , numnode);
    fprintf(fp, "VECTORS point_data float\n");
    for(int i=0;i<numnode;i++){
        fprintf(fp ,"%.15lf %.15lf %.15lf\n" , B_x[i] , B_y[i] , 0.0);
    }
    fclose(fp);
}

//メッシュデータの読み取り(.msh1)
void toolbox::read_mesh1(const char* filename, double** node, int* material, int** element, int numnode, int numele){
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

    for(int i=0;i<numele;i++){
        if(fscanf(fp, "%d %d %d %d %d %d %d %d\n", &n, &tmp0, &m, &tmp1, &tmp2, &a0, &a1, &a2) >0){
            material[i] = m-1;
            element[i][0] = a0-1;
            element[i][1] = a1-1;
            element[i][2] = a2-1;
        }
    }
    fclose(fp);
}

//(r,θ[deg])から(x,y)へ変換(格納有)
void toolbox::pol_to_rec(double r, double deg, double &x, double &y){
    double PI = 3.141592653589;
    double rad = 0.0;
    rad = deg * PI / 180;

    x = r * cos(rad);
    y = r * sin(rad);
}

//(r,θ[deg])から(x,y)へ変換(格納無)
void toolbox::pol_to_rec(double r, double deg){
    double PI = 3.141592653589;
    double rad = 0.0;
    double x = 0.0;
    double y = 0.0;
    rad = deg * PI / 180;

    x = r * cos(rad);
    y = r * sin(rad);

    cout << "(" << r << ", " << deg << "°) -> (" << x << ", " << y << ")" << endl;
}

//(x,y)から(r,θ[deg])へ変換(格納有)
void toolbox::rec_to_pol(double x, double y, double &r, double &deg){
    double PI = 3.141592653589;
    double rad = 0.0;

    r = sqrt(x*x + y*y);

    if(r < 1e-6){
        rad = 0.0;
    }
    else if(x < 1e-6){
        rad = PI * 0.5;
    }
    else{
        rad = atan(y/x);
    }

    deg = rad * 180 / PI;
}   

//CG法実行
void toolbox::CG_Method(double** K, double* b, double* a, int TotalNumber){
    double *r = new double[TotalNumber];//残差ベクトル
    double *p = new double[TotalNumber];
    double *Kp = new double[TotalNumber];
    double *Ka = new double[TotalNumber];

    for( int i=0;i<TotalNumber;i++){
        r[i] = 0.0;
        p[i] = 0.0;
        Kp[i] = 0.0;
        Ka[i] = 0.0;
    }

    for( int i=0;i<TotalNumber;i++){
        for(int j=0;j<TotalNumber;j++){
            Ka[i] += K[i][j] *a[j];//Ka
        }
        r[i] = b[i] - Ka[i];//残差計算
        p[i] = r[i];  //p=r
    }

    double alpha,ke;
    for( int k=0;k<TotalNumber;k++){
        double pKp = 0;
        double rp = 0;
        double rKp = 0;

        for( int i=0;i<TotalNumber;i++){
            Kp[i] = 0;
            rp += r[i] * p[i];//rp

            for( int j=0;j<TotalNumber;j++){
                Kp[i] += K[i][j] * p[j];//Kp
            }
            pKp += p[i] * Kp[i];//pKp
        }

        alpha = rp/pKp;

        double beta;
        for( int i=0;i<TotalNumber;i++){
            a[i] = a[i] + alpha * p[i];
            r[i] = r[i] - alpha * Kp[i];
            rKp += r[i] * Kp[i];//rKp
        }
        beta = -rKp/pKp;

        for( int i=0;i<TotalNumber;i++){
            p[i] = r[i] + beta * p[i];
        }

        double normR = 0.0;
        double normB = 0.0;
        for( int i=0;i<TotalNumber;i++){
            normR = normR + r[i] * r[i];
            normB = normB + b[i] * b[i];
        }

        if( sqrt(normR) < sqrt(normB) * 10e-8 ) break;//収束判定

        // cout << sqrt(normR) << endl;
        ke = k;
    }
    // cout << ke <<endl;

    delete[] r;
    delete[] p;
    delete[] Kp;
    delete[] Ka;
}

//int型二次元配列を表示
void toolbox::cout_method(int** K, int numrow, int numcolmn){
    for(int i=0;i<numrow;i++){
        for(int j=0;j<numcolmn;j++){
            cout << K[i][j] << ", ";
        }
        cout << "\n";
    }
}

//double型二次元配列を表示
void toolbox::cout_method(double** K, int numrow, int numcolmn){
    for(int i=0;i<numrow;i++){
        for(int j=0;j<numcolmn;j++){
            cout << K[i][j] << ", ";
        }
        cout << "\n";
    }
}