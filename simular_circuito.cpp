/*

Universidade Federal do Rio Grande do Sul
Departamento de Engenharia El�trica
ENG04062 � T�picos Especiais em Eletr�nica I
Prof. Raphael M. Brum

Projeto 2: Solu��o de Circuitos El�tricos Lineares

LUCAS KIRSTEN - 230262

Descri��o: o programa faz a leitura de um arquivo com nome passado por linha de comando ao chamar a execu��o do mesmo. O arquivo deve estar com
formata��o SPICE. Ser� feita a leitura dos componentes presentes no arquivo e, caso haja alguma incoer�ncia, o programa ser� interrompido e uma
mensagem de erro ser� mostrada no console. Para a solu��o do circuito � necess�rio passar o comando <.OP> dentro do arquivo SPICE. O algoritmo
� capaz de resolver circuitos n�o-lineares contendo diodos e MOSFETs tipo NMOS.

Para resolu��o dos circuitos, ser� utilizado o m�todo do refinamento iterativo caso n�o haja componentes n�o lineares no circuito. Entretanto, caso
haja componentes n�o-lineares no circuito, ser� utilizado o m�todo modificado de Newton.

Foi atribu�do erro relativo de 1e-3, erro absoluto de 1e-9 e um m�ximo de 1000 itera��es para converg�ncia. Esses valores s�o pass�veis de munda�a
nos defines logo abaixo.

Observa��o: O programa foi desenvolvido em WINDOWS 10 e n�o h� garantia de funcionar em outro sistema operacional!

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

    //insere os componentes e verifica se foi passado algum comando de execu��o
    int comando = insere_comp(argv[1]);

    // mostra os componentes se n�o ocorrer nenhum erro
    if (comando>=0)
        comps.mostrar();

    // resolve o sistema com fatora��o LU e refinamento iterativo
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
