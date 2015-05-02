#include "display.h"
#include "Matrix.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char *argv[]){
	//Validar argumentos
	if (argc != 3) {
		cout << "usage: ./tp <input file> <output file>" << endl;
		return 0;
	}
	
	//Cargar datos input
	std::ifstream input;
	input.open(argv[1]);

	string databasename;
	int k;
	int alpha;
	int Kfoldings;
	vector<vector<bool>> crossval;

	input >> databasename;
	input >> k;
	input >> alpha;
	input >> Kfoldings;
	
	crossval.resize(Kfoldings);
	int aux;
	for (int line = 0; line < Kfoldings; line++) {
		crossval[line].resize(42000);
		for (int i = 0; i < 42000; i++) {
			input >> aux;
			crossval[line][i] = (bool)aux;
		}
	}


	return 0;
}

