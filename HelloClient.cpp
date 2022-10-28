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
	c -> clhelp();
	
	
	while(on){
		std::cout << "(esperando comando...)\n";
		std::getline(std::cin,cmd);		
		if(cmd=="exit"){
			on = false;
			break;
		}
		c->interpreta(cmd);

	}
		
	
  return 0;
}
