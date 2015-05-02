#include <stdlib.h>
#include <stdio.h>
#include <time.h> //libreria para tomar numeros del timer de la maquina //Chris: tu vieja timer
#include <iostream>
#include <fstream>
#include <vector>

struct Matriz
{
	double** mat;		//Matriz entera, o Upper en LU 
	double** L;			//Lower en LU
	int n;				//Cantidad de coeficientes. La matriz seria de nxn
	int ancho;			//Cantidad de puntos de ancho en la discretizacion del parabrisas. Alto = n/ancho
	double * valores;	//b en Ax = b
	double * solution;	//x en Ax = b
	bool solved;		//Si esta resuelta
	double h;			//precision de discretizacion del parabrisas
	int pto;			//Indice de la temperatura en el Punto Critico. El x en valores[x]
	int sanguipre;		//Sanguijuela borrada para restaurarla. Si nunca se borro es -1
	std::vector<int> *sanguis;
	double * tempSangui;
	std::vector<int> *puntos;

public:
	double get(int y, int x){
		int xf = ancho - y + x;
		return (xf < 0 || xf > ancho * 2) ? 0 : this->mat[y][xf];
	}

	void set(int y, int x, double d){
		int xf = ancho - y + x;
		if (xf < 0 || xf > ancho * 2){
			std::cout << "Set choto y= " << y << " x= " << x << std::endl;
		}
		else{
			this->mat[y][xf] = d;
		}
	}
	double getL(int y, int x){
		int xf = ancho - y + x;
		return (xf < 0 || xf > ancho * 2) ? 0 : this->L[y][xf];
	}

	void setL(int y, int x, double d){
		int xf = ancho - y + x;
		if (xf < 0 || xf > ancho * 2){
			std::cout << "Set choto L y= " << y << " x= " << x << std::endl;
		}
		else {
			this->L[y][xf] = d;

		}
	}
};

/**
*
*	Valorizacion del parabrisas
*
**/

struct coordenada{
	double y;
	double x;
};

struct sanguijuela
{
	coordenada coord;
	double r;
	double t;
};

//chequea que la direccion no se vaya de rango, devuelve el valor valido
double chequearLimites(double coordenada, double principio, double limite){
	double result = coordenada;
	if (coordenada <= principio){
		result = principio;
	}
	else if(coordenada >= limite){
		result = limite - principio;
	}
	return result;
}

//cheuqea que el punto este dentro el diametro de la sanguijuela
bool pertenece(int x, int y, sanguijuela actual, double h){
	bool result = false;
	double sx = actual.coord.x;
	double sy = actual.coord.y;
	double r  = actual.r;
	if ((x*h - sx) * (x*h - sx) + (y*h - sy) * (y*h - sy) <= r*r){ //chequeo que (x-x0)^2 * (y-y0)^2 <= r^2
		result = true;
	}
	return result;
}

void obtenerPuntosDeCalor(sanguijuela* sanguiList, int nsangui, Matriz* mat){
	double h = mat->h;
	double alto = ((mat->n / mat->ancho) - 1) * h;
	double ancho = (mat->ancho - 1) * h;
	double * valores = mat->valores;
	mat->sanguis = new std::vector<int>[nsangui];
	mat->tempSangui = new double[nsangui];
	for (int i = 0; i < nsangui; i++){
		//seteo los bordes del cuadrado donde va a estar el circulo
		sanguijuela actual = sanguiList[i];
		double xizq, xder, yabj, yarr;
		xizq = chequearLimites(actual.coord.x - actual.r, h, ancho);
		xder = chequearLimites(actual.coord.x + actual.r, h, ancho);
		yabj = chequearLimites(actual.coord.y - actual.r, h, alto);
		yarr = chequearLimites(actual.coord.y + actual.r, h, alto);
		mat->tempSangui[i] = actual.t;
		for (int x = xizq / h; x <= xder / h; x++){
			for (int y = yabj / h; y <= yarr / h; y++){
				int pos = (x * ((alto / h) + 1)) + y;
				if (pertenece(x, y, actual, h)){
					mat->sanguis[i].push_back(pos);
					mat->puntos[pos].push_back(i);
					if (actual.t > valores[pos]){
						valores[pos] = actual.t;
						//printf("Sangui: %i - Toca(%i,%i) - Temp=%lf\n", i, y, x, actual.t);
					}
				}
			}
		}
	}
}

void recalcularFila(Matriz * mat, int punto){
	int anchbanda = (mat->ancho * 2) + 1;
	int ancho = mat->ancho;
	for (int c = 0; c < anchbanda; c++){ mat->mat[punto][c] = 0; }
	if ((punto % ancho == ancho - 1) ||
		(punto % ancho == 0) ||
		(punto < ancho) ||
		(punto > mat->n - ancho) ||
		(mat->valores[punto] != 0)){
		if (mat->valores[punto] == 0){
			mat->valores[punto] = -100;
		}
		mat->mat[punto][ancho] = 1;
	}
	else {
		mat->mat[punto][0] = 0.25;
		mat->mat[punto][ancho - 1] = 0.25;
		mat->mat[punto][ancho + 1] = 0.25;
		mat->mat[punto][ancho * 2] = 0.25;
		mat->mat[punto][ancho] = -1;
	}
}

void regenerarSinSangui(Matriz * mat, int isangui){
	//Restauro la sanguijuela borrada anterior, si existe
	mat->solved = false;
	int sanguipre = mat->sanguipre;
	if (sanguipre != -1){
		for (int x = 0; x < mat->sanguis[sanguipre].size(); x++){
			int puntoactual = mat->sanguis[sanguipre][x];
			mat->valores[puntoactual] = 0;
			for (int p = 0; p < mat->puntos[puntoactual].size(); p++){
				int sanguienpunto = mat->puntos[puntoactual][p];
				if (mat->tempSangui[sanguienpunto] > mat->valores[puntoactual]){
					mat->valores[puntoactual] = mat->tempSangui[sanguienpunto];
				}
			}
			recalcularFila(mat, puntoactual);
		}
	}
	//Borro la sanguijuela pedida y proceso
	for (int x = 0; x < mat->sanguis[isangui].size(); x++){
		int puntoactual = mat->sanguis[isangui][x];
		mat->valores[puntoactual] = 0;
		for (int p = 0; p < mat->puntos[puntoactual].size(); p++){
			int sanguienpunto = mat->puntos[puntoactual][p];
			if (sanguienpunto != isangui && mat->tempSangui[sanguienpunto] > mat->valores[puntoactual]){
				mat->valores[puntoactual] = mat->tempSangui[sanguienpunto];
			}
		}
		recalcularFila(mat, puntoactual);
	}
	mat->sanguipre = isangui;
}

/**
*
*	Manipulacion de matriz
*
**/


void GenerarMatriz(Matriz* mat, int alto, int ancho, double h, sanguijuela* sanguiList, int nsangui){
	mat->h = h;
	mat->ancho = ancho;
	mat->solved = false;
	mat->n = ancho*alto;
	mat->solution = new double[mat->n];
	mat->mat = new double*[mat->n];
	mat->sanguipre = -1;
	int n = mat->n;
	mat->puntos = new std::vector<int>[n];
	//Inicializamos valores y luego la cargamos con las temperaturas que corresponden
	mat->valores = new double[n];
	for (int x = 0; x < n; x++){ mat->valores[x] = 0; }
	//Carga las temperaturas fijandose que sanguijuela toca que punto
	obtenerPuntosDeCalor(sanguiList, nsangui, mat);

	//Inicializa la matriz Lower
	int anchbanda = (ancho * 2) + 1;
	mat->L = new double*[n];
	for (int x = 0; x < n; x++){
		mat->L[x] = new double[anchbanda];
		for (int c = 0; c < anchbanda; c++){ mat->L[x][c] = 0; }
		mat->L[x][ancho] = 1;
	}

	//Inicializamos y construimos las bandas para este sistema
	for (int x = 0; x < mat->n; x++){
		mat->mat[x] = new double[anchbanda];
		for (int c = 0; c < anchbanda; c++){ mat->mat[x][c] = 0; }
		if ((x % ancho == ancho - 1) ||
			(x % ancho == 0) ||
			(x < ancho) ||
			(x > n - ancho) ||
			(mat->valores[x] != 0)){
			if (mat->valores[x] == 0){
				mat->valores[x] = -100;
			}
			mat->mat[x][ancho] = 1;
		}
		else {
			mat->mat[x][0] = 0.25;
			mat->mat[x][ancho - 1] = 0.25;
			mat->mat[x][ancho + 1] = 0.25;
			mat->mat[x][ancho * 2] = 0.25;
			mat->mat[x][ancho] = -1;
		}
	}

}

void mostrarF(Matriz* mat, char * name){
	FILE * out;
	out = fopen(name, "w");
	int n = mat->n;
	int xf = mat->ancho;
	int yf = n / xf;
	for (int y = 0; y < yf; y++){
		for (int x = 0; x < xf; x++){
			fprintf(out, "%i\t%i\t%.5lf\n", y, x, mat->solution[y*xf + x]);
			//out << y << "\t" << x << "\t" << mat->valores[y*xf + x] << std::endl;
		}
	}
	fclose(out);
}

void mostrarMF(Matriz* mat){
	std::ofstream out;
	out.open("matr");
	int i, j;
	for (i = 0; i < mat->n; i++){
		for (j = 0; j < mat->n; j++){
			out << mat->get(i, j) << " ";
		}
		out << std::endl;
	}
	printf("\nPrinteado a matr\n");
	out.close();

}

void mostrarLF(Matriz* mat){
	std::ofstream out;
	out.open("matr");
	int i, j;
	for (i = 0; i < mat->n; i++){
		for (j = 0; j < mat->n; j++){
			out << mat->L[i][j] << " ";
		}
		out << std::endl;
	}
	printf("\nPrinteado a matr\n");
	out.close();

}

void mostrarBandas(Matriz * mat){
	printf("\n-----------------------\n");
	for (int i = 0; i < mat->n; i++){
		for (int j = 0; j < (mat->ancho * 2 + 1); j++){
			printf("%#5.2lf ", mat->mat[i][j]);
		}
		printf("\n");
	}
	printf("\n-----------------------\n");
}

void mostrarSolucion(Matriz * mat){
	int n = mat->n;
	int xf = mat->ancho;
	int yf = n / xf;
	for (int y = 0; y < yf; y++){ printf("---"); }
	printf("\n|");
	for (int y = yf - 1; 0 <= y; y--){
		for (int x = 0; x < xf; x++){
			printf("%#7.2lf ", mat->solution[y*xf + x]);
		}
		printf("|\n|");
	}
	for (int y = 0; y < yf; y++){ printf("---"); }
	printf("\nLISTO PRINT!\n");
}

void mostrarPosiciones(Matriz * mat){
	int n = mat->n;
	int xf = mat->ancho;
	int yf = n / xf;
	for (int y = 0; y < yf; y++){ printf("---"); }
	printf("\n|");
	for (int y = 0; y < yf; y++){
		for (int x = 0; x < xf; x++){
			printf("%03i ", y*xf + x);
		}
		printf("|\n|");
	}
	for (int y = 0; y < yf; y++){ printf("---"); }
}

/**
*
*	Eliminación Gauseana
*
**/

void eliminacionGauseana(Matriz* mat){
	int i, j, pivote = 0;
	int filas = mat->n;
	int columnas = mat->n;
	int ancho = mat->ancho;
	long double multiplicador;
	for (int x = 0; x < columnas; x++){ mat->solution[x] = mat->valores[x]; }
	for (pivote = 0; pivote < filas - 1; ++pivote){
		int endi;
		if (pivote + ancho + 1 <= mat->n) { endi = pivote + ancho + 1; }
		else { endi = mat->n; }
		for (i = pivote + 1; i < endi; i++){
			multiplicador = mat->get(i, pivote) / mat->get(pivote, pivote);
			int endj;
			if (i + ancho + 1 <= mat->n) { endj = i + ancho + 1; }
			else { endj = mat->n; }
			for (j = pivote; j < endj; j++){
				mat->set(i, j, mat->get(i, j) - (multiplicador * mat->get(pivote, j)));
			}
			mat->valores[i] = mat->valores[i] - multiplicador*mat->valores[pivote];
			//mat->set(i, pivote, 0);
		}
	}
}

void backwardSubstitution(Matriz* mat){
	double y;
	int je;
	for (int i = mat->n - 1; i >= 0; i--){
		y = mat->valores[i];
		if (i + mat->ancho + 1 > mat->n) { je = mat->n; } else { je = i + 1 + mat->ancho; }		//Adaptacion a banda
		for (int j = i + 1; j < je; j++){
			y = y - mat->get(i, j)* mat->valores[j];
		}
		mat->solution[i] = y / mat->get(i, i);
	}
	mat->solved = true;
}

/**
*
*	Factorización LU
*
**/

void setDiagonalUnos(Matriz * mat){
	for(int i=0; i< mat->n ; i++){
		mat->setL(i,i,1);
	}
}

void descomposicionLU(Matriz * mat){
   	int i, j, pivote = 0;
	int n = mat->n;
	int ancho = mat->ancho;
	double multi;
	for (pivote = 0; pivote < n - 1; ++pivote){
		int endi;
		if (pivote + ancho + 1 <= mat->n) { endi = pivote + ancho + 1; }
		else { endi = mat->n; }
		for (i = pivote + 1; i < endi; i++){	
			multi = mat->get(i,pivote) / mat->get(pivote,pivote);
			int endj;
			if (i + ancho + 1 <= mat->n) { endj = i + ancho + 1; }
			else { endj = mat->n; }
			for (j = pivote; j < endj; j++){
				mat->set(i,j,mat->get(i,j) - (multi * mat->get(pivote, j)));
			}
			mat->setL(i, pivote, multi);
		}
	}
	setDiagonalUnos(mat);
}

void fordwardSubstitution(Matriz * mat){
	double y;
	int n = mat->n;
	int ji;
	for(int i = 0; i < n; i++){
      y = mat->valores[i];
	  if (i - mat->ancho < 0) { ji = 0; }
	  else { ji = i - mat->ancho; }				//Adaptacion a banda
      for (int j = ji; j < i ; j++){
          y = y - mat->getL(i, j)* mat->solution[j];
      }
      mat->solution[i] = y/mat->getL(i, i);
    }
}

void backwardSubstitutionBanda(Matriz* mat){
	double y;
	int je;
	for (int i = mat->n - 1; i >= 0; i--){
		y = mat->solution[i];
		if (i + mat->ancho + 1 > mat->n) { je = mat->n; }
		else { je = i + 1 + mat->ancho; }		//Adaptacion a banda
		for (int j = i + 1; j < je; j++){
			y = y - mat->get(i, j)* mat->solution[j];
		}
		mat->solution[i] = y / mat->get(i, i);
	}
	mat->solved = true;
}

int ultimaEsperanza(Matriz* mat, sanguijuela* sanguiList, int nsangui, double heightp, double widthp){
	int result = -1;
	int min = 10000000;
	for(int i = 0; i < nsangui; i++){
		//reconstruyo la matriz y resuelvo
		regenerarSinSangui(mat, i);
		mostrarSolucion(mat);
		int medio = mat->valores[(mat->n/2)];
		if( medio < min){
			min 	= medio;
			result  = i;
		}
	}
	regenerarSinSangui(mat, result);
	eliminacionGauseana(mat);
	backwardSubstitution(mat);
	return result;
}