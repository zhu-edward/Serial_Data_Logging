using namespace std;

#include <string>
#include <stdio.h>

int main (void) {
	std::string buffer = "$Ax:-0.877822, Ay:0.992961, Az:-0.953830, t:20.956000!119\n";
	std::string startChar = "$";
	std::string endChar = "!";
	std::size_t foundStart, foundEnd;
	int chksum = 0;
	std::string::size_type sz;

	foundStart = buffer.find(startChar);
	foundEnd = buffer.find(endChar);

	if ((foundStart != std::string::npos) && (foundEnd != std::string::npos)) {
		int msgChksum = std::stoi(buffer.substr((foundEnd+1), (buffer.length()-foundEnd-2)), &sz);
		std::string msg = buffer.substr(foundStart, (foundEnd-foundStart+1));
		for (unsigned int i = 0; i <= (msg.length()-1); i++) {
			chksum ^= (int)msg[i];
		}
		printf("The checksum from the message is %i\n", msgChksum);
		printf("The calcuated checksum is %i\n", chksum);
	}
	else {
		exit(EXIT_FAILURE);
	}
}