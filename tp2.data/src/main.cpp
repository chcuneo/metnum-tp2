#include "display.h"
#include "Matrix.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>
#include <cstdint>

using namespace std;

int main(int argc, char *argv[]){
	//Validar argumentos
	if (argc != 4) {
		cout << "usage: ./tp <input file> <output file> <method>" << endl;
		return 0;
	}
	
	//Cargar datos de input file
	ifstream input;
	input.open(argv[1], ifstream::in);

	string databasedir;
	int k;
	int alpha;
	int Kfoldings;

	input >> databasedir;
	input >> k;
	input >> alpha;
	input >> Kfoldings;
	
	vector<bitset<42000>> crossval(Kfoldings);
	vector < pair<int, int> > crossvaldim(Kfoldings);			//First: dimension de train; Second: dimension de test

	int aux;
	for (int line = 0; line < Kfoldings; line++) {
		crossvaldim[line].first = 0;
		crossvaldim[line].second = 0;
		for (int i = 0; i < 42000; i++) {
			input >> aux;
			crossval[line][i] = (int)aux;
			if (aux == 1) crossvaldim[line].first++;
			if (aux == 0) crossvaldim[line].second++;
		}
	}
	input.close();
	//Cargar datos trainset
	input.open(databasedir + "train.csv", ifstream::in);
	input.ignore(numeric_limits<streamsize>::max(), '\n');     //Ignoro primera linea

	Matrix trainset(42000, 784);
	vector<uint8_t> trainsetlabels(42000);

	char pixel[4];
	for (int digitN = 0; digitN < 42000; digitN++) {
		input.getline(pixel, 3, ',');
		trainsetlabels[digitN] = atoi(pixel);
		for (int pixelN = 0; pixelN < 783; pixelN++) {
			input.getline(pixel, 3, ',');
			trainset(digitN, pixelN) = (double)atoi(pixel);
		}
		input.getline(pixel, 3);								//Cargo ultimo pixel, de esta forma porque no tiene "," despues
		trainset(digitN, 783) = (double)atoi(pixel);
	}

	//Creo y ejecuto cada particion
	for (int foldingN = 0; foldingN < Kfoldings; foldingN++) {
		int trainsize = crossvaldim[foldingN].first;
		int testsize = crossvaldim[foldingN].second;
		Matrix train(trainsize, 784);
		vector<uint8_t> trainlabels(trainsize);
		Matrix test(testsize, 784);
		vector<uint8_t> testchecklabels(testsize);
		int train0 = 0;
		int test0 = 0;
		for (int sampleN = 0; sampleN < 42000; sampleN++) {
			if (crossval[foldingN][sampleN] == 1) {
				for (int y = 0; y < 784; y++) train(train0, y) = trainset(sampleN, y);
				trainlabels[train0] = trainsetlabels[sampleN];
				train0++;
			} else {
				for (int y = 0; y < 784; y++) test(test0, y) = trainset(sampleN, y);
				testchecklabels[test0] = trainsetlabels[sampleN];
				test0++;
			}
		}
		//A esta altura ya tengo la matriz train con las imagenes de su particion y la matriz de test con el resto, ambas con sus labels
		//TODO: Que se hace despues aca?
	}

	return 0;
}

