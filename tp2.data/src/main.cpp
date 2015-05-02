#include "display.h"
#include "Matrix.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]){
	Matrix c(10, 5);
	c(1, 1) = 1;
	c(9, 1) = 2;
	c(5, 4) = 3;
	c(9, 4) = 4;
	showMatCons(c);
	c.transpuesta();
	showMatCons(c);
	system("pause");
	return 0;
}

