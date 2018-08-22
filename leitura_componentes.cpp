#include "lista_encadeada.cpp"

#include <fstream>
#include <iostream>
using std::cout;
using std::cerr;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <sstream>
using std::stringstream;
#include <algorithm>
using std::replace;
#include <locale>
#include <math.h>

Lista comps;

// verifica se o tipo lido existe
bool validaTipo(char tipo){
    if (tipo=='R'||tipo=='L'||tipo=='C'||tipo=='D'||\
        tipo=='E'||tipo=='F'||tipo=='G'||tipo=='H'||\
        tipo=='Q'||tipo=='M'||tipo=='V'||tipo=='I') return 1;
    else return 0;
}

// informa o numero mínimo de informacao que determinado tipo deve receber
int nroInfo(char tipo) {
    if (tipo=='R'||tipo=='L'||tipo=='C'||tipo=='D'||tipo=='V'||tipo=='I') return 4;
    if (tipo=='Q'||tipo=='M'||tipo=='F'||tipo=='H') return 5;
    if (tipo=='E'||tipo=='G') return 6;
}

// valida se uma string é composta apenas por números
bool verificaNro(const string &str) {
    return str.find_first_not_of("0123456789.E-+") == string::npos;
}

// valida se há apenas caracteres válidos no valor
bool verificaValor (const string &str) {
    string copia = str;
    string validos = "0123456789EFPNUMKGT.-+";
    for (int i=0; i<copia.size(); i++){
        if ( validos.find(copia[i]) == string::npos ) {return 0;}
    }
    //verifica se não há múltiplos multiplicadores
    vector<string> multp;
    multp.push_back("MEG"); multp.push_back("F"); multp.push_back("P"); multp.push_back("N");
    multp.push_back("U"); multp.push_back("M"); multp.push_back("K"); multp.push_back("G"); multp.push_back("T");
    for (int i=0; i<multp.size(); i++){
        // remove a primeira ocorrência do multiplicador se estiver no valor
        size_t pos = copia.find(multp[i]);
        if (pos != std::string::npos) {
            copia.erase(pos, multp[i].size());
            if (!verificaNro(copia)) {cout << copia << endl; return 0;}
        }
    }
    return 1;
}

// valida o valor passado para o componente
double validaValor(string val){
    int pot=0; //potência multiplicada pelo valor
    if (!verificaNro(val)) {
        if (val.find("MEG") != string::npos) pot=6;
        else if (val.find("F") != string::npos) pot=-15;
        else if (val.find("P") != string::npos) pot=-12;
        else if (val.find("N") != string::npos) pot=-9;
        else if (val.find("U") != string::npos) pot=-6;
        else if (val.find("M") != string::npos) pot=-3;
        else if (val.find("K") != string::npos) pot=3;
        else if (val.find("G") != string::npos) pot=9;
        else if (val.find("T") != string::npos) pot=12;
    }
    std::istringstream iss(val);
    double valor = 0;
    iss >> valor;
    return valor*pow(10,pot);
}

// valida qual(s) comando foi passado
int validaComando(string comando){
    // cada comando teria um valor relacionado
    if (comando.compare("OP") == 0) return 1;
    else {
        cerr << "Comando " + comando + " invalido!" << endl;
        return 0;
    }
}

// insere todos os componentes
int insere_comp(char* arquivo){
    std::ifstream arq(arquivo);
    string linha, comando;
    int nroLinha=0; //conta o numero de linhas
    int cmd=0; //refere-se ao número do comando que será executado (0=sem comando)
    while (std::getline(arq, linha))
    {
        if (++nroLinha<2) {continue;} //elimina primeira linha
        linha = linha.substr(0, linha.find("*", 0)); //elimina comentários
        // pula linhas vazias (apenas com espaço ou tabulação)
        if(linha.find_first_not_of(' ') == std::string::npos || linha.find_first_not_of('\t') == std::string::npos) continue;

        replace(linha.begin(), linha.end(), '\t', ' '); //troca todas tabulações por espaços
        for (int i=0; i<linha.size(); i++) linha[i] = toupper(linha[i]); //coloca todos os caracteres em maiúsculo

        // separa os elementos de cada linha e os coloca dentro de uma array
        vector<string> infos;
        stringstream ll(linha);
        string temp;
        while (ll >> temp)
            infos.push_back(temp);

        // verifica se foi passado algum comando
        if (infos[0][0]=='.'){
            comando = infos[0].substr(1);
            cmd = validaComando(comando); //indica qual comando foi passado
            continue;
        }

        // identifica o tipo de componente e o valida
        char tipo = infos[0][0];
        if (!validaTipo(tipo)) {
            cerr << "Tipo de componente \"" << tipo << "\" na linha "<< nroLinha << " NAO valido!" << endl;
            return -1;
        }
        if ( (tipo!='M' && tipo!='D') && nroInfo(tipo)!=infos.size()) {
            cerr << "Tipo de componente \"" << tipo << "\" recebe " << nroInfo(tipo) << " argumentos, "
                << infos.size() << " informados para " << infos[0] << " na linha " << nroLinha << "!" << endl;
            return -1;
        }
        if (tipo=='D' && (infos.size()>6 || infos.size()<4) ){ //valida diodo
            cerr << "Tipo de componente \"" << tipo << "\" recebe no minimo 4 e no maximo 6 argumentos, "
                << infos.size() << " informados para " << infos[0] << " na linha " << nroLinha << "!" << endl;
            return -1;
        }
        if (tipo=='M' && (infos.size()>10 || infos.size()<5) ){ //valida mosfet
            cerr << "Tipo de componente \"" << tipo << "\" recebe no minimo 5 e no maximo 10 argumentos, "
                << infos.size() << " informados para " << infos[0] << " na linha " << nroLinha << "!" << endl;
            return -1;
        }
        // armazena a label do componente
        string label = infos[0];

        // armazena informação dos nós (o no3 também pode ser elemento de controle para fontes controladas por corrente)
        string no1=infos[1], no2=infos[2], no3_controle, no4;
        if (nroInfo(tipo)>4)
            no3_controle = infos[3];
        if (nroInfo(tipo)>5)
            no4 = infos[4];

        // armazena o valor do componente
        string val = infos[infos.size()-1];
        if (tipo=='Q'){
            comps.inserir_final(tipo, label, no1, no2, no3_controle, val);
        }
        else if (tipo=='M'){
            // verifica se o modelo é válido
            string modelo = infos[4];
            if (modelo.compare("NMOS") != 0) {
                cerr << "Modelo " + modelo + " invalido!" << endl;
                return -1;
            }
            // adiciona os parâmetros passados a mais, ou coloca os valores padrão
            double par_K=110e-6, par_VTH=0.7, par_LAMBDA=0.04, par_W=5e-6, par_L=1e-6;
            if (infos.size()>nroInfo(tipo)){
                for (int it=nroInfo(tipo); it<infos.size(); it++){
                    if (infos[it].find("K'") != string::npos) par_K = validaValor(infos[it].substr(3));
                    else if (infos[it].find("VTH") != string::npos) par_VTH = validaValor(infos[it].substr(4));
                    else if (infos[it].find("LAMBDA") != string::npos) par_LAMBDA = validaValor(infos[it].substr(7));
                    else if (infos[it].find("W") != string::npos) par_W = validaValor(infos[it].substr(2));
                    else if (infos[it].find("L") != string::npos) par_L = validaValor(infos[it].substr(2));
                    else {
                        cerr << "Parametro extra " << infos[it] << " passado para componente " << label << " na linha "<< nroLinha << " invalido!";
                        return -1;
                    }
                }
            }
            comps.inserir_final(tipo, label, no1, no2, no3_controle, par_K, par_VTH, par_LAMBDA, par_W, par_L, modelo);
        }
        else if (tipo=='D'){
            // verifica se o modelo é válido
            string modelo = infos[3];
            if (modelo.compare("DIODEM1") != 0) {
                cerr << "Modelo " + modelo + " invalido!" << endl;
                return -1;
            }
            // adiciona os parâmetros passados a mais, ou coloca os valores padrão
            double par_IS=1e-12, par_N=1.5;
            if (infos.size()>nroInfo(tipo)){
                for (int it=nroInfo(tipo); it<infos.size(); it++){
                    if (infos[it].find("IS") != string::npos) par_IS = validaValor(infos[it].substr(3));
                    else if (infos[it].find("N") != string::npos) par_N = validaValor(infos[it].substr(2));
                    else {
                        cerr << "Parametro extra passado para componente " << label << " na linha "<< nroLinha << " invalido!";
                        return -1;
                    }
                }
            }
            comps.inserir_final(tipo, label, no1, no2, par_IS, par_N, modelo);
        }
        else if (tipo=='E'||tipo=='G'){
            if (!verificaValor(val)) {
                cerr << "Valor passado para componente \"" << infos[0] << "\" na linha " << nroLinha << " NAO valido!" << endl;
                return -1;
            }
            double valor = validaValor(val);
            comps.inserir_final(tipo, label, no1, no2, no3_controle, no4, valor);
        }
        else if (tipo=='F'||tipo=='H'){
            if (!verificaValor(val)) {
                cerr << "Valor passado para componente \"" << infos[0] << "\" na linha " << nroLinha << " NAO valido!" << endl;
                return -1;
            }
            double valor = validaValor(val);
            comps.inserir_final(tipo, label, no1, no2, no3_controle, valor);
        }
        else {
            if (!verificaValor(val)) {
                cerr << "Valor passado para componente \"" << infos[0] << "\" na linha " << nroLinha << " NAO valido!" << endl;
                return -1;
            }
            double valor = validaValor(val);
            comps.inserir_final(tipo, label, no1, no2, valor);
        }

    }
    if (cmd>0) cout << "Comando " + comando + " passado." << endl;
    return cmd;
}
