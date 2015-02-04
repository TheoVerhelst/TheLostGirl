#include <TheLostGirl/scripts/scriptsFunctions.h>

int checkFoes()
{
	std::cout << "Foes are checked...\n";
	return 0;
}

int attack(int a, float b, bool c)
{
	std::cout << "Attack " << a << " times with a strenght of " << b << ", with a " <<
		(c?"goood white":"bad dark") << " spell!\n";
    return 0;
}
