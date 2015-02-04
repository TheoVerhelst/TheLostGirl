#ifndef SCRIPTSFUNCTIONS_H
#define SCRIPTSFUNCTIONS_H

#include <iostream>

#include <entityx/Entity.h>

int checkFoes();
int attack(int a, float b, bool c);
template <typename T>
int print(T val);

template <typename T>
int print(T val)
{
	if(typeid(val) == typeid(bool))
		std::cout << (val ? "True\n" : "False\n");
	else
		std::cout << val << "\n";
}

#endif// SCRIPTSFUNCTIONS_H
