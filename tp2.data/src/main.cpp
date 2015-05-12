#include "display.h"
#include "Matrix.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <bitset>
#include <stdint.h>
#include "algoritmos.h"
#include <limits>

using namespace std;

const int KNN = 0;
const int PCAKNN = 1;

int main(int argc, char *argv[]) {
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

	vector< bitset<42000> > crossval(Kfoldings);
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
	string trdata = databasedir + "train.csv";
	input.open(trdata.c_str(), ifstream::in);
	input.ignore(numeric_limits<streamsize>::max(), '\n');     //Ignoro primera linea

	Matrix trainset(42000, 784);
	vector<uint8_t> trainsetlabels(42000);

	string pixel;
	for (int digitN = 0; digitN < 42000; digitN++) {
		getline(input, pixel, ',');
		trainsetlabels[digitN] = stoi(pixel);
		for (int pixelN = 0; pixelN < 783; pixelN++) {
			getline(input, pixel, ',');
			trainset(digitN, pixelN) = stod(pixel);
		}
		getline(input, pixel, '\n');								//Cargo ultimo pixel, de esta forma porque no tiene "," despues
		trainset(digitN, 783) = stod(pixel);
	}
	input.close();

	//Cargo datos testset
	string tstdata = databasedir + "test.csv";
	input.open(tstdata.c_str(), ifstream::in);
	input.ignore(numeric_limits<streamsize>::max(), '\n');     //Ignoro primera linea

	Matrix testset(28000, 784);

	for (int digitN = 0; digitN < 28000; digitN++) {
		for (int pixelN = 0; pixelN < 783; pixelN++) {
			getline(input, pixel, ',');
			testset(digitN, pixelN) = stod(pixel);
		}
		getline(input, pixel, '\n');								//Cargo ultimo pixel, de esta forma porque no tiene "," despues
		testset(digitN, 783) = stod(pixel);
	}
	input.close();

	switch (atoi(argv[3])) {
		case KNN:
			for (int sample = 0; sample < testset.getn(); sample++) {
				vector<double> sampleN(testset.getm());
				for (int y = 0; y < sampleN.size(); y++) {
					sampleN[y] = testset(sample, y);
				}
				printf("Sample %i= %i \n", sample, kNN(trainset,trainsetlabels, sampleN, k));
			}
			break;
		case PCAKNN:
			break;
		default:
			break;
	}

	ofstream output;
	output.open(argv[2], ofstream::out);
	output << scientific;
	//Creo y ejecuto cada particion
	for (int foldingN = 0; foldingN < Kfoldings; foldingN++) {
		if (foldingN == 1) break;		//PARA TEST, BORRAR

		cout << "Creando particion:" << foldingN << "..." << endl;

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

		cout << "Particion " << foldingN << " creada" << endl;
		//A esta altura ya tengo la matriz train con las imagenes de su particion y la matriz de test con el resto, ambas con sus labels
		cout << "Crando matriz de covarianza..." << endl;

		//SI QUIERO CALCULARLA
		//Matrix covm(covarianceMatrix(train));
		Matrix covm(loadMatFile("covm"));
		//SI QUIERO CARGARLA
		//SI QUIERO GUARDARLA
		//saveMatFile(covm, "covm");
		//return 0;

		int m = covm.getm();
		Matrix autovects(m, alpha);
		vector<double> autovals(alpha);

		//Calculo alpha autovalores y autovectores
		cout << "Calculando autovalores y autovectores..." << endl;
		for (int i = 0; i < alpha; i++) {
			//Creo vector inicial
			vector<double> v0(m);
			for (int x = 0; x < m; x++) v0[x] = rand() % 10 + 1;
			//Power Iteration
			autovals[i] = PowerIteration(v0, covm, 1000);
			//Guardo el vector en la matriz de autovectores
			for (int x = 0; x < m; x++) {
				autovects(x, i) = v0[x];
			}
			Deflation(v0, covm, autovals[i]);
			cout << i << ": " << autovals[i] << endl;
			output << autovals[i] << endl;
		}
		//TODO general: expresar todos los vectores como matrices
		//TODO: crear nueva matriz trainsize x alpha, y en cada fila poner cada muestra tranformada al cambio de espacio
		//		luego para cada muestra de test, transformarla y buscar que digito es con knn
	}
	output.close();

	return 0;
}

