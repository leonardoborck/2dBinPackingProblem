#ifndef NODO_H
#define NODO_H

#include <iostream>
#include <vector>
#include "TipoDeNodo.h"
#include "TipoOrientacao.h"

using namespace std;

class Nodo
{
public:
	int Height;
	int Length;
	int CodigoDoItem;
	TipoDeNodo Type;
	vector<Nodo> Children;

public:
	Nodo(int height, int length);
	Nodo(int height, int length, vector<Nodo> children);
	Nodo(int height, int length, int codigoDoItem);
	void criaItem(int height, int length, int codigoDoItem);
	void criaEstrutura(int height, int length, vector<Nodo> children);
	void criaSobra(int height, int length);
};

#endif

