/*
Classe dedicada ao tratamento de elementos n�o lineares
Implementados: diodo e transistor MOSFET
*/

#include <math.h>

class Diodo
{
private:
    string label;
    int no1, no2;
    double IS, N, VT;
    double Geq, Ieq, id; //valores para serem recalculados

public:
    Diodo(string label, int no1, int no2, double IS, double N){
        this->label = label;
        this->no1 = no1;
        this->no2 = no2;
        this->IS = IS;
        this->N = N;

        VT = 26e-3; //fixo
        id = 0;     //inicial
    }

    // getters
    string getLabel(){return label;}
	int getNo1(){return no1;}
	int getNo2(){return no2;}

    // c�lculo dos par�metros
        //v1 e v2 s�o os valores de tens�o nos n�s 1 e 2
    void calcParametros(double v1, double v2){
        Geq = IS/(N*VT)*exp( (v1-v2)/(N*VT) );
        id = IS*( exp( (v1-v2)/(N*VT) ) - 1 );
        Ieq = id - Geq*(v1-v2);
    }

    // retorno dos par�metros
    double getGeq() {return Geq;}
    double getIeq() {return Ieq;}
};

class MOSFET
{
private:
    string label;
    int no1, no2, no3;
    double K, VTH, LAMBDA, W, L;
    double beta;
    double id, ig, is, Ieq;
    double vgs, vds;
    double Gds, gm;

public:
    MOSFET(string label, int no1, int no2, int no3,
                    double K, double VTH, double LAMBDA, double W, double L){
        this->no1 = no1;
        this->no2 = no2;
        this->no3 = no3;
        this->K = K;
		this->VTH = VTH;
		this-> LAMBDA = LAMBDA;
		this->W = W;
		this->L = L;

        //valores fixos
        ig = 0;
        beta = K*W/L;
    }

    //getters
    string getLabel(){return label;}
	int getNo1(){return no1;}
	int getNo2(){return no2;}
	int getNo3(){return no3;}

	// c�lculo dos par�metros
        //v1, v2 e v3 s�o os valores de tens�o nos n�s D, G e S
    void calcParametros(double v1, double v2, double v3){
        vgs = v2-v3;
        vds = v1-v3;
        // verifica ponto de opera��o
        if ( (0<=(vgs-VTH)) && ((vgs-VTH)<=vds) ){ //satura��o
            id=(beta/2)*(vgs-VTH)*(vgs-VTH)*(1+LAMBDA*vds);
            Gds=(beta/2)*(vgs-VTH)*(1+LAMBDA*vds);
            gm=beta*(vgs-VTH)*(1+LAMBDA*vds);
        }
        else if ( (0<=vds) && (vds<=(vgs-VTH)) ) { //linear
            id=beta*( (vgs-VTH)*vds - vds*vds/2 )*(1+LAMBDA*vds);
            Gds=(beta/2)*LAMBDA*vds*vds + beta*(vgs-VTH-vds)*(1+LAMBDA*vds);
            gm=beta*vds*(1+LAMBDA*vds);
        }
        else if (vgs<=VTH) { //corte
            id=0;
            Gds=0;
            gm=0;
        }

        is=-id;
        Ieq = id-Gds*vds-gm*vgs;
    }

    double getGds() {return Gds;}
    double getGm() {return gm;}
    double getIeq() {return Ieq;}
};
