#include <winsock2.h> 
#include <ws2tcpip.h>
#include <windows.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#include <iostream>


//struct for TCP
#pragma pack(push, x, 1)
typedef struct _PLAYERINFO_
{
	char _id;
	char _hSpot;  //where the player is hiding
	char _target; //where the player is aiming
	char _pLives = '3';
	char _opLives = '3';
} PLAYERINFO, * PPLAYERINFO;

//function prototypes
void PrintPlayerBoard(PPLAYERINFO p);
char GetPlayerHidingSpot();
char GetPlayerTarget();
void ExecutePlayerTurn(PPLAYERINFO p);
void DetermineResults(PPLAYERINFO p1, PPLAYERINFO p2);
void DetermineWinner(PPLAYERINFO p1);


//constants
const unsigned short SIZE_OF_PACKET = sizeof(PLAYERINFO);
const unsigned short PORT = 3399;


int main()
{
	SOCKADDR_IN addr;	 // socket address structure
	SOCKET ClientSocket;
	WSADATA wsaData;     // Windows Socket API data
	int sockErr;

	// start the Windows Socket API version 2.2 (we might not get this version)
	WSAStartup(MAKEWORD(2, 2), &wsaData); // sometimes you will see version 1,1

	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT); // host to network short

	//TODO: Change ADDRESS AS NEEDED!!!
	inet_pton(AF_INET, "64.72.1.242", &addr.sin_addr);

	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	

	if (ClientSocket == INVALID_SOCKET)
	{
		std::cout << "Error connecting: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	sockErr = connect(ClientSocket, (sockaddr*)&addr, sizeof(addr));

	if (sockErr == SOCKET_ERROR)
	{

		closesocket(ClientSocket);
		WSACleanup();

		return 1;
	}

	//player objects
	PLAYERINFO P1;
	PPLAYERINFO pPlayer1 = &P1;
	unsigned short turnNum = 1;

	//player waits for a second player from the server
	int ret = 0;
	while (ret == 0)
	{
		ret = recv(ClientSocket, (char*)pPlayer1, sizeof(pPlayer1), 0);
	}

	while (pPlayer1->_pLives > '0' && pPlayer1->_opLives > '0')
	{
		std::cout << "\t----- Turn # " << turnNum << " -----" << std::endl;
		ExecutePlayerTurn(pPlayer1);
		std::cout << "\n\n\n\n" << std::endl;
		sockErr = send(ClientSocket, (char *)pPlayer1, SIZE_OF_PACKET, 0);

		//wait for data from the server.
		int ret = recv(ClientSocket, (char *)pPlayer1, SIZE_OF_PACKET, 0);

		turnNum++;
	}
	DetermineWinner(pPlayer1);

	// Clean up
	pPlayer1 = nullptr; 
	closesocket(ClientSocket); 
	WSACleanup();
	system("pause");
	return 0;
}


//Functions
void PrintPlayerBoard(PPLAYERINFO p)
{
	std::cout << "Hiding Spot:\t\tTarget: " << std::endl;
	std::cout << "\t1 |\t\t | 1\n" << std::endl;
	std::cout << "\t2 |\t\t | 2\n" << std::endl;
	std::cout << "\t3 |\t\t | 3\n" << std::endl;
	std::cout << "Remaining lives: " << p->_pLives << "\tOpponents lives: " << p->_opLives << "\n" << std::endl;
}

//function definitions
char GetPlayerHidingSpot()
{
	char choice = '0';

	while (choice < '1' || choice > '3') //make sure the user provides a valid choice
	{
		std::cout << "Where would you like to hide? (type 1-3): ";
		std::cin >> choice;
		if (choice < '1' || choice > '3') //Error checking
		{
			std::cout << "Selection out of bounds!" << std::endl;
		}
	}

	return choice;
}

char GetPlayerTarget()
{
	char choice = '0';

	while (choice < '1' || choice > '3') //make sure the user provides a valid choice
	{
		std::cout << "Where would you like to target? (type 1-3): ";
		std::cin >> choice;
		if (choice < '1' || choice > '3') //Error checking
		{
			std::cout << "Selection out of bounds!" << std::endl;
		}
	}

	return choice;
}

void ExecutePlayerTurn(PPLAYERINFO p)
{
	PrintPlayerBoard(p);
	p->_hSpot = GetPlayerHidingSpot();
	p->_target = GetPlayerTarget();
}

void DetermineResults(PPLAYERINFO p1, PPLAYERINFO p2)
{
	if (p1->_hSpot == p2->_target)
	{
		p1->_pLives -= 1;
		std::cout << "Player " << p2->_id << " hit Player " << p1->_id << "\n" << std::endl;
	}

	if (p2->_hSpot == p1->_target)
	{
		p2->_pLives -= 1;
		std::cout << "Player " << p1->_id << " hit Player " << p2->_id << "\n" << std::endl;
	}

	if (p1->_hSpot != p2->_target && p2->_hSpot != p1->_target)
	{
		std::cout << "This round is a draw. \n" << std::endl;
	}

	p1->_opLives = p2->_pLives;
	p2->_opLives = p1->_pLives;
}

void DetermineWinner(PPLAYERINFO p1)
{
	std::cout << "----- FINAL RESULTS -----" << std::endl;
	std::cout << "Player " << p1->_id << " lives left: " << p1->_pLives << std::endl;
	std::cout << "Opponent lives left: " << p1->_opLives << std::endl;

	if (p1->_pLives != '0')
	{
		std::cout << "Congratulations Player " << p1->_id << ", you've won!" << std::endl;
	}
	else
	{
		std::cout << "You lost, better luck next time." << std::endl;
	}
}
