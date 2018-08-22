#include <iostream>
using std::cout;
using std::cerr;
#include <string>
using std::string;
#include <ctype.h>
#include <map>
#include <set>
#include<vector>
#include <algorithm>

#include "nao_lineares.cpp"
std::vector<Diodo> vecDiodos; //vetor contendo todos os diodos
std::vector<MOSFET> vecMosfet; //vetor contendo todos os MOSFET

using namespace std;

std::map<std::string, int> mapNos; //mapeamento dos nos
int valorNo=0; //valor do nó adicionadoz

std::set<string> elGrupo2; //elementos para o grupo2 da matriz MNA

//mapeia o código ASCII do componente com seu nome para printar
string mapc(char comp){
    switch(toupper(comp)){ //elimina sensibilidade a maiúscula e minúscula
    case 'R':
        return "RES";
    case 'C':
        return "CAP";
    case 'L':
        return "IND";
    case 'V':
        return "VSR";
    case 'I':
        return "ISR";
    case 'E':
        return "VCVS";
    case 'F':
        return "CCCS";
    case 'G':
        return "VCCS";
    case 'H':
        return "CCVS";
    case 'D':
        return "DIO";
    case 'Q':
        return "TJB";
    case 'M':
        return "MOSFET";
    }
}

// classe No
class No
{
private:
	char tipo;     //tipo de componente (ASCII)
    string label; //label do componente
    int no1;   //nós
    int no2;
    int no3;
    int no4;
    string el_controle;
    double valor; //valor do componente
    string modelo; //modelo no caso de transistor
    double IS, N; //parâmetros para diodo
    double K, VTH, LAMBDA, W, L; //parâmetros para MOSFET
	No* prox;
public:
	No(char tipo, string label, string no1, string no2, double valor) // construtor para componentes passivos e ativos
	{
		this->tipo = tipo;
		this->label = label;
		if(mapNos.find(no2) != mapNos.end()) this->no2 = mapNos[no2];
		else {
            this->no2 = valorNo;
            mapNos[no2] = valorNo++;
		}
		if(mapNos.find(no1) != mapNos.end()) this->no1 = mapNos[no1];
		else {
            this->no1 = valorNo;
            mapNos[no1] = valorNo++;
		}
		this->valor = valor;
		this->prox = NULL;
	}
	No(char tipo, string label, string no1, string no2, string no3, string no4, double valor) // construtor para fontes controladas por tensão
	{
		this->tipo = tipo;
		this->label = label;
		if(mapNos.find(no4) != mapNos.end()) this->no4 = mapNos[no4];
		else {
            this->no4 = valorNo;
            mapNos[no4] = valorNo++;
		}
		if(mapNos.find(no3) != mapNos.end()) this->no3 = mapNos[no3];
		else {
            this->no3 = valorNo;
            mapNos[no3] = valorNo++;
		}
		if(mapNos.find(no2) != mapNos.end()) this->no2 = mapNos[no2];
		else {
            this->no2 = valorNo;
            mapNos[no2] = valorNo++;
		}
		if(mapNos.find(no1) != mapNos.end()) this->no1 = mapNos[no1];
		else {
            this->no1 = valorNo;
            mapNos[no1] = valorNo++;
		}
		this->valor = valor;
		this->prox = NULL;
	}
	No(char tipo, string label, string no1, string no2, string el_controle, double valor) // construtor para fontes controladas por corrente
	{
		this->tipo = tipo;
		this->label = label;
		if(mapNos.find(no2) != mapNos.end()) this->no2 = mapNos[no2];
		else {
            this->no2 = valorNo;
            mapNos[no2] = valorNo++;
		}
		if(mapNos.find(no1) != mapNos.end()) this->no1 = mapNos[no1];
		else {
            this->no1 = valorNo;
            mapNos[no1] = valorNo++;
		}
		this->el_controle = el_controle;
		this->valor = valor;
		this->prox = NULL;
	}
	No(char tipo, string label, string no1, string no2, string no3, string modelo) // construtor para transistor não implementados
	{
		this->tipo = tipo;
		this->label = label;
		if(mapNos.find(no3) != mapNos.end()) this->no3 = mapNos[no3];
		else {
            this->no3 = valorNo;
            mapNos[no3] = valorNo++;
		}
		if(mapNos.find(no2) != mapNos.end()) this->no2 = mapNos[no2];
		else {
            this->no2 = valorNo;
            mapNos[no2] = valorNo++;
		}
		if(mapNos.find(no1) != mapNos.end()) this->no1 = mapNos[no1];
		else {
            this->no1 = valorNo;
            mapNos[no1] = valorNo++;
		}
		this->modelo = modelo;
		this->prox = NULL;
	}
	No(char tipo, string label, string no1, string no2, double IS, double N, string modelo) // construtor para diodos
	{
        this->tipo = tipo;
		this->label = label;
		if(mapNos.find(no2) != mapNos.end()) this->no2 = mapNos[no2];
		else {
            this->no2 = valorNo;
            mapNos[no2] = valorNo++;
		}
		if(mapNos.find(no1) != mapNos.end()) this->no1 = mapNos[no1];
		else {
            this->no1 = valorNo;
            mapNos[no1] = valorNo++;
		}
		this->IS = IS;
		this->N = N;
        this->modelo = modelo;
		this->prox = NULL;

		// adiciona o novo diodo ao vetor de diodos
		Diodo dio(label, this->no1, this->no2, IS, N);
		vecDiodos.push_back(dio);
 	}
	No(char tipo, string label, string no1, string no2, string no3,
                    double K, double VTH, double LAMBDA, double W, double L, string modelo) // construtor para MOSFET
	{
		this->tipo = tipo;
		this->label = label;
		if(mapNos.find(no3) != mapNos.end()) this->no3 = mapNos[no3];
		else {
            this->no3 = valorNo;
            mapNos[no3] = valorNo++;
		}
		if(mapNos.find(no2) != mapNos.end()) this->no2 = mapNos[no2];
		else {
            this->no2 = valorNo;
            mapNos[no2] = valorNo++;
		}
		if(mapNos.find(no1) != mapNos.end()) this->no1 = mapNos[no1];
		else {
            this->no1 = valorNo;
            mapNos[no1] = valorNo++;
		}
		this->K = K;
		this->VTH = VTH;
		this-> LAMBDA = LAMBDA;
		this->W = W;
		this->L = L;
		this->modelo = modelo;
		this->prox = NULL;

		//adiciona o novo MOSFET ao vetor de MOSFETS
		MOSFET mos(label, this->no1, this->no2, this->no3, K, VTH, LAMBDA, W, L);
		vecMosfet.push_back(mos);
	}
    // obtém os valores
	char getTipo(){return tipo;}
	string getLabel(){return label;}
	int getNo1(){return no1;}
	int getNo2(){return no2;}
	int getNo3(){return no3;}
	int getNo4(){return no4;}
	double getValor(){return valor;}
	string getModelo(){return modelo;}
	string getEl_controle(){return el_controle;}
	//parâmetros diodos
    double getIS() {return IS;}
    double getN() {return N;}
	//parâmetros MOSFETS
	double getK() {return K;}
    double getVTH() {return VTH;}
    double getLAMBDA() {return LAMBDA;}
    double getW() {return W;}
    double getL() {return L;}

	No* obterProx() // obtém o próximo No
	{
		return prox;
	}

	void setProx(No* p) // seta o próximo No
	{
		prox = p;
	}
};

// classe Lista
class Lista
{
private:
	No* cabeca; // primeiro elemento
	No* cauda; // último elemento

public:
	Lista()
	{
		// se não passar elemento, então cabeca e cauda são NULL
		cabeca = NULL;
		cauda = NULL;
	}

	void mostrar() // mostra todos os elementos da lista
	{
		cout << "\nImprimindo todos os elementos...\n";
		No* c = cabeca;

		if(vazia())
			cerr << "A lista NAO possui elementos!!\n";
		else
		{
		    int cnt = 1; //contador de elementos
			while(c) // percorre a lista com os elementos
			{
			    int tipo = c->getTipo();
				cout << "#" << cnt++ << ": ";
				cout << mapc(tipo) << "[" << c->getLabel() << "];\t";
                if (tipo=='E'||tipo=='G')
                    cout << "n+[" << c->getNo1() << "], n-[" << c->getNo2() << "], Cno+[" << c->getNo3() << "], Cno-[" << c->getNo4() << "];\t valor=" << c->getValor() << endl;
                else if (tipo=='F'||tipo=='H')
                    cout << "n+[" << c->getNo1() << "], n-[" << c->getNo2() << "], Cntrl[" << c->getEl_controle() << "];\t valor=" << c->getValor() << endl;
                else if (tipo=='Q')
                    cout << "C[" << c->getNo1() << "], B[" << c->getNo2() << "], E[" << c->getNo3() << "];\t modelo=" << c->getModelo() << endl;
                else if (tipo=='M')
                    cout << "D[" << c->getNo1() << "], G[" << c->getNo2() << "], S[" << c->getNo3() << "];\t modelo=" << c->getModelo()
                    << ";\tK'=" <<c->getK() << "; VTH=" <<c->getVTH() << "; LAMBDA=" <<c->getLAMBDA() << "; W=" <<c->getW() << "; L=" <<c->getL() << endl;
                else if (tipo=='D')
                    cout << "n+[" << c->getNo1() << "], n-[" << c->getNo2() << "];\tIS=" << c->getIS() << "; N=" <<c->getN() << endl;
                else
                    cout << "n+[" << c->getNo1() << "], n-[" << c->getNo2() << "];\t valor=" << c->getValor() << endl;
				c = c->obterProx();
			}
			// percorre a lista com os valores de mapeamento dos nós
			cout << "\nRotulos dos nos:" << endl;
			std::map<string, int>::iterator it = mapNos.begin();
            while(it != mapNos.end())
            {
                cout << "No " << it->first << " -> " << it->second << endl;
                it++;
            }
		}
	}

	No* getInicio() { //retorna o inicio da lista
        return cabeca;
	}

	bool vazia() // verifica se a lista está vazia
	{
		return (cabeca == NULL);
	}

	// insere no final (semelhante a push_back da list)
	void inserir_final(char tipo, string label, string no1, string no2, double valor) //elementos passivos e ativos
	{
	    // verifica se é fonte de tensão
	    if (tipo=='V') elGrupo2.insert(label);

		No* novo_no = new No(tipo, label, no1, no2, valor);

		if(vazia())
		{
			cabeca = novo_no;
			cauda = novo_no;
		}
		else
		{
			cauda->setProx(novo_no);
			cauda = novo_no;
		}
	}
	void inserir_final(char tipo, string label, string no1, string no2, string no3, string no4, double valor) //fontes controladas por tensão
	{
	    // verifica se é fonte de tensão
	    if (tipo=='E') elGrupo2.insert(label);

		No* novo_no = new No(tipo, label, no1, no2, no3, no4, valor);

		if(vazia())
		{
			cabeca = novo_no;
			cauda = novo_no;
		}
		else
		{
			cauda->setProx(novo_no);
			cauda = novo_no;
		}
	}
	void inserir_final(char tipo, string label, string no1, string no2, string el_controle, double valor) //fontes controladas por corrente
	{
	    elGrupo2.insert(el_controle);
	    // verifica se é fonte de tensão
	    if (tipo=='H') elGrupo2.insert(label);

		No* novo_no = new No(tipo, label, no1, no2, el_controle, valor);

		if(vazia())
		{
			cabeca = novo_no;
			cauda = novo_no;
		}
		else
		{
			cauda->setProx(novo_no);
			cauda = novo_no;
		}
	}
	void inserir_final(char tipo, string label, string no1, string no2, string no3, string modelo) //transistores não implementados
	{
		No* novo_no = new No(tipo, label, no1, no2, no3, modelo);

		if(vazia())
		{
			cabeca = novo_no;
			cauda = novo_no;
		}
		else
		{
			cauda->setProx(novo_no);
			cauda = novo_no;
		}
	}
	void inserir_final(char tipo, string label, string no1, string no2, double IS, double N, string modelo) //diodos
	{
		No* novo_no = new No(tipo, label, no1, no2, IS, N, modelo);

		if(vazia())
		{
			cabeca = novo_no;
			cauda = novo_no;
		}
		else
		{
			cauda->setProx(novo_no);
			cauda = novo_no;
		}
	}
	void inserir_final(char tipo, string label, string no1, string no2, string no3,
                    double K, double VTH, double LAMBDA, double W, double L, string modelo) //transistores MOSFET
	{
		No* novo_no = new No(tipo, label, no1, no2, no3, K, VTH, LAMBDA, W, L, modelo);

		if(vazia())
		{
			cabeca = novo_no;
			cauda = novo_no;
		}
		else
		{
			cauda->setProx(novo_no);
			cauda = novo_no;
		}
	}

	// retorna o tamanho da lista
	int tamanho()
	{
		if(vazia()) // se for vazia, entã retorna 0
			return 0;

		No* c = cabeca;
		int tam = 0;

		// percorre a lista
		do
		{
			c = c->obterProx();
			tam++;
		}
		while(c);

		return tam;
	}
};
