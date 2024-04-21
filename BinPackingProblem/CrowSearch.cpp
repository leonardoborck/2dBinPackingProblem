#include <iostream>
#include <vector>

#include "Crow.h"
#include "classes\Item.h"
#include "classes\Recipiente.h"

//rand
#include <stdlib.h>
#include <time.h>

using namespace std;

vector<Crow> PopulacaoDeCorvos;
vector<Item> Itens;
vector<Recipiente> Recipientes;

void inicializaCorvos(int tamanhoDaPopulacao) {
	srand(time(NULL));
	for (int i = 0; i < tamanhoDaPopulacao; i++) {
		Crow novoCorvo(Itens, Recipientes);
		novoCorvo.GeraSolucaoInicialAleatoria();
		novoCorvo.CorrigeSolucoesInviaveis();
		PopulacaoDeCorvos.push_back(novoCorvo);
	}
}


