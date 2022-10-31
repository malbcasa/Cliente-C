
#include "XmlRpcClient.h"

#include "XmlRpcSocket.h"
#include "XmlRpc.h"
#include "XmlRpcException.h"



#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <fstream>
#include "Archivo.h"

using namespace XmlRpc;

// Static data
const char XmlRpcClient::REQUEST_BEGIN[] = 
  "<?xml version=\"1.0\"?>\r\n"
  "<methodCall><methodName>";
const char XmlRpcClient::REQUEST_END_METHODNAME[] = "</methodName>\r\n";
const char XmlRpcClient::PARAMS_TAG[] = "<params>";
const char XmlRpcClient::PARAMS_ETAG[] = "</params>";
const char XmlRpcClient::PARAM_TAG[] = "<param>";
const char XmlRpcClient::PARAM_ETAG[] =  "</param>";
const char XmlRpcClient::REQUEST_END[] = "</methodCall>\r\n";
const char XmlRpcClient::METHODRESPONSE_TAG[] = "<methodResponse>";
const char XmlRpcClient::FAULT_TAG[] = "<fault>";
const std::vector <std::string> comands{"help","setupConnection","switchlog","getreporte","close","exit"};


XmlRpcClient::XmlRpcClient(const char* host, int port, const char* uri/*=0*/)
{
	if(XmlRpc::getVerbosity() > 0){ 
		_log = new Archivo;
		_log -> open();
		_log -> write(XmlRpcUtil::log(1, "XmlRpcClient new client: host %s, port %d.", host, port));
	}
  std::cout << "\n\n**********Bienvenido a Veneris Client  Â®**********\n" ;
  std::cout << "Lista de comandos disponibles\n";
  _host = host;
  _port = port;
  this->help();
  if (uri)
    _uri = uri;
  else
    _uri = "/RPC2";
  _connectionState = NO_CONNECTION;
  _executing = false;
  _eof = false;

  // Default to keeping the connection open until an explicit close is done
  setKeepOpen();
}


XmlRpcClient::XmlRpcClient(){
		
	std::cout << "\n\n**********Bienvenido a Veneris Client ®**********\n" ;
	std::cout << "Lista de comandos disponibles\n";
	
	_uri = "/RPC2";
	_connectionState = NO_CONNECTION;
	_executing = false;
	_eof = false;
	
	_host = "vacio";
	this->help();
	// Default to keeping the connection open until an explicit close is done
	setKeepOpen();
}

XmlRpcClient::~XmlRpcClient()
{
}

Archivo* XmlRpcClient::getloghandlerf(){
	return _log;
}

int XmlRpcClient::getVerbosity(){
	return XmlRpc::getVerbosity();
}

void XmlRpcClient::getreporte(){
	XmlRpcValue noArgs,result;
	
	try{
		this->execute("getcomandos",noArgs,result);
		std::cout << "Estado de la conexión: Online\n";
		try{
			std::cout << "Estado robot : ";
			this -> interpreta("svgetestadopuertoserie");
			std::cout << "\nEl numero de ordenes hasta el momento es: ";
			this -> interpreta("svgetnumordenes");
			std::cout << "\nLos comandos ejecutados son:\n";
			this -> interpreta("svgetlistaordenes");
			
			
		}catch(XmlRpcException ex){
			std::cout << ex.getMessage() << "\n" ;
			std::cout << "Por el momento no se puede recuperar más información";
		}
	}catch(XmlRpcException ex){
		std::cout << "Estado de la conexión: Offline\nNo se puede recuperar más información por el momento\n";
	}
		
}
void XmlRpcClient::interpreta(std::string cmd){
	
	if(cmd.length() > 0){
		if( cmd.at(0) == 's'  && cmd.at(1)=='v'){
			cmd = cmd.substr(2,cmd.length());
			std::string Argsstr = "0";
			int espacios = 0;
			
			for(size_t i = 0 ; i < cmd.length(); i++){
				if(isspace(cmd[i])){
					if (espacios ==0){
						Argsstr = cmd.substr(i+1,cmd.length());	
					}
					
					espacios += 1;
				}
			}
			
			cmd = cmd.substr(0,cmd.find_first_of(" \n"));
			XmlRpcValue* noArgs;
			XmlRpcValue multiargs;
			XmlRpcValue result;
			
			if (espacios <=1){
				if(Argsstr.at(0) != '0'){
					noArgs = new XmlRpcValue(Argsstr);
				}else{
					noArgs = new XmlRpcValue();
				}
				try{
					this->execute(cmd.c_str(), *noArgs, result);
					std::cout <<  result << "\n";
				}catch(XmlRpcException ex){
					std::cout << ex.getMessage() << "\n" ;
				}
			}else if(espacios >1){
				for(int i=0;i<espacios;i++){
					multiargs[i]=(XmlRpcValue(stoi(Argsstr.substr(0,Argsstr.find(" "))) ));
					Argsstr.erase(0,Argsstr.find(" ")+1);
				}
				
				try{
					this->execute(cmd.c_str(), multiargs, result);
					std::cout <<  result << "\n";
				}catch(XmlRpcException ex){
					std::cout << ex.getMessage() << "\n" ;
				}
			}
			
			
			
			
			
		}
		else{
			try{
				if (cmd.substr(0,cmd.find_first_of(" \n")) == "help"){
					if (cmd.find("-") != -1){
						this -> help(cmd.substr(cmd.find("-")+1));
					}else{
						this -> help();
					}					
				}else if(cmd == "setupConnection"){
					this -> setupConnection();
				}else if(cmd == "close"){
					this -> close();
				}else if(cmd == "getreporte"){
					this -> getreporte();
				}else if(cmd == "switchlog"){
					if (this->getVerbosity() == 0){
						XmlRpc::setVerbosity(5);
						_log = new Archivo;
						_log -> open();
						std::cout << "Log Activado\n";
					}else if(this->getVerbosity() == 5){
						XmlRpc::setVerbosity(0);
						std::cout << "Log Desactivado\n";
					}
				}else if(cmd == "exit"){
					if (XmlRpc::getVerbosity()){
						_log ->close(_log);
					}
					try{
						XmlRpcValue noArgs,result ;
						this-> execute("cerrararchivoexterno",noArgs,result);
					}catch(XmlRpcException ex){
						std::cout << ex.getMessage() << "\n" ;
					}
				}else{
					throw("Comando no reconocido\n");
				}
			}catch(const char* s){
				std::cout << s; 
			}
		}
	}
}
void XmlRpcClient::help(){
	std::cout << "Comandos ejecutados por el cliente: \n";
	for (int i = 0; i < comands.size() ; i++){
		std::cout << comands.at(i) << "\n";
	}
	if( _host != "vacio"){
		std::cout << "Comandos ejecutados por el servidor: (recuerde agregar el prefijo sv a estos comandos, ej.: svcomando1 arg1 arg2)\n";
		this -> interpreta("svgetcomandos");
	}else{
		std::cout << "Conectese al servidor para recibir información de los camandos ejecutados por el servidor\n";
	}
	std::cout << "('help' muestra los comandos disponibles, 'help -comando' muestra ayuda relativa a comando\n";
}

void XmlRpcClient::help(string comando){
	if (comando=="modomanual"){
		cout<<"Modo manual(Aprendizaje): Permite al usuario controlar el robot manualmente y almacenar los comandos en un archivo para posterios replicacion"<<endl;
	}
	if(comando=="modoautomatico"){
		cout<<"Modo automatico(Replicacion): Permite al usuario replicar los comandos almacenados en un archivo de texto"<<endl;
	}
	if(comando=="turnonport"){
		cout<<"TurnOnPort: Enciende el puerto serie del robot"<<endl;
	}
	if(comando=="turnoffport"){
		cout<<"TurnOffPort: Apaga el puerto serie del robot"<<endl;
	}
	if(comando=="setmotores"){
		cout<<"SetMotores: Permite al usuario controlar los motores del robot"<<endl;
	}
	if(comando=="setposicionlineal"){
		cout<<"SetPosicionLineal: Permite al usuario establecer una coordenada para un despl. lineal y la velocidad"<<endl;
	}
	if(comando=="setangularmotor1"){
		cout<<"SetAngularMotor1: Permite al usuario establecer un angulo de despl. para el motor1, su velocidad y sentido"<<endl;
	}
	if(comando=="setangularmotor2"){
		cout<<"SetAngularMotor2: Permite al usuario establecer un angulo de despl. para el motor2, su velocidad y sentido"<<endl;
	}
	if(comando=="setangularmotor3"){
		cout<<"SetAngularMotor3: Permite al usuario establecer un angulo de despl. para el motor3, su velocidad y sentido"<<endl;
	}
	if(comando=="setpinza"){
		cout<<"SetPinza: Permite al usuario habilitar o deshabilitar la pinza del robot"<<endl;
	}
	if(comando=="reset"){
		cout<<"Reset: Permite al usuario llevar al robot a su posicion de reposo/origen (HOMING)"<<endl;
	}
	if(comando=="estadorobot"){
		cout<<"EstadoRobot: Permite al usuario conocer el estado de conexion al robot, servidor y numero de ordenes"<<endl;
	}
	if(comando=="listarcomandos"){
		cout<<"ListarComandos: Permite al usuario conocer la lista de comandos disponibles"<<endl;
	}
	if(comando=="exit"){
		cout<<"Exit: Permite al usuario salir de este CLI cliente"<<endl;
	}
	
}
// Close the owned fd
void 
XmlRpcClient::close()
{
	if(XmlRpc::getVerbosity() > 0){ 
		_log -> write(XmlRpcUtil::log(4, "XmlRpcClient::close: fd %d.", getfd()));
	}
  _connectionState = NO_CONNECTION;
  _disp.exit();
  _disp.removeSource(this);
  XmlRpcSource::close(_log,XmlRpc::getVerbosity());
}


// Clear the referenced flag even if exceptions or errors occur.
struct ClearFlagOnExit {
  ClearFlagOnExit(bool& flag) : _flag(flag) {}
  ~ClearFlagOnExit() { _flag = false; }
  bool& _flag;
};

// Execute the named procedure on the remote server.
// Params should be an array of the arguments for the method.
// Returns true if the request was sent and a result received (although the result
// might be a fault).
bool 
XmlRpcClient::execute(const char* method, XmlRpcValue const& params, XmlRpcValue& result)
{
	if(XmlRpc::getVerbosity() > 0){ 
		_log->write(XmlRpcUtil::log(1, "XmlRpcClient::execute: method %s (_connectionState %d).", method, _connectionState));
	}
  // This is not a thread-safe operation, if you want to do multithreading, use separate
  // clients for each thread. If you want to protect yourself from multiple threads
  // accessing the same client, replace this code with a real mutex.
  if (_executing)
    return false;

  _executing = true;
  ClearFlagOnExit cf(_executing);

  _sendAttempts = 0;
  _isFault = false;

  if ( ! setupConnection())
    return false;

  if ( ! generateRequest(method, params))
    return false;

  result.clear();
  double msTime = -1.0;   // Process until exit is called
  _disp.work(msTime);

  if (_connectionState != IDLE || ! parseResponse(result))
    return false;

  if(XmlRpc::getVerbosity() > 0){ 
	_log->write(XmlRpcUtil::log(1, "XmlRpcClient::execute: method %s completed.", method));
  }
  _response = "";
  return true;
}

// XmlRpcSource interface implementation
// Handle server responses. Called by the event dispatcher during execute.
unsigned
XmlRpcClient::handleEvent(unsigned eventType)
{
  if (eventType == XmlRpcDispatch::Exception)
  {
    if (_connectionState == WRITE_REQUEST && _bytesWritten == 0){
		_host = "vacio";
		XmlRpcUtil::error("Error in XmlRpcClient::handleEvent: could not connect to server (%s).", 
							    XmlRpcSocket::getErrorMsg().c_str());
		
    }else{
      XmlRpcUtil::error("Error in XmlRpcClient::handleEvent (state %d): %s.", 
                        _connectionState, XmlRpcSocket::getErrorMsg().c_str());
	}
    return 0;
  }

  if (_connectionState == WRITE_REQUEST)
    if ( ! writeRequest()) return 0;

  if (_connectionState == READ_HEADER)
    if ( ! readHeader()) return 0;

  if (_connectionState == READ_RESPONSE)
    if ( ! readResponse()) return 0;

  // This should probably always ask for Exception events too
  return (_connectionState == WRITE_REQUEST) 
        ? XmlRpcDispatch::WritableEvent : XmlRpcDispatch::ReadableEvent;
}


// Create the socket connection to the server if necessary
bool 
XmlRpcClient::setupConnection()
{
	
	if(_host == "vacio"){
		std::string host;
		int port;
		std::cout << "Ingrese direccion IP del servidor " ;
		std::cin >> host;
		std::cout << "Ingrese puerto del servidor " ;
		std::cin >> port;
		cin.ignore();
		if(XmlRpc::getVerbosity() > 0){ 
			_log = new Archivo;
			_log -> open();
			_log -> write(XmlRpcUtil::log(1, "XmlRpcClient new client: host %s, port %d.", host, port));
		}
		
	   _host = host;
	   _port = port;
	}
	
  // If an error occurred last time through, or if the server closed the connection, close our end
  if ((_connectionState != NO_CONNECTION && _connectionState != IDLE) || _eof)
    close();

  _eof = false;
  if (_connectionState == NO_CONNECTION)
    if (! doConnect()) 
      return false;

  // Prepare to write the request
  _connectionState = WRITE_REQUEST;
  _bytesWritten = 0;

  // Notify the dispatcher to listen on this source (calls handleEvent when the socket is writable)
  _disp.removeSource(this);       // Make sure nothing is left over
  _disp.addSource(this, XmlRpcDispatch::WritableEvent | XmlRpcDispatch::Exception);

  return true;
}


// Connect to the xmlrpc server
bool 
XmlRpcClient::doConnect()
{

  int fd = XmlRpcSocket::socket();
  if (fd < 0)
  {
	//_host = "vacio";
    XmlRpcUtil::error("Error in XmlRpcClient::doConnect: Could not create socket (%s).", XmlRpcSocket::getErrorMsg().c_str());
    return false;
  }
	
  if(XmlRpc::getVerbosity() > 0){ 
	_log -> write(XmlRpcUtil::log(3, "XmlRpcClient::doConnect: fd %d.", fd));
  }
  this->setfd(fd);

  // Don't block on connect/reads/writes
  if ( ! XmlRpcSocket::setNonBlocking(fd))
  {
    this->close();
    XmlRpcUtil::error("Error in XmlRpcClient::doConnect: Could not set socket to non-blocking IO mode (%s).", XmlRpcSocket::getErrorMsg().c_str());
    return false;
  }

  if ( ! XmlRpcSocket::connect(fd, _host, _port))
  {
    this->close();
	//_host = "vacio";
    XmlRpcUtil::error("Error in XmlRpcClient::doConnect: Could not connect to server (%s).", XmlRpcSocket::getErrorMsg().c_str());
    return false;
  }
 
  return true;
}

// Encode the request to call the specified method with the specified parameters into xml
bool 
XmlRpcClient::generateRequest(const char* methodName, XmlRpcValue const& params)
{
  std::string body = REQUEST_BEGIN;
  body += methodName;
  body += REQUEST_END_METHODNAME;

  // If params is an array, each element is a separate parameter
  if (params.valid()) {
    body += PARAMS_TAG;
    if (params.getType() == XmlRpcValue::TypeArray)
    {
      for (int i=0; i<params.size(); ++i) {
        body += PARAM_TAG;
        body += params[i].toXml();
        body += PARAM_ETAG;
      }
    }
    else
    {
      body += PARAM_TAG;
      body += params.toXml();
      body += PARAM_ETAG;
    }
      
    body += PARAMS_ETAG;
  }
  body += REQUEST_END;

  std::string header = generateHeader(body);
  if(XmlRpc::getVerbosity() > 0){ 
	  _log->write(XmlRpcUtil::log(4, "XmlRpcClient::generateRequest: header is %d bytes, content-length is %d.", 
                  header.length(), body.length()));
  }
  _request = header + body;
  return true;
}

// Prepend http headers
std::string
XmlRpcClient::generateHeader(std::string const& body)
{
  std::string header = 
    "POST " + _uri + " HTTP/1.1\r\n"
    "User-Agent: ";
  header += XMLRPC_VERSION;
  header += "\r\nHost: ";
  header += _host;

  char buff[40];
  sprintf(buff,":%d\r\n", _port);

  header += buff;
  header += "Content-Type: text/xml\r\nContent-length: ";

  sprintf(buff,"%d\r\n\r\n", body.size());

  return header + buff;
}

bool 
XmlRpcClient::writeRequest()
{
  if (_bytesWritten == 0){
	  if(XmlRpc::getVerbosity() > 0){ 
		  _log->write(XmlRpcUtil::log(5, "XmlRpcClient::writeRequest (attempt %d):\n%s\n", _sendAttempts+1, _request.c_str()));
		}
  }
  // Try to write the request
  if ( ! XmlRpcSocket::nbWrite(this->getfd(), _request, &_bytesWritten, this->getVerbosity())) {
    XmlRpcUtil::error("Error in XmlRpcClient::writeRequest: write error (%s).",XmlRpcSocket::getErrorMsg().c_str());
    return false;
  }
  if(XmlRpc::getVerbosity() > 0){ 
	_log->write(XmlRpcUtil::log(3, "XmlRpcClient::writeRequest: wrote %d of %d bytes.", _bytesWritten, _request.length()));
  }
  
  // Wait for the result
  if (_bytesWritten == int(_request.length())) {
    _header = "";
    _response = "";
    _connectionState = READ_HEADER;
  }
  return true;
}


// Read the header from the response
bool 
XmlRpcClient::readHeader()
{
  // Read available data
  if ( ! XmlRpcSocket::nbRead(this->getfd(), _header, &_eof) ||
       (_eof && _header.length() == 0)) {
	
    // If we haven't read any data yet and this is a keep-alive connection, the server may
    // have timed out, so we try one more time.
    if (getKeepOpen() && _header.length() == 0 && _sendAttempts++ == 0) {
		if(XmlRpc::getVerbosity() > 0){ 
			_log->write(XmlRpcUtil::log(4, "XmlRpcClient::readHeader: re-trying connection"));
		}
      XmlRpcSource::close(_log,XmlRpc::getVerbosity());
      _connectionState = NO_CONNECTION;
      _eof = false;
      return setupConnection();
    }

    XmlRpcUtil::error("Error in XmlRpcClient::readHeader: error while reading header (%s) on fd %d.",
                      XmlRpcSocket::getErrorMsg().c_str(), getfd());
    return false;
  }

	   if(XmlRpc::getVerbosity() > 0){
		   _log->write(XmlRpcUtil::log(4, "XmlRpcClient::readHeader: client has read %d bytes", _header.length()));
	   }

  char *hp = (char*)_header.c_str();  // Start of header
  char *ep = hp + _header.length();   // End of string
  char *bp = 0;                       // Start of body
  char *lp = 0;                       // Start of content-length value

  for (char *cp = hp; (bp == 0) && (cp < ep); ++cp) {
    if ((ep - cp > 16) && (strncasecmp(cp, "Content-length: ", 16) == 0))
      lp = cp + 16;
    else if ((ep - cp > 4) && (strncmp(cp, "\r\n\r\n", 4) == 0))
      bp = cp + 4;
    else if ((ep - cp > 2) && (strncmp(cp, "\n\n", 2) == 0))
      bp = cp + 2;
  }

  // If we haven't gotten the entire header yet, return (keep reading)
  if (bp == 0) {
    if (_eof)          // EOF in the middle of a response is an error
    {
      XmlRpcUtil::error("Error in XmlRpcClient::readHeader: EOF while reading header");
      return false;   // Close the connection
    }
    
    return true;  // Keep reading
  }

  // Decode content length
  if (lp == 0) {
    XmlRpcUtil::error("Error XmlRpcClient::readHeader: No Content-length specified");
    return false;   // We could try to figure it out by parsing as we read, but for now...
  }

  _contentLength = atoi(lp);
  if (_contentLength <= 0) {
    XmlRpcUtil::error("Error in XmlRpcClient::readHeader: Invalid Content-length specified (%d).", _contentLength);
    return false;
  }
  	
  if(XmlRpc::getVerbosity() > 0){ 
	  _log->write(XmlRpcUtil::log(4, "client read content length: %d", _contentLength));
  }

  // Otherwise copy non-header data to response buffer and set state to read response.
  _response = bp;
  _header = "";   // should parse out any interesting bits from the header (connection, etc)...
  _connectionState = READ_RESPONSE;
  return true;    // Continue monitoring this source
}

    
bool 
XmlRpcClient::readResponse()
{
  // If we dont have the entire response yet, read available data
  if (int(_response.length()) < _contentLength) {
    if ( ! XmlRpcSocket::nbRead(this->getfd(), _response, &_eof)) {
      XmlRpcUtil::error("Error in XmlRpcClient::readResponse: read error (%s).",XmlRpcSocket::getErrorMsg().c_str());
      return false;
    }

    // If we haven't gotten the entire _response yet, return (keep reading)
    if (int(_response.length()) < _contentLength) {
      if (_eof) {
        XmlRpcUtil::error("Error in XmlRpcClient::readResponse: EOF while reading response");
        return false;
      }
      return true;
    }
  }

  // Otherwise, parse and return the result
  if(XmlRpc::getVerbosity() > 0){ 
	  _log->write(XmlRpcUtil::log(3, "XmlRpcClient::readResponse (read %d bytes)", _response.length()));
	  _log->write(XmlRpcUtil::log(5, "response:\n%s", _response.c_str()));
	}
  _connectionState = IDLE;

  return false;    // Stop monitoring this source (causes return from work)
}


// Convert the response xml into a result value
bool 
XmlRpcClient::parseResponse(XmlRpcValue& result)
{
  // Parse response xml into result
  int offset = 0;
  if ( ! XmlRpcUtil::findTag(METHODRESPONSE_TAG,_response,&offset)) {
    XmlRpcUtil::error("Error in XmlRpcClient::parseResponse: Invalid response - no methodResponse. Response:\n%s", _response.c_str());
    return false;
  }

  // Expect either <params><param>... or <fault>...
  if ((XmlRpcUtil::nextTagIs(PARAMS_TAG,_response,&offset) &&
       XmlRpcUtil::nextTagIs(PARAM_TAG,_response,&offset)) ||
      XmlRpcUtil::nextTagIs(FAULT_TAG,_response,&offset) && (_isFault = true))
  {
    if ( ! result.fromXml(_response, &offset)) {
      XmlRpcUtil::error("Error in XmlRpcClient::parseResponse: Invalid response value. Response:\n%s", _response.c_str());
      _response = "";
      return false;
    }
  } else {
    XmlRpcUtil::error("Error in XmlRpcClient::parseResponse: Invalid response - no param or fault tag. Response:\n%s", _response.c_str());
    _response = "";
    return false;
  }
      
  _response = "";
  return result.valid();
}

