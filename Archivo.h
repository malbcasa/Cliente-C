#ifndef _ARCHIVO_H
#define _ARCHIVO_H


#include <string>
#include<list>
#include<vector>
#include <fstream>
using namespace std;

class Figura;
class Mapa;

class Archivo {
  private:
    string nombre;

    string contenido;

    int lineas;
	
	ofstream _fichero;


  public:
    void set_nom();

    string get_nom();

    void set_cont(string cont);
	
	string set_cont();
	
	void* open();
	
	void write(string msg);
	
	void close(Archivo* fich);

};
#endif
