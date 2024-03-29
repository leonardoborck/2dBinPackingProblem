#include <iostream>
#include <json/json.h>
#include <fstream>
#include <math.h>

//mede tempo de execucao
#include<chrono> 

//rand
#include <stdlib.h>
#include <time.h>

#include "classes/Item.h"
#include "classes/Recipiente.h"
#include "classes/Nodo.h"

using namespace std;

vector<Item> Itens;
vector<Recipiente> Recipientes;
vector<Nodo> Arvores;

void converteDeJsonParaObjeto(Json::Value dadosDoJson, bool ehConversaoDeItem)
{
	for (int i = 0; i < dadosDoJson.size(); i++) {
		Json::Value objeto = dadosDoJson[i];

		if (ehConversaoDeItem)
		{
			Item novoItem(objeto["Height"].asInt64(), objeto["Length"].asInt64(), objeto["Demand"].asInt64(), objeto["Value"].asInt64(), objeto["Reference"].asInt64());
			Itens.push_back(novoItem);
		}
		else
		{
			Recipiente novoRecipiente(objeto["Height"].asInt64(), objeto["Length"].asInt64(), objeto["Cost"].asInt64(), objeto["Reference"].asInt64());
			Recipientes.push_back(novoRecipiente);
		}
	}
}

bool AdicionaItem(Item itemEscolhido, Nodo& raizAtual, Nodo& raizAnterior)
{
	if (raizAtual.Children.empty() && raizAtual.Type == TipoDeNodo::Leftover) {//adiciona item
		Nodo novoFilho(itemEscolhido.Height, itemEscolhido.Length, itemEscolhido.Reference);

		if (novoFilho.Height <= raizAtual.Height && novoFilho.Length <= raizAtual.Length)//item cabe
		{
			int sobraDoCorteHorizontal = (raizAtual.Height - novoFilho.Height) * raizAtual.Length; // ---
			int sobraDoCorteVertical = (raizAtual.Length - novoFilho.Length) * raizAtual.Height; // |

			int alturaDaRaiz = raizAtual.Height;
			int larguraDaRaiz = raizAtual.Length;

			if (novoFilho.Height < raizAtual.Height && novoFilho.Length < raizAtual.Length)  //CORTE 2 NIVEIS
			{

				bool primeiroCorteEhHorizontal = sobraDoCorteHorizontal > sobraDoCorteVertical;

				TipoOrientacao orientacaoDoPrimeiroCorte = primeiroCorteEhHorizontal ? TipoOrientacao::H : TipoOrientacao::V;
				TipoOrientacao orientacaoDoSegundoCorte = primeiroCorteEhHorizontal ? TipoOrientacao::V : TipoOrientacao::H;

				Nodo segundoResto(0, 0);

				int alturaDoSegundoResto, larguraDoSegundoResto;

				if (primeiroCorteEhHorizontal) // resto verdadeiro
				{
					raizAtual.Height -= novoFilho.Height;

					segundoResto.Height = alturaDoSegundoResto = novoFilho.Height;
					segundoResto.Length = larguraDoSegundoResto = raizAtual.Length;
					segundoResto.Length -= novoFilho.Length;

				}
				else
				{
					raizAtual.Length -= novoFilho.Length;

					segundoResto.Height = alturaDoSegundoResto = raizAtual.Height;
					segundoResto.Length = larguraDoSegundoResto = novoFilho.Length;
					segundoResto.Height -= novoFilho.Height;
				}

				vector<Nodo> children, children2;

				children2.push_back(novoFilho);
				children2.push_back(segundoResto);

				Nodo segundoCorte(alturaDoSegundoResto, larguraDoSegundoResto, orientacaoDoSegundoCorte, children2);

				if (raizAnterior.Orientation == orientacaoDoPrimeiroCorte) //caso tenha 2 cortes iguais em sequencia, apenas ajusta o tamanho do resto e adiciona filho na lista.
				{
					raizAnterior.Children.push_back(segundoCorte);
					return true;
				}

				children.push_back(segundoCorte);
				children.push_back(raizAtual);

				Nodo novaRaiz(alturaDaRaiz, larguraDaRaiz, orientacaoDoPrimeiroCorte, children);

				raizAtual = novaRaiz;

				return true;
			}

			else //corte 1 nivel
			{
				TipoOrientacao orientacao
					= sobraDoCorteHorizontal > sobraDoCorteVertical
					? TipoOrientacao::H : TipoOrientacao::V;

				if (orientacao == TipoOrientacao::H)
					raizAtual.Height -= novoFilho.Height;
				else
					raizAtual.Length -= novoFilho.Length;

				if (raizAtual.Height == 0 || raizAtual.Length == 0)
				{
					raizAtual = novoFilho;
					return true;
				}

				if (raizAnterior.Orientation == orientacao) //caso tenha 2 cortes iguais em sequencia, apenas ajusta o tamanho do resto e adiciona filho na lista.
				{
					raizAnterior.Children.push_back(novoFilho);
					return true;
				}

				vector<Nodo> children;

				children.push_back(novoFilho);
				children.push_back(raizAtual);

				Nodo novaRaiz(alturaDaRaiz, larguraDaRaiz, orientacao, children);

				raizAtual = novaRaiz;

				return true;
			}
		}
		return false;
	}
	else
	{
		for (int i = 0; i < raizAtual.Children.size(); i++)
		{
			if (AdicionaItem(itemEscolhido, raizAtual.Children[i], raizAtual)) return true;
		}
	}

	return false;
}

void AjustaIndexDaArvoreNoItemAdicionado(int codigoDoItem, int indexDaArvore = -1)
{
	for (int i = 0; i < Itens.size(); i++)
		if (Itens[i].Reference == codigoDoItem)
			Itens[i].IndexDaArvore = indexDaArvore;
}


void GerenciaProcessoDeAdicaoDeItens(vector<Item>& itensFaltantes)
{
	int alturaDoRecipiente = Recipientes[0].Height;
	int larguraDoRecipiente = Recipientes[0].Height;

	Nodo arvore(alturaDoRecipiente, larguraDoRecipiente);
	Arvores.push_back(arvore);

	while (!itensFaltantes.empty())
	{
		int indexDoItem = itensFaltantes.size() != 1 ? rand() % (itensFaltantes.size() - 1) : 0;

		Item itemEscolhido = itensFaltantes[indexDoItem];

		bool adicionou = false;

		for (int i = 0; i < Arvores.size(); i++)
		{
			if (AdicionaItem(itemEscolhido, Arvores[i], Arvores[i])) {
				itensFaltantes.erase(itensFaltantes.begin() + indexDoItem);
				adicionou = true;
				AjustaIndexDaArvoreNoItemAdicionado(itemEscolhido.Reference, i);
				break;
			}
		}

		if (!adicionou)
		{
			Nodo novaArvore(alturaDoRecipiente, larguraDoRecipiente);

			if (AdicionaItem(itemEscolhido, novaArvore, novaArvore)) {
				itensFaltantes.erase(itensFaltantes.begin() + indexDoItem);
				AjustaIndexDaArvoreNoItemAdicionado(itemEscolhido.Reference, Arvores.size());
			}

			Arvores.push_back(novaArvore);
		}

	}
}

void AjustaArvoreAposARemocao(Nodo& raiz)
{
	int numeroDeRestos = 0, numeroDeNaoRestos = 0;
	for (int i = 0; i < raiz.Children.size(); i++)
	{
		switch (raiz.Children[i].Type)
		{
		case TipoDeNodo::Leftover:
			numeroDeRestos++;
			break;
		default:
			numeroDeNaoRestos++;
			break;
		}
	}

	if (numeroDeRestos <= 1) //resto que acabei de transformar
		return;

	if (numeroDeRestos == 2 && numeroDeNaoRestos >= 1) //soma restos e tranforma em apenas 1
	{
		bool ehOPrimeiroResto = true;
		int indiceDoPrimeiroResto;
		Nodo primeiroResto(0, 0), segundoResto(0, 0);

		for (int i = 0; i < raiz.Children.size(); i++)
		{
			if (raiz.Children[i].Type == TipoDeNodo::Leftover)
			{
				if (ehOPrimeiroResto)
				{
					primeiroResto = raiz.Children[i];
					indiceDoPrimeiroResto = i;

					ehOPrimeiroResto = false;
				}
				else
				{
					if (raiz.Orientation == TipoOrientacao::H)
						raiz.Children[i].Height += primeiroResto.Height;
					else
						raiz.Children[i].Length += primeiroResto.Length;

					raiz.Children.erase(raiz.Children.begin() + indiceDoPrimeiroResto); //remove primeiro resto
					return;
				}
			}
		}
	}

	raiz.TransformaEmResto();

}

bool RemoveItem(int codigoDoItem, Nodo& raizAtual) //fazer remoção em 2 etapas, primeiro tranforma esse item em resto e depois normaliza a arvore
{
	if (raizAtual.Children.empty() && raizAtual.CodigoDoItem == codigoDoItem)
	{
		AjustaIndexDaArvoreNoItemAdicionado(raizAtual.CodigoDoItem);
		raizAtual.TransformaEmResto();

		return true;
	}
	else
	{
		for (int i = 0; i < raizAtual.Children.size(); i++)
		{
			if (RemoveItem(codigoDoItem, raizAtual.Children[i])) {
				AjustaArvoreAposARemocao(raizAtual);
				return true;
			}
		}
	}
	return false;
}


int VerificaUtilizacaoDaArvore(Nodo raizAtual)
{
	if (raizAtual.Type == TipoDeNodo::Item)
	{
		return raizAtual.Height * raizAtual.Length;
	}
	else
	{
		int valor = 0;
		for (int i = 0; i < raizAtual.Children.size(); i++)
		{
			valor += VerificaUtilizacaoDaArvore(raizAtual.Children[i]);
		}

		return valor;
	}
}

double AvaliaSolucao(vector<Nodo> arvores, int k)
{
	double somaDaUtilizacaoDasArvores = 0;
	for (Nodo arvore : arvores)
	{
		double utilizacao = pow((double)VerificaUtilizacaoDaArvore(arvore) / (arvore.Height * arvore.Length), k);
		somaDaUtilizacaoDasArvores += utilizacao;
	}

	return somaDaUtilizacaoDasArvores / arvores.size();
}

void AdicionaItemAEsquerda(vector<Nodo>& arvoresLocal, Item item, int& indexDaArvoreEmQueOItemFoiAdicionado)
{
	bool adicionou = false;
	for (int i = 0; i < arvoresLocal.size(); i++)
	{
		if (AdicionaItem(item, arvoresLocal[i], arvoresLocal[i])) {
			adicionou = true;
			indexDaArvoreEmQueOItemFoiAdicionado = i;
			break;
		}
	}

	if (!adicionou)
	{
		Nodo novaArvore(10, 10);
		AdicionaItem(item, novaArvore, novaArvore);
		arvoresLocal.push_back(novaArvore);
		indexDaArvoreEmQueOItemFoiAdicionado = arvoresLocal.size() - 1;
	}
}

bool ItemShuffle()
{
	//Escolher 2 Arvores e 1 Item em cada Arvore
	int indexDoPrimeiroItem = rand() % Itens.size();
	int indexDoSegundoItem;
	do
	{
		indexDoSegundoItem = rand() % Itens.size();
	} while (Itens[indexDoPrimeiroItem].IndexDaArvore == Itens[indexDoSegundoItem].IndexDaArvore);

	//TROCA DE ARVORE
	Item primeiroItem = Itens[indexDoPrimeiroItem], segundoItem = Itens[indexDoSegundoItem];

	int indexDaPrimeiraArvore = primeiroItem.IndexDaArvore;
	int indexDaSegundaArvore = segundoItem.IndexDaArvore;

	vector<Nodo> arvoresLocal(Arvores);

	RemoveItem(primeiroItem.Reference, arvoresLocal[indexDaPrimeiraArvore]);
	RemoveItem(segundoItem.Reference, arvoresLocal[indexDaSegundaArvore]);

	bool deuCerto1 = AdicionaItem(primeiroItem, arvoresLocal[indexDaSegundaArvore], arvoresLocal[indexDaSegundaArvore]);
	bool deuCerto2 = AdicionaItem(segundoItem, arvoresLocal[indexDaPrimeiraArvore], arvoresLocal[indexDaPrimeiraArvore]);


	int indexAux1 = indexDaPrimeiraArvore, indexAux2 = indexDaSegundaArvore;
	if (!deuCerto1) AdicionaItemAEsquerda(arvoresLocal, primeiroItem, indexAux2);
	if (!deuCerto2) AdicionaItemAEsquerda(arvoresLocal, segundoItem, indexAux1);

	double valor1 = AvaliaSolucao(Arvores, 2);
	double valor2 = AvaliaSolucao(arvoresLocal, 2);

	if (valor1 < valor2)
	{
		Itens[indexDoPrimeiroItem].IndexDaArvore = indexAux2;
		Itens[indexDoSegundoItem].IndexDaArvore = indexAux1;
		Arvores = arvoresLocal;
		return true;
	}

	Itens[indexDoPrimeiroItem].IndexDaArvore = indexDaPrimeiraArvore;
	Itens[indexDoSegundoItem].IndexDaArvore = indexDaSegundaArvore;
	return false;
}

void Heuristica(int numeroDeIteracoes)
{
	int cont = 0;
	while (cont < numeroDeIteracoes) {
		int operacao = rand() % 3; // saber operação 0 - Item-Shuffle - 1 - Bin-Shake - 2 Mistura
		bool safe = false;

		switch (operacao)
		{
		case 0:
			safe = ItemShuffle();
			break;
		case 1:
			safe = ItemShuffle();
			break;
		case 2:
			safe = ItemShuffle();
			break;
		}
		if (!safe)
			cont++;
		//cout << "utilizacao: " << AvaliaSolucao(Arvores, 2) << "\t cont:" << cont << endl;
	}
}

//PRA DEBUGAR
//vector<int> itensRep;
//void VerificaSeTemItensRepetidos(Nodo raizAtual)
//{
//	if (raizAtual.Type == TipoDeNodo::Item)
//	{
//		itensRep.push_back(raizAtual.CodigoDoItem);
//	}
//	else
//	{
//		int valor = 0;
//		for (int i = 0; i < raizAtual.Children.size(); i++)
//		{
//			VerificaSeTemItensRepetidos(raizAtual.Children[i]);
//		}
//
//	}
//}
//
//void Checa(vector<Nodo>arvores) 
//{
//	for (Nodo arvore : arvores)
//	{
//		VerificaSeTemItensRepetidos(arvore);
//	}
//}

#include <algorithm>
int main()
{
	srand(time(NULL));

	ifstream base_file("base\\CLASS01_020_01.json", ifstream::binary);

	Json::Value dadosDoJson;
	base_file >> dadosDoJson;

	converteDeJsonParaObjeto(dadosDoJson["Items"], true);
	converteDeJsonParaObjeto(dadosDoJson["Objects"], false);

	vector<Item> itensFaltantes = Itens;

	GerenciaProcessoDeAdicaoDeItens(itensFaltantes);
	cout << "utilizacao: " << AvaliaSolucao(Arvores, 2) << endl;

	//avalia o tempo da heuristica
	auto start = chrono::steady_clock::now();
	Heuristica(300);
	auto end = chrono::steady_clock::now();
	auto elapsed = end - start;

	cout << "utilizacao: " << AvaliaSolucao(Arvores, 2) << endl;

	cout << ((double)elapsed.count()/ 1000000000) << "s\n";

	return 0;
}