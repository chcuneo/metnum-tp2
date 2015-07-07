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
#include <chrono>
#include <tuple>


using namespace std;

const int KNN = 0;
const int PCAKNN = 1;
const int CVPARTITIONTEST = 2;
const int TESTS = 3;

//Argumentos:
//<input file> = test catedra
//<output file> = salida catedra
//<method> -> 0 = KNN; 1 = PCA+KNN; 2 = CrossValidation; 3 = Tests
int main(int argc, char *argv[]) {
	//Validar argumentos: ./tp <input file> <output file> <method> <Save|notLoad>
	if (argc < 4 || argc > 5) {
		cout << "usage: ./tp <input file> <output file> <method>" << endl;
		return 0;
	}

	bool savenotload;
	if (argc == 5) savenotload = atoi(argv[4]); else savenotload = true;
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

	Matrix testset(28000, 784);

	if (atoi(argv[3]) != CVPARTITIONTEST && atoi(argv[3]) != TESTS) {
		string tstdata = databasedir + "test.csv";
		input.open(tstdata.c_str(), ifstream::in);
		input.ignore(numeric_limits<streamsize>::max(), '\n');			//Ignoro primera linea


		for (int digitN = 0; digitN < 28000; digitN++) {
			for (int pixelN = 0; pixelN < 783; pixelN++) {
				getline(input, pixel, ',');
				testset(digitN, pixelN) = stod(pixel);
			}
			getline(input, pixel, '\n');								//Cargo ultimo pixel, de esta forma porque no tiene "," despues
			testset(digitN, 783) = stod(pixel);
		}
		input.close();
	}

	switch (atoi(argv[3])) {
		case KNN:
		{
			ofstream output;
			output.open("predictions-KNN", ofstream::out);		//Archivo donde guardo predicciones (va a kaggle)
			for (int sample = 0; sample < testset.getn(); sample++) output << kNN(trainset, trainsetlabels, testset(sample), k) << endl;
			output.close();
		}
			break;
		case PCAKNN:
		{
			Matrix covm(covarianceMatrix(trainset));		//Obtengo matriz de covarianzas
			Matrix autovects(alpha, covm.getm());			//V = aca van a estar los autovectores como filas, por cuestion de eficiencia
			vector<double> autovals(alpha);					//Sigma^2 = esta seria la diagonal de sigma^2, y almacena los autovalores de covm o los valores singulares de trainset al cuadrado

			getAlphaEigenvectorAndValues(covm, autovects, autovals, alpha); //Proceso y obtengo los alpha autovalores mas grandes y sus respectivos autovectores

			int trainsize = trainset.getn();				
			int testsize = testset.getn();

			Matrix tctrain(trainsize, alpha);				//Aca voy a guardar las muestras de train cambiadas de base (base de autovectores)
			for (int x = 0; x < trainsize; x++) for (int y = 0; y < alpha; y++)	tctrain(x, y) = vectorMul(autovects(y), trainset(x)); //Para cada muestra, la cambio de base y almaceno en la matriz tctrain

			ofstream output;
			output.open("predictions-PCAKNN", ofstream::out);		//Archivo donde guardo predicciones (va a kaggle)
			
			vector<double> testsample(alpha);				//Aca va la muestra del test a procesar, cambiada de base
			int guess;
			for (int x = 0; x < testsize; x++) {
				for (int y = 0; y < alpha; y++) testsample[y] = vectorMul(testset(x), autovects(y)); //Cambio de base la muestra
				guess = kNN(tctrain, trainsetlabels, testsample, k);	//Aplico knn para predecir que digito es
				output << guess << endl;								//Lo grabo al archivo
			}

			output.close();
		}
			break;
		case CVPARTITIONTEST:
		{
			ofstream output(argv[2], ofstream::out);						//Salida pedida por la catedra testX.out
			output << scientific;
			ofstream olog("/home/ccuneo/TmpMetNum/logcv", ofstream::out);	//Para guardar rendimiento de predicciones

			//Creo y ejecuto cada particion
			for (int foldingN = 0; foldingN < Kfoldings; foldingN++) {
				cout << "Creando particion:" << foldingN << "..." << endl;

				int trainsize = crossvaldim[foldingN].first; int testsize = crossvaldim[foldingN].second; //Tamaño de particiones

				Matrix train(trainsize, 784); vector<uint8_t> trainlabels(trainsize);	//Train de la particion y sus etiquetas
				Matrix test(testsize, 784);	vector<uint8_t> testchecklabels(testsize);	//Test de la particion y sus etiquetas

				//Genero las Particiones
				int train0 = 0;	int test0 = 0;
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

				Matrix covm(train.getm(), train.getm());

				string covmfilename = "/home/ccuneo/TmpMetNum/covm-CV" + to_string(foldingN);
				if (savenotload) {
					cout << "Calculando matriz de covarianza..." << endl;
					//SI QUIERO CALCULARLA Y GUARDARLA
					covm = covarianceMatrix(train);
					saveMatFile(covm, covmfilename.c_str());
				} else {
					//SI QUIERO CARGARLA
					cout << "Cargando matriz de covarianza..." << endl;
					covm = loadMatFile(covmfilename.c_str());
				}

				//Aca en covm ya tenemos la matriz de covarianzas de train

 				Matrix autovects(alpha, covm.getm());			//V = aca van a estar los autovectores como filas, por cuestion de eficiencia
				vector<double> autovals(alpha);					//Sigma^2 = esta seria la diagonal de sigma^2, y almacena los autovalores de covm o los valores singulares de train al cuadrado

				string autovecfilename = "/home/ccuneo/TmpMetNum/autovecs" + to_string(alpha) + "-CV" + to_string(foldingN);
				if (savenotload) {
					//SI QUIERO CALCULARLOS Y GUARDARLOS
					getAlphaEigenvectorAndValues(covm, autovects, autovals, alpha);		//Proceso y obtengo los alpha autovalores mas grandes y sus respectivos autovectores
					saveMatFile(autovects, autovecfilename.c_str());
				} else {
					//SI QUIERO CARGARLOS
					cout << "Cargando autovectores..." << endl;
					autovects = loadMatFile(autovecfilename.c_str());
				}

				//Aca en autovects ya tenemos los autovectores de la matriz de covarianzas
				//En autovals tenemos los autovalores si es que los calculamos o nada si cargamos de archivo

				for (int i = 0; i < autovals.size(); i++) output << sqrt(autovals[i]) << endl;	//Vuelco valores singulares a archivo (catedra)
				
				cout << "Calculo tc..." << endl;
				
				Matrix tctrain(trainsize, alpha);				//Aca voy a guardar las muestras de trainset cambiadas de base (base de autovectores)
				for (int x = 0; x < trainsize; x++) for (int y = 0; y < alpha; y++)	tctrain(x, y) = vectorMul(autovects(y), train(x)); //Para cada muestra, la cambio de base y almaceno en la matriz tctrain
				
				cout << "Listo tc" << endl;
				cout << "Proceso casos de test:" << endl;
				
				vector<double> testsample(alpha);				//Aca va la muestra del test a procesar, cambiada de base
				int guess;
				int correctguesses = 0;							//Cantidad de predicciones correctas
				for (int x = 0; x < testsize; x++) {
					for (int y = 0; y < alpha; y++) {
						testsample[y] = vectorMul(test(x), autovects(y));		//Hago cambio de base
					}
					guess = kNN(tctrain, trainlabels, testsample, k);			//Hago la prediccion con knn
					if ((int)testchecklabels[x] == guess) correctguesses++;		//Si predije lo que deberia, incremento
				}
				olog << "CV " << foldingN << ": Guessed " << correctguesses << " of " << testsize << endl;
			}
			output.flush();
			output.close();
			olog.flush();
			olog.close(); 
		}
			break;
		case TESTS:
		{
			cout << "Iniciando Tests:" << endl;
			ofstream log("/home/ccuneo/TmpMetNum/db.csv", ofstream::out);
			log << "CVpartition,k,alpha,hits,timekNN,timeBaseChange" << endl;

			for (int foldingN = 1; foldingN < Kfoldings; foldingN++) {
				int trainsize = crossvaldim[foldingN].first; int testsize = crossvaldim[foldingN].second; //Tamaño de particion

				Matrix train(trainsize, 784); vector<uint8_t> trainlabels(trainsize);	//Train de la particion y sus etiquetas
				Matrix test(testsize, 784);	vector<uint8_t> testchecklabels(testsize);	//Test de la particion y sus etiquetas

				cout << "Generando Particion: " << foldingN << endl;

				//Genero las Particiones
				int train0 = 0;	int test0 = 0;
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

				cout << "Particion creada" << endl;
				//A esta altura ya tengo la matriz train con las imagenes de su particion y la matriz de test con el resto, ambas con sus labels

				Matrix covm(train.getm(), train.getm());

				string covmfilename = "/home/ccuneo/TmpMetNum/covm-CV" + to_string(foldingN);

				printUpdateLine("Cargando matriz de covarianza...");
				covm = loadMatFile(covmfilename.c_str());
				printNewLine("Matriz cargada");
				//Aca en covm ya tenemos la matriz de covarianzas de train

				Matrix autovects(alpha, covm.getm());			//V = aca van a estar los autovectores como filas, por cuestion de eficiencia

				string autovecfilename = "/home/ccuneo/TmpMetNum/autovecs" + to_string(alpha) + "-CV" + to_string(foldingN);

				printUpdateLine("Cargando autovectores...");
				autovects = loadMatFile(autovecfilename.c_str());
				printNewLine("Autovectores cargados");

				//Aca en autovects ya tenemos los autovectores de la matriz de covarianzas
				/////////////////////////////////////////////////////////
				/////////////////////////////////////////////////////////
				///////// A PARTIR DE ACA EMPIEZAN LOS TESTS ////////////
				/////////////////////////////////////////////////////////
				/////////////////////////////////////////////////////////
				int alphaMAX = 50;
				int kMAX = 1000;

				for (int alpha = 1; alpha < alphaMAX; alpha++){
					string testdataname("Alpha=" + to_string(alpha) + " k=variable");
					printUpdateLine("Calculo tc(test): " + testdataname);

					Matrix tctrain(trainsize, alpha);				//Aca voy a guardar las muestras de trainset cambiadas de base (base de autovectores)

					chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();

					for (int x = 0; x < trainsize; x++)
						for (int y = 0; y < alpha; y++)
							tctrain(x, y) = vectorMul(autovects(y), train(x)); //Para cada muestra, la cambio de base y almaceno en la matriz tctrain

					chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
					auto timebase = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

					printUpdateLine("Listo tc(test):" + testdataname);
					cout << endl << "Proceso distintos k:" << endl;

					vector<double> testsample(alpha);				//Aca va la muestra del test a procesar, cambiada de base
					vector<int> guessforK(kMAX);
					long long timek;
					vector<int> correctguesses(kMAX, 0);							//Cantidad de predicciones correctas
					for (int x = 0; x < testsize; x++) {
						for (int y = 0; y < alpha; y++) {
							testsample[y] = vectorMul(test(x), autovects(y));		//Hago cambio de base
						}
						chrono::high_resolution_clock::time_point t3 = chrono::high_resolution_clock::now();
						ztokNN(tctrain, trainlabels, testsample, kMAX, guessforK);			//Hago la prediccion con knn
						chrono::high_resolution_clock::time_point t4 = chrono::high_resolution_clock::now();
						timek = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();

						//printUpdateLine("Test " + to_string(x));
						for (int i = 0; i < kMAX; i++) if ((int)testchecklabels[x] == guessforK[i]) correctguesses[i]++;		//Si predije lo que deberia, incremento
					}
					//log << "CVpartition, k, alpha, hits, timek, timebase" << endl;
					for (int i = 0; i < kMAX; i++) {
						log << foldingN << "," << i << "," << alpha << "," << correctguesses[i] << "," << timek << "," << timebase << endl;
					}
					log.flush();
				}
			}
			log.close();
		}
			break;
		default:
			break;
	}

	
	return 0;
}

