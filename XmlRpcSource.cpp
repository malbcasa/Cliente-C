
#include "XmlRpcSource.h"
#include "XmlRpcSocket.h"
#include "XmlRpcUtil.h"
#include "Archivo.h"

namespace XmlRpc {


  XmlRpcSource::XmlRpcSource(int fd /*= -1*/, bool deleteOnClose /*= false*/) 
    : _fd(fd), _deleteOnClose(deleteOnClose), _keepOpen(false)
  {
  }

  XmlRpcSource::~XmlRpcSource()
  {
  }


  void
  XmlRpcSource::close(Archivo* flog, int verbosity)
  {
    if (_fd != -1) {
		if(verbosity >0){
			flog->write(XmlRpcUtil::log(2,"XmlRpcSource::close: closing socket %d.", _fd));
		}
	  XmlRpcSocket::close(_fd,verbosity);
		if(verbosity >0){
			flog->write(XmlRpcUtil::log(2,"XmlRpcSource::close: done closing socket %d.", _fd));
		}
      _fd = -1;
    }
    if (_deleteOnClose) {
		if(verbosity > 0 ){
			flog->write(XmlRpcUtil::log(2,"XmlRpcSource::close: deleting this"));
		}
      
      _deleteOnClose = false;
      delete this;
    }
  }

  void
	  XmlRpcSource::close()
  {
	 
	    XmlRpcUtil::log(2,"XmlRpcSource::close: closing socket %d.", _fd);
		 
		XmlRpcSocket::close(_fd);
		  
		XmlRpcUtil::log(2,"XmlRpcSource::close: done closing socket %d.", _fd);
		  
		  _fd = -1;
	  
	  if (_deleteOnClose) {
		  XmlRpcUtil::log(2,"XmlRpcSource::close: deleting this");
		  _deleteOnClose = false;
		  delete this;
	  }
  }
  
} // namespace XmlRpc
