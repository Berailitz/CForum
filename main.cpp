#include <iostream>
#include "controller.h"
#include "utility.h"

using namespace std;

int main()
{
	cforum::Controller c;
    c.registerUser("admin", "admin");
	c.addBoard("TestBoard");
	c.postThread("Test thread", "This is content.");
	c.postComment("Comment 1");
	c.save("data");
	cout << "Exit.";
    cin.get();
	return 0;
}
