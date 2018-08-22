#include "leitura_componentes.cpp"

#include <iostream>
using std::cout;
using std::cerr;
#include <map>
#include <string>
using std::string;
#include<set>
#include<vector>
#include<cmath>

// classe que cria a matriz para a MNA com base na lista de componentes
class SolveSys{

private:
    // entradas
    Lista componentes;
    double er_rel; //erro relativo
    double er_abs; //erro absoluto
    int max_it;    //máximo de iterações para convergência

    std::map<std::string, int> mapCorr; //mapeamento da linha da corrente com o elemento
    int TAM;
    double** A;
    double** nA;
    double** H;
    double** LU;
    double* b;
    double* nb;
    double* s;
    int* p; //array de permutações de linhas

    //matrizes soluções
    double* x;
    double* y;

    // mapeia as correntes do grupo2 com os elementos
    void mapearCorrentes(){
        typename std::set<string>::iterator it;
        int cont=0;
        for (it=elGrupo2.begin(); it!=elGrupo2.end(); ++it)
            mapCorr[*it] = mapNos.size()+cont++;
    }

    // MONTA MATRIZ MNA LINEAR
    void montaMNA(){
        this->TAM = mapNos.size()+elGrupo2.size();

        // alocamento de memória para as matrizes
        A = new double *[TAM]; for(int i=0; i <TAM; i++) A[i] = new double[TAM];
        b = new double [TAM];
        nA = new double *[TAM]; for(int i=0; i <TAM; i++) nA[i] = new double[TAM];
        nb = new double [TAM];
        H = new double *[TAM]; for(int i=0; i <TAM; i++) H[i] = new double[TAM];
        LU = new double *[TAM]; for(int i=0; i <TAM; i++) LU[i] = new double[TAM];
        s = new double [TAM];
        p = new int [TAM];

        // zera todos elementos das matrizes
        for (int l=0; l<TAM; l++) {
            p[l] = l;
            b[l] = 0.0;
            nb[l] = 0.0;
            for (int cl=0; cl<TAM; cl++) {
                A[l][cl]=0.0;
                nA[l][cl]=0.0;
            }
        }

        //aloca memória e zera as componentes de x e y
        x = new double [TAM]; y = new double [TAM];
        for (int i=0; i<TAM; i++){ x[i] = 0.0; y[i] = 0.0; }

        No* c = componentes.getInicio();
        while(c) // percorre a lista com os elementos
        {
            // elementos para grupo2
            if (elGrupo2.find(c->getLabel())!=elGrupo2.end()) {
                if (c->getTipo()=='V') {
                    A[ c->getNo1() ][ mapCorr[c->getLabel()] ] += 1;
                    A[ c->getNo2() ][ mapCorr[c->getLabel()] ] += -1;
                    A[ mapCorr[c->getLabel()] ][ c->getNo1() ] += 1;
                    A[ mapCorr[c->getLabel()] ][ c->getNo2() ] += -1;
                    b[ mapCorr[c->getLabel()] ] += c->getValor();
                }
                else if (c->getTipo()=='I') {
                    A[ c->getNo1() ][ mapCorr[c->getLabel()] ] += 1;
                    A[ c->getNo2() ][ mapCorr[c->getLabel()] ] += -1;
                    A[ mapCorr[c->getLabel()] ][ mapCorr[c->getLabel()] ] += 1;
                    b[ mapCorr[c->getLabel()] ] += c->getValor();
                }
                else if (c->getTipo()=='E') {
                    A[ c->getNo1() ][ mapCorr[c->getLabel()] ] += 1;
                    A[ c->getNo2() ][ mapCorr[c->getLabel()] ] += -1;
                    A[ mapCorr[c->getLabel()] ][ c->getNo1() ] += 1;
                    A[ mapCorr[c->getLabel()] ][ c->getNo2() ] += -1;
                    A[ mapCorr[c->getLabel()] ][ c->getNo3() ] += -c->getValor();
                    A[ mapCorr[c->getLabel()] ][ c->getNo4() ] += c->getValor();
                }
                else if (c->getTipo()=='F') {
                    A[ c->getNo1() ][ mapCorr[c->getLabel()] ] += 1;
                    A[ c->getNo2() ][ mapCorr[c->getLabel()] ] += -1;
                    A[ mapCorr[c->getLabel()] ][ mapCorr[c->getLabel()] ] += 1;
                    A[ mapCorr[c->getLabel()] ][ mapCorr[c->getEl_controle()] ] += c->getValor();
                }
                else if (c->getTipo()=='G') {
                    A[ c->getNo1() ][ mapCorr[c->getLabel()] ] += 1;
                    A[ c->getNo2() ][ mapCorr[c->getLabel()] ] += -1;
                    A[ mapCorr[c->getLabel()] ][ c->getNo3() ] += c->getValor();
                    A[ mapCorr[c->getLabel()] ][ c->getNo4() ] += -c->getValor();
                    A[ mapCorr[c->getLabel()] ][ mapCorr[c->getLabel()] ] += 1;
                }
                else if (c->getTipo()=='H') {
                    A[ c->getNo1() ][ mapCorr[c->getLabel()] ] += 1;
                    A[ c->getNo2() ][ mapCorr[c->getLabel()] ] += -1;
                    A[ mapCorr[c->getLabel()] ][ c->getNo1() ] += 1;
                    A[ mapCorr[c->getLabel()] ][ c->getNo2() ] += -1;
                    A[ mapCorr[c->getLabel()] ][ mapCorr[c->getEl_controle()] ] += -c->getValor();
                }
                else if (c->getTipo()=='R') {
                    A[ c->getNo1() ][ mapCorr[c->getLabel()] ] += 1;
                    A[ c->getNo2() ][ mapCorr[c->getLabel()] ] += -1;
                    A[ mapCorr[c->getLabel()] ][ c->getNo1() ] += 1;
                    A[ mapCorr[c->getLabel()] ][ c->getNo2() ] += -1;
                    A[ mapCorr[c->getLabel()] ][ mapCorr[c->getLabel()] ] += -c->getValor();
                }

            }
            // elementos para grupo1
            else {
                if (c->getTipo()=='I') {
                    b[ c->getNo1() ] += -c->getValor();
                    b[ c->getNo2() ] += c->getValor();
                }
                else if (c->getTipo()=='F') {
                    A[ c->getNo1() ][ mapCorr[c->getEl_controle()] ] += c->getValor();
                    A[ c->getNo2() ][ mapCorr[c->getEl_controle()] ] += -c->getValor();

                }
                else if (c->getTipo()=='G') {
                    A[ c->getNo1() ][ c->getNo3() ] += c->getValor();
                    A[ c->getNo2() ][ c->getNo3() ] += -c->getValor();
                    A[ c->getNo1() ][ c->getNo4() ] += -c->getValor();
                    A[ c->getNo2() ][ c->getNo4() ] += c->getValor();
                }
                else if (c->getTipo()=='R') {
                    A[ c->getNo1() ][ c->getNo1() ] += 1/c->getValor();
                    A[ c->getNo1() ][ c->getNo2() ] += -1/c->getValor();
                    A[ c->getNo2() ][ c->getNo1() ] += -1/c->getValor();
                    A[ c->getNo2() ][ c->getNo2() ] += 1/c->getValor();
                }
            }
            c = c->obterProx();
        }
    }

    // CRIA A MATRIZ DOS NÃO-LINEARES
    void montaMNAnaoLinear(){
        // zera os elementos
        for (int i=0; i<TAM; i++){
            for (int j=0; j<TAM; j++){
                nA[i][j] = 0.0;
                nb[j] = 0.0;
            }
        }

        // percorre os diodos
        for (int i=0; i<vecDiodos.size(); i++){
            Diodo dio = vecDiodos[i];
            // calcula os novos parâmetros Geq e Ieq
            dio.calcParametros( x[dio.getNo1()] , x[dio.getNo2()] );

            nA[ dio.getNo1() ][ dio.getNo1() ] += dio.getGeq();
            nA[ dio.getNo1() ][ dio.getNo2() ] -= dio.getGeq();
            nA[ dio.getNo2() ][ dio.getNo2() ] += dio.getGeq();
            nA[ dio.getNo2() ][ dio.getNo1() ] -= dio.getGeq();

            nb[ dio.getNo1() ] -= dio.getIeq();
            nb[ dio.getNo2() ] += dio.getIeq();
        }

        // percorre os MOSFETs
        for (int i=0; i<vecMosfet.size(); i++){
            MOSFET mos = vecMosfet[i];
            // calcula os novos parâmetros Gds e gm
            mos.calcParametros( x[mos.getNo1()] , x[mos.getNo2()] , x[mos.getNo3()] );

            nA[ mos.getNo1() ][ mos.getNo1() ] += mos.getGds();
            nA[ mos.getNo1() ][ mos.getNo2() ] -= mos.getGds()+mos.getGm();
            nA[ mos.getNo1() ][ mos.getNo3() ] += mos.getGm();
            nA[ mos.getNo2() ][ mos.getNo1() ] -= mos.getGds();
            nA[ mos.getNo2() ][ mos.getNo2() ] += mos.getGds()+mos.getGm();
            nA[ mos.getNo2() ][ mos.getNo3() ] -= mos.getGm();

            nb[ mos.getNo1() ] -= mos.getIeq();
            nb[ mos.getNo2() ] += mos.getIeq();
        }
    }

    // monta a matriz LU
    void makeLU(){

        for (int i=0; i<TAM; i++){
            p[i] = i; // reordena o vetor de permutações
            for (int j=0; j<TAM; j++) {
                H[i][j] = A[i][j]+nA[i][j];
                LU[i][j] = H[i][j];
            }
        }

        for (int k=1; k<TAM; k++){

            int kmax = k;
            // encontra linha com maior elemento e troca os valores
            for (int l=k+1; l<TAM; l++){
                if ( std::abs(LU[p[l]][k]) > std::abs(LU[p[k]][k]) ){
                    kmax = l;
                }
            }

            // permutando o elemento k com o elemento kmax
            int aux = p[k];
            p[k] = p[kmax];
            p[kmax] = aux;

            for (int i=k+1; i<TAM; i++){
                LU[p[i]][k] = LU[p[i]][k]/LU[p[k]][k];
                for (int j=k+1; j<TAM; j++){
                    LU[p[i]][j] = LU[p[i]][j] - LU[p[i]][k]*LU[p[k]][j];
                }
            }
        }
    }

    // resolve o sistema com base na matriz LU
    void solveLU(){
        // zera os valores de solução e monta matriz s
        for (int i=0; i<TAM; i++){
            y[i] = 0.0; x[i] = 0.0;
            s[i] = b[i]+nb[i];
        }

        //substituição progressiva sobre L
        for (int k=1; k<TAM; k++){
            y[k] = s[p[k]];
            for (int j=1; j<k; j++)
                y[k] -= LU[p[k]][j]*y[j];
        }

        //substituição regressiva sobre U
        for (int k=TAM-1; k>0; k--){
            x[k] = y[k];
            for (int j=k+1; j<TAM; j++)
                x[k] -= LU[p[k]][j]*x[j];
            x[k] /= LU[p[k]][k];
        }
    }

public:
    SolveSys(Lista componentes, double er_rel, double er_abs, int max_it){ //construtor
        this->componentes = componentes;
        this->er_rel = er_rel;
        this->er_abs = er_abs;
        this->max_it = max_it;
        mapearCorrentes();
        montaMNA();
    }

    //mostra as matrizes para validação
    void showMatrix(){

        cout << "\nMatriz H\n" << std::scientific;
        for (int l=1; l<TAM; l++)
            for (int cl=1; cl<TAM; cl++)
                cout << "H(" << l << "," << cl << ") = " << H[l][cl] << endl;

        cout << "\nMatriz s\n" << std::scientific;
        for (int l=1; l<TAM; l++) cout << "s(" << l << ") = " << s[l] << endl;

        /*
        //formatação em matriz
        cout << "\nMatriz H" << endl;
        for (int l=1; l<TAM; l++){
            for (int cl=1; cl<TAM; cl++)
                cout << H[l][cl] << "\t";
            cout << "\n";
        }

        cout << "\nMatriz s" << endl;
        for (int i=1; i<TAM; i++) cout << s[i] << endl;
        */

    }

    // resolve o sistema usando o método de Newton para sistemas não lineares
    void solveNewton(){
        int it=0; //número de iterações

        // valor de solução anterior
        double x_ant[TAM];

        // resíduo
        double maxResiduo;   // |s-H*x|
        double residuo[TAM]; // matriz

        // correção
        double maxCorrecao;     // |x - x_ant|
        double correcao[TAM];   // matriz

        // tolerância
        double tol = er_rel*0.0 + er_abs; // tol = RELTOL*norma(x) + ABSTOL;

        // iterações do método de Newton
        do{
            it++;

            // resolve sistema para próxima iteração
            montaMNAnaoLinear();
            makeLU();
            solveLU();

            // correção
            for (int i=0; i<TAM; i++) correcao[i] = x[i] - x_ant[i];
            maxCorrecao = correcao[1];
            for (int i=2; i<TAM; i++)
                if ( std::abs(correcao[i])>maxCorrecao )
                    maxCorrecao = std::abs(correcao[i]);

            // resíduo
            for (int i=0; i<TAM; i++){
                residuo[i] = s[i];
                for (int j=0; j<TAM; j++)
                    residuo[i] -= H[i][j]*x[j];
            }
            maxResiduo = std::abs(residuo[1]);
            for (int i=2; i<TAM; i++)
                if ( std::abs(residuo[i])>maxResiduo )
                    maxResiduo = std::abs(residuo[i]);

            // atualiza x_ant
            for (int i=0; i<TAM; i++) {
                x_ant[i] = x[i];
            }
        } while( (maxCorrecao>tol || maxResiduo>tol) && max_it>it);

        //mostra a solução do problema
        cout << "\nSolucao do circuito:" << std::scientific;
        // valor das tensões de nó
        for (std::map<std::string, int>::iterator it=mapNos.begin(); it!=mapNos.end(); ++it)
            cout << "\nVno[" << it->first << "] = " << x[it->second] << std::scientific;
        cout << endl;
        // valor das correntes dos elementos do grupo2
        for (std::map<std::string, int>::iterator it=mapCorr.begin(); it!=mapCorr.end(); ++it)
            cout << "\nIel[" << it->first << "] = " << x[it->second] << std::scientific;
        cout << "\n\nIteracoes para convergencia: " << it << endl;
    }

    // faz o refinamento iterativo sobre o sistema
    void solveRefIt(){
        makeLU();
        solveLU();

        double r[TAM];
        double z[TAM]; //equivalente ao x
        double w[TAM]; //equivalente ao y
        for (int i=0; i<TAM; i++) { z[i] = x[i]; w[i] = y[i]; }
        double maxZ;
        double maxX;

        int it=0;
        do{
            it++;
            // r = b - Ax~
            for (int i=0; i<TAM; i++){
                r[i] = b[i];
                for (int j=0; j<TAM; j++)
                    r[i] -= A[i][j]*x[j];
            }

            //****** RESOLUÇÃO POR LU *****
            //substituição progressiva sobre L
            for (int k=1; k<TAM; k++){
                w[k] = r[p[k]];
                for (int j=1; j<k; j++)
                    w[k] -= LU[p[k]][j]*w[j];
            }

            //substituição regressiva sobre U
            for (int k=TAM-1; k>0; k--){
                z[k] = w[k];
                for (int j=TAM-1; j>k+1; j--)
                    z[k] -= LU[p[k]][j]*z[j];
                z[k] /= LU[p[k]][k];
            }
            //********************************

            // x~ = x~ + z
            for (int i=0; i<TAM; i++) x[i] += z[i];

            // encontra maior valor de z e x
            maxZ = std::abs(z[1]);
            maxX = std::abs(x[1]);
            for (int i=2; i<TAM; i++){
                if ( std::abs(z[i])>maxZ ) maxZ = std::abs(z[i]);
                if ( std::abs(x[i])>maxX ) maxX = std::abs(x[i]);
            }
        } while ( maxZ > (er_rel*maxX+er_abs) && max_it>it );

        //mostra a solução do problema
        cout << "\nSolucao do circuito:" << std::scientific;
        // valor das tensões de nó
        for (std::map<std::string, int>::iterator it=mapNos.begin(); it!=mapNos.end(); ++it)
            cout << "\nVno[" << it->first << "] = " << x[it->second] << std::scientific;
        cout << endl;
        // valor das correntes dos elementos do grupo2
        for (std::map<std::string, int>::iterator it=mapCorr.begin(); it!=mapCorr.end(); ++it)
            cout << "\nIel[" << it->first << "] = " << x[it->second] << std::scientific;
        cout << "\n\nIteracoes para convergencia: " << it << endl;
    }
};
