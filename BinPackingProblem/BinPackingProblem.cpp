#include <iostream>
#include <json/json.h>
#include <fstream>

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

bool AdicionaItem(Nodo novoFilho, Nodo& raiz) {
	if (raiz.Children.empty() && raiz.Type == TipoDeNodo::Leftover) {//adiciona item
		if (novoFilho.Height <= raiz.Height && novoFilho.Length <= raiz.Length)//item cabe
		{
			int sobraDoCorteHorizontal = (raiz.Height - novoFilho.Height) * raiz.Length; // ---
			int sobraDoCorteVertical = (raiz.Length - novoFilho.Length) * raiz.Height; // |

			int alturaDaRaiz = raiz.Height;
			int larguraDaRaiz = raiz.Length;

			if (novoFilho.Height < raiz.Height && novoFilho.Height < raiz.Height) { //CORTE 2 NIVEIS

				bool primeiroCorteEhHorizontal = sobraDoCorteHorizontal > sobraDoCorteVertical;
				TipoOrientacao orientacaoDoPrimeiroCorte = primeiroCorteEhHorizontal ? TipoOrientacao::H : TipoOrientacao::V;
				TipoOrientacao orientacaoDoSegundoCorte = primeiroCorteEhHorizontal ? TipoOrientacao::V : TipoOrientacao::H;

				Nodo segundoResto(0, 0);

				int alturaDoSegundoResto, larguraDoSegundoResto;

				if (orientacaoDoPrimeiroCorte == TipoOrientacao::H) // resto verdadeiro
				{
					raiz.Height -= novoFilho.Height;

					segundoResto.Height = alturaDoSegundoResto = novoFilho.Height;
					segundoResto.Length = larguraDoSegundoResto = raiz.Length;
					segundoResto.Length -= novoFilho.Length;

				}
				else {
					raiz.Length -= novoFilho.Length;

					segundoResto.Height = alturaDoSegundoResto = raiz.Height;
					segundoResto.Length = larguraDoSegundoResto = novoFilho.Length;
					segundoResto.Height -= novoFilho.Height;
				}

				vector<Nodo> children, children2;

				children2.push_back(novoFilho);
				children2.push_back(segundoResto);

				Nodo segundoCorte(alturaDoSegundoResto, larguraDoSegundoResto, children2);

				children.push_back(segundoCorte);
				children.push_back(raiz);

				Nodo novaRaiz(alturaDaRaiz, larguraDaRaiz, children);

				raiz = novaRaiz;
			}

			else { //corte 1 nivel
				TipoOrientacao orientacao
					= sobraDoCorteHorizontal > sobraDoCorteVertical
					? TipoOrientacao::H : TipoOrientacao::V;

				if (orientacao == TipoOrientacao::H)
					raiz.Height -= novoFilho.Height;
				else
					raiz.Length -= novoFilho.Length;

				vector<Nodo> children;

				children.push_back(novoFilho);
				children.push_back(raiz);

				Nodo novaRaiz(alturaDaRaiz, larguraDaRaiz, children);

				raiz = novaRaiz;

			}
			return true;
		}
		return false;
	}
	else {
		for (int i = 0; i < raiz.Children.size(); i++) {
			if (AdicionaItem(novoFilho, raiz.Children[i])) {
				return true;
			}
		}
	}

	return false;
}

void GerenciaProcessoDeAdicaoDeItens(vector<Item>& itensFaltantes, Nodo& raiz)
{
	while (!itensFaltantes.empty())
	{
		int indexDoItem = itensFaltantes.size() != 1 ? rand() % (itensFaltantes.size() - 1) : 0;

		Item itemEscolhido = itensFaltantes[indexDoItem];

		Nodo novoFilho(itemEscolhido.Height, itemEscolhido.Length, itemEscolhido.Reference);

		if (AdicionaItem(novoFilho, raiz))
			itensFaltantes.erase(itensFaltantes.begin() + indexDoItem);
		/*else 
		{
			Nodo novaArvore(Recipientes[0].Height, Recipientes[0].Height);
			Arvores.push_back(novaArvore);
		}*/

	}
}

int main()
{
	srand(time(NULL));

	ifstream base_file("base\\CLASS01_020_01.json", ifstream::binary);

	Json::Value dadosDoJson;
	base_file >> dadosDoJson;

	converteDeJsonParaObjeto(dadosDoJson["Items"], true);
	converteDeJsonParaObjeto(dadosDoJson["Objects"], false);

	int alturaDoRecipiente = Recipientes[0].Height;
	int larguraDoRecipiente = Recipientes[0].Height;

	Nodo raiz(alturaDoRecipiente, larguraDoRecipiente);

	vector<Item> itensFaltantes = Itens;

	GerenciaProcessoDeAdicaoDeItens(itensFaltantes, raiz);

	return 0;
}