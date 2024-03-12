#ifndef RECIPIENTE_H
#define RECIPIENTE_H

#include <iostream>
using namespace std;

class Recipiente
{
public:
	int Height;
	int Length;
	int Cost;
	int Reference;

	Recipiente(int height, int length, int cost, int reference);
};

#endif
