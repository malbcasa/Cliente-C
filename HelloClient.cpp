// HelloClient.cpp : A simple xmlrpc client. Usage: HelloClient serverHost serverPort
// Link against xmlrpc lib and whatever socket libs your system needs (ws2_32.lib 
// on windows)
#include "XmlRpc.h"
#include "XmlRpcSocket.h"
#include "XmlRpcException.h"
#include <iostream>
#include <string>

using namespace XmlRpc;

int main(int argc, char* argv[])
{
	bool on = true;
	std::string cmd;
	XmlRpcClient* c;
	c = new XmlRpcClient();
	std::cout << "Cliente operativo a la espera de comandos (0 para info sobre los comandos del cliente)\n" ;
	
	while(on){
		std::cout << "(esperando comando...)\n";
		std::getline(std::cin,cmd);
		c->interpreta(cmd);
		std::cin.ignore(500,'\n');
	}
		
	
  return 0;
}
