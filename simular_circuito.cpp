/*

Universidade Federal do Rio Grande do Sul
Departamento de Engenharia Elétrica
ENG04062 – Tópicos Especiais em Eletrônica I
Prof. Raphael M. Brum

Projeto 2: Solução de Circuitos Elétricos Lineares

LUCAS KIRSTEN - 230262

Descrição: o programa faz a leitura de um arquivo com nome passado por linha de comando ao chamar a execução do mesmo. O arquivo deve estar com
formatação SPICE. Será feita a leitura dos componentes presentes no arquivo e, caso haja alguma incoerência, o programa será interrompido e uma
mensagem de erro será mostrada no console. Para a solução do circuito é necessário passar o comando <.OP> dentro do arquivo SPICE. O algoritmo
é capaz de resolver circuitos não-lineares contendo diodos e MOSFETs tipo NMOS.

Para resolução dos circuitos, será utilizado o método do refinamento iterativo caso não haja componentes não lineares no circuito. Entretanto, caso
haja componentes não-lineares no circuito, será utilizado o método modificado de Newton.

Foi atribuído erro relativo de 1e-3, erro absoluto de 1e-9 e um máximo de 1000 iterações para convergência. Esses valores são passíveis de mundaça
nos defines logo abaixo.

Observação: O programa foi desenvolvido em WINDOWS 10 e não há garantia de funcionar em outro sistema operacional!

*/

#define ABSTOL 1e-3
#define RELTOL 1e-9
#define MAXIT 100000

#include "resolve_sistema.cpp"

#include <iostream>
using std::cout;
using std::cerr;

int main( int argc, char *argv[] ){
    if( argc>2 ) {
        cerr << "Muitos argumentos passados, maximo 1!" << endl;
        return -1;
    }

    //insere os componentes e verifica se foi passado algum comando de execução
    int comando = insere_comp(argv[1]);

    // mostra os componentes se não ocorrer nenhum erro
    if (comando>=0)
        comps.mostrar();

    // resolve o sistema com fatoração LU e refinamento iterativo
    if (comando==1) {
        SolveSys sistema(comps, ABSTOL, RELTOL, MAXIT);
        if (vecDiodos.size()<1 && vecMosfet.size()<1)
            sistema.solveRefIt();
        else
            sistema.solveNewton();
        cout << "ABSTOL=" << ABSTOL << " ; RELTOL=" << RELTOL << " ; MAXIT=" << MAXIT;

        //sistema.showMatrix();
    }

	return 0;
}
