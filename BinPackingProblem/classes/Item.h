#ifndef ITEM_H
#define ITEM_H

#include <iostream>
using namespace std;

class Item
{
public:
	int Height;
	int Length;
	int Demand;
	int Value;
	int Reference;
	int IndexDaArvore;

	Item(int height, int length, int demand, int value, int reference);
};

#endif