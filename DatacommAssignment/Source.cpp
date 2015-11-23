#include <iostream>
#include "Protocol.h"

using namespace std;

int main() {
	char q;

	Protocol protocol;
	protocol.sendMessage("y super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super duper duper long message.Here is my super", true);
	cout << protocol.priority << endl;
	cout << protocol.messagesToSend.size() << endl;
	for (auto i : protocol.messagesToSend)
		cout << i << endl << endl;

	cout << A << endl;
	cin >> q;
}