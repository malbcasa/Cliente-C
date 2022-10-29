#include <iostream>
#include "Archivo.h"

#include <string>
#include <fstream>
#include <list>
#include<vector>
#include <cctype>
#include<cstring>
#include<sstream>
#include <bits/stdc++.h>

void Archivo::set_nom() {
	 nombre = "log.txt";
}

void* Archivo::open(){
	this->set_nom();
	_fichero.open(nombre);
	if (!_fichero)
	{
		cout << "Error al abrir " << nombre; 
		exit(EXIT_FAILURE);
	}
	
}
string Archivo::get_nom() {	
	return nombre;
}

void Archivo::set_cont(string fname){
	ofstream fichero(fname);
	
	if (!fichero)
	{
		cout << "Error al abrir " << nombre; 
		exit(EXIT_FAILURE);
	}
};

void Archivo::write(string msg){
	msg.append("\n");
	_fichero << msg;
}

void Archivo::close(Archivo* fich){
	fich->_fichero.close();
}
/*
string Archivo::set_mapa(Archivo fich_in) {
	ofstream fichero(nombre);
	ifstream fichin(fich_in.get_nom());
	m = new Mapa;
	if (!fichero)
	{
		cout << "Error al abrir " << nombre; 
		exit(EXIT_FAILURE);
	}
	
	string resol1, resol2;
	getline(fichin,resol1,',');
	getline(fichin,resol2,',');
	string relleno;
	getline(fichin,relleno);
	(*m).set_resolu(stoi(resol1) ,stoi(resol2));
	int errores = 0;
	lineas = 0;
	int tq = 0;
	int cq = 0;
	int rq = 0;
	float areat = 0;
	
	contenido.append("Resolución: " + resol1 + "x" + resol2 + "\n") ; 
	contenido.append("Relleno personalizado: " + (*m).get_codigo() + "\n\n");
	contenido.append("Figura	Color	A/Centro	B/Radio/Base	C/Altura	Perimetro	Superficie\n");
	int val = 0;
	string cont;
	vector<string> datosmapa;
	while (!fichin.eof()){
		getline(fichin,cont);
		int espacios=0;
		vector <string> datosmapa;
		size_t j = 0;
		datosmapa.push_back(cont.substr(0,1));
		for(size_t i = 0 ; i < cont.length(); i++){
			if(isspace(cont[i])){
				espacios++;
				j = cont.find_first_of(",\n",i);
				if( j != std::string::npos){
					datosmapa.push_back(cont.substr(i,j-i));
				}else{
					datosmapa.push_back(cont.substr(i,cont.length()));
				}
			}
			
		}
		
		
		if(cont.substr(0,1) == "r"){
			if(espacios >= 4 ){
				string tipo = "r";
				vector<int> vert;
				int b,h;
				
				if ((datosmapa.size()) == 5){
					vert.push_back(stoi(datosmapa[1]));
					vert.push_back(stoi(datosmapa[2]));
					b = stoi(datosmapa[3]);
					h = stoi(datosmapa[4]);
				
				}else if((datosmapa.size()) == 6){
					vert.push_back(stoi(datosmapa[2]));
					vert.push_back(stoi(datosmapa[3]));
					b = stoi(datosmapa[4]);
					h = stoi(datosmapa[5]);
					
				}
				
				Rectangulo rect(vert,b,h);
				rect.set_tipo(tipo);
				val = (*m).set_validez(rect);
				if (val == 0){
					(*m).append_figura(rect);
					if((datosmapa.size()) == 5){
						contenido.append("Rectangulo 	9");
					}else if((datosmapa.size()) == 6){
						contenido.append("Rectangulo 	" + datosmapa[0]);
					}
					contenido.append("	"+ to_string(vert[0]) + "," + to_string(vert[1]));
					contenido.append("	" + to_string(b) + "		" + to_string(h));
					contenido.append("	" + to_string(rect.get_perimetro()) + "	" + to_string(rect.get_area())+ "\n");
					rq++;
					areat+=rect.get_area();
				}else{
					contenido.append("Error (fuera de rango)\n");
					errores++;
				}
			}else{
				contenido.append("Error (parametros invalidos)\n");
				errores++;
			}
		}else if(cont.substr(0,1) == "c"){
			if(espacios >= 3){
				
				string tipo = "c";
				vector<int> centro ;
				int radio;
				
				if ((datosmapa.size()) == 4){
					centro.push_back(stoi(datosmapa[1]));
					centro.push_back(stoi(datosmapa[2]));
					radio = stoi(datosmapa[3]);
					
				}else if((datosmapa.size()) == 5){
					centro.push_back(stoi(datosmapa[2]));
					centro.push_back(stoi(datosmapa[3]));
					radio = stoi(datosmapa[4]);
					
				}
				
				Circulo circ(centro,radio);
				circ.set_tipo(tipo);
				val = (*m).set_validez(circ);
				if (val == 0){
					(*m).append_figura(circ);
					if((datosmapa.size()) == 4){
						contenido.append("Circulo 	9");
					}else if((datosmapa.size()) == 5){
						contenido.append("Circulo 	" + datosmapa[1]);
					}
					contenido.append("	"+ to_string(centro[0]) + "," + to_string(centro[1]));
					contenido.append("	" + to_string(circ.get_radio()) + "			" );
					contenido.append("	" + to_string(circ.get_perimetro()) + "	" + to_string(circ.get_area())+ "\n");
					cq++;
					areat+=circ.get_area();
				}else{
					contenido.append("Error (fuera de rango)\n");
					errores++;
				}
				
			}else{
				contenido.append("Error (parametros invalidos)\n");
				errores++;
			}
		}else if(cont.substr(0,1) == "t"){
			if(espacios >= 6){
				string tipo = "t";
				vector<vector<int>> vert;
				vector<int> vertaux;
				
				if ((datosmapa.size()) == 7){
					for(int i=1; i <= 5 ; i += 2){
						vertaux.push_back(stoi(datosmapa[i]));
						vertaux.push_back(stoi(datosmapa[i+1]));
						vert.push_back(vertaux);
						vertaux.clear();
					}
				}else if((datosmapa.size()) == 8){
					for(int i=2; i <= 6 ; i += 2){
						vertaux.push_back(stoi(datosmapa[i]));
						vertaux.push_back(stoi(datosmapa[i+1]));
						vert.push_back(vertaux);
						vertaux.clear();
					}
				}
				Triangulo tri(vert);
				tri.set_tipo(tipo);
				
				val = (*m).set_validez(tri);
				if (val == 0){
					(*m).append_figura(tri);
					if((datosmapa.size()) == 7){
						contenido.append("Triangulo	9");
					}else if((datosmapa.size()) == 8){
						contenido.append("Triangulo	" + datosmapa[1]);
					}
					contenido.append("	"+ to_string((tri.get_vertice())[0][0]) + "," + to_string((tri.get_vertice())[0][1]));
					contenido.append("	" + to_string((tri.get_vertice())[1][0]) +","+to_string((tri.get_vertice())[1][1]));
					contenido.append("	" + to_string((tri.get_vertice())[2][0]) +","+to_string((tri.get_vertice())[2][1]));
					contenido.append("	" + to_string(tri.get_perimetro()) + "		" + to_string(tri.get_area())+ "\n");
					tq++;
					areat+=tri.get_area();
				}else{
					contenido.append("Error (fuera de rango)\n");
					errores++;
				}
				
			}else{

				contenido.append("Error (parametros invalidos)\n");
				errores++;
			}
		}else{
			contenido.append("Error (fig. no existe)\n");
			errores++;
		}
		
	}
	
	contenido.append("Lineas de entidad en el archivo: ");
	contenido.append(to_string(fich_in.get_lineas()));
	contenido.append("\n");
	
	if (errores > 0){
		contenido.append("Errores: ");
		contenido.append(to_string(errores));
		contenido.append("\n");
	}
	if( tq != 0 ){
		contenido.append("Triangulos: ");
		contenido.append(to_string(tq));
		contenido.append("\n");
	}
	if( rq != 0 ){
		contenido.append("Rectangulos: ");
		contenido.append(to_string(rq));
		contenido.append("\n");
	}
	if( cq != 0 ){
		contenido.append("Circulos: ");
		contenido.append(to_string(cq));
		contenido.append("\n");
	}
	contenido.append("%Areas: ");
	contenido.append(to_string(areat/((*m).get_resolu()[0] * (*m).get_resolu()[1]) *100));
	fichin.close();
	fichero << contenido;		
	fichero.close();
	
	return contenido;
}
*/
