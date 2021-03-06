# Solucionador de Circuitos (Circuit-Solver)

Programa simples desenvolvido para solucionar circuitos lineares utilizando a idéia da matriz MNA não-linear. O programa é capaz de resolver corretamente circuitos contendo resistores, fontes independentes, fontes dependentes e diodos. Foi implementada a solução com transistores MOS, mas ainda não está funcionando corretamente.

>A simple circuit solver using the non-linear MNA matrix. It can solve circuits with resistors, independent and dependent source and diodes. It also has the implementation for MOS transistor, but it not seems to work.

## Guia para o usuário (User guide)

**Este tutorial só foi testado para sistema operacional Windows!**

Escreva o circuito que você queira solucionar em formatação SPICE dentro de um arquivo de extensão *.sp* como ilustrado abaixo.

```
* Testcase 2-3 (1-7)

H_Hesq	b	0	R_Rdi_4	6250m
R_Resq	a	0	20
I_Icima	a	b	5
R_Rmeio	a	c	10
R_Rdi_1	c	b	40
R_Rdi_2	c	b	60
V_Vdir	c	d	10
R_Rdi_3	d	0	20
R_Rdi_4 c	0	20	 

.op
```

O programa implementa apenas a opção de resolução *.op*. A escrita NÃO é afetada por letras maiúsculas e minúsculas.

Para simular os valores do circuito, navegue até a pasta onde se encontra o arquivo **simular_cirucito.exe** e execute o comando:

```
simular_circuito.exe <NOME DO ARQUIVO>.sp
```

Uma lista de circuitos exemplo estão na pasta *testes* junto com suas saídas em formato *.out*.

Para maiores informações se refira as códigos.

## Organização do código (Code's organization)

O programa foi feito em C++ no Windows 10 e compilado no mesmo. Não foi utilizada nenhuma biblioteca além das padrões instaladas com o g++ e dos arquivos extras que estão sendo enviados em conjunto no arquivo. Os arquivos estão assim organizados:

- **leitura_componentes.cpp** : faz a leitura do arquivo em formato SPICE, procurando erros;

- **lista_encadeada.cpp** : contém as informações dos componentes guardados em memória;

- **nao_lineares.cpp** : contém as classes responsáveis por lidar com os componentes não lineares diodo e MOSFET tipo NMOS;

- **resolve_sistema.cpp** : contém toda a classe responsável por montar a matriz MNA, fazer a fatoração LU, a resolução do sistema, o refinamento iterativo e o método de Newton;

- **simular_circuito.cpp** : contém a chamada da função "main" com todos os processos necessários para a solução do circuito.
