#pragma warning(disable : 4996)
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"

bool InitializeWindowsSockets();

int  main(void)
{
	// Socket used for listening for new clients 
	SOCKET listenSocket = INVALID_SOCKET;
	// koliko klijenta toliko acceptedSocketa
	SOCKET acceptedSocket[2];
	for (int i = 0; i < 2; i++)
		acceptedSocket[i] = INVALID_SOCKET;
	// variable used to store function return value
	int iResult;
	// Buffer used for storing incoming data
	char recvbuf[DEFAULT_BUFLEN];

	if (InitializeWindowsSockets() == false)
	{
		// we won't log anything since it will be logged
		// by InitializeWindowsSockets() function
		return 1;
	}

	// Prepare address information structures
	addrinfo* resultingAddress = NULL;
	addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;       // IPv4 address
	hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
	hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
	hints.ai_flags = AI_PASSIVE;     // 

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &resultingAddress);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	listenSocket = socket(AF_INET,      // IPv4 address famly
		SOCK_STREAM,  // stream socket
		IPPROTO_TCP); // TCP

	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket - bind port number and local address 
	// to socket
	iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(resultingAddress);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	// Since we don't need resultingAddress any more, free it
	freeaddrinfo(resultingAddress);

	// Set listenSocket in listening mode
	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("Server initialized, waiting for clients.\n");

	int brojClienata = 0;

	do
	{
		// Wait for clients and accept client connections.
		// Returning value is acceptedSocket used for further
		// Client<->Server communication. This version of
		// server will handle only one client.
		if (brojClienata < 100)
		{
			acceptedSocket[brojClienata] = accept(listenSocket, NULL, NULL);
			if (acceptedSocket[brojClienata] == INVALID_SOCKET)
			{
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(listenSocket);
				WSACleanup();
				return 1;
			}
			brojClienata++;
		}

		unsigned long int nonBlockingMode = 1;
		iResult = ioctlsocket(acceptedSocket[0], FIONBIO, &nonBlockingMode);
		FD_SET set;
		timeval timeVal;
		FD_ZERO(&set);
		FD_SET(acceptedSocket[0], &set);
		timeVal.tv_sec = 0;
		timeVal.tv_usec = 0;

		do
		{
			FD_SET(acceptedSocket[0], &set);

			iResult = select(0, &set, NULL, NULL, &timeVal);

			if (iResult == -1)
			{
				printf("select failed with error: %d\n", WSAGetLastError());
				closesocket(acceptedSocket[0]);
			}
			else if (iResult > 0)
			{
				for (int i = 0; i < brojClienata; i++)
				{
					if (FD_ISSET(acceptedSocket[i], &set))
					{
						iResult = recv(acceptedSocket[i], recvbuf, DEFAULT_BUFLEN, 0);
						if (iResult > 0)
						{
							printf("Message received from client: %s.\n", recvbuf);
						}
						else if (iResult == 0)
						{
							// connection was closed gracefully
							printf("Connection with client closed.\n");
							closesocket(acceptedSocket[i]);
						}
						else
						{
							// there was an error during recv
							printf("recv failed with error: %d\n", WSAGetLastError());
							closesocket(acceptedSocket[i]);
						}
					}
				}

			}

			FD_ZERO(&set);
		} while (true);

		// here is where server shutdown loguc could be placed

	} while (1);

	// shutdown the connection since we're done
	for (int i = 0; i < brojClienata; i++)
	{
		iResult = shutdown(acceptedSocket[0], SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			printf("shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(acceptedSocket[0]);
			WSACleanup();
			return 1;
		}
	}

	// cleanup
	closesocket(listenSocket);
	closesocket(acceptedSocket[0]);
	WSACleanup();

	return 0;
}

bool InitializeWindowsSockets()
{
	WSADATA wsaData;
	// Initialize windows sockets library for this process
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}