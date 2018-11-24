#include <iostream>
#include "controller.h"
#include "utility"

using namespace std;

int main()
{
	cforum::Controller c;
    c.registerUser("admin", "admin");
    cin.get();
	return 0;
}
