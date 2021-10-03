#include <iostream>
#include <fstream>
#include <stdlib.h>  
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <iomanip>
#include <time.h> 
//#include <locale.h>
#pragma warning(disable:4996)

#include <complex.h>
#include <fftw3.h>
#include "stdafx.h"

//definições para FFTW3
#define REAL 0
#define IMAG 1

#define DEBUG 1


bool is_number(const string& s)
{
	string::const_iterator it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

bool is_alpha(const string& s)
{
	string::const_iterator it = s.begin();
	while (it != s.end() && std::isalpha(*it)) ++it;
	return !s.empty() && it == s.end();
}



void fft_real(double* in, fftw_complex* out, int Quantidade)
{
	fftw_plan plan = fftw_plan_dft_r2c_1d(Quantidade, in, out, FFTW_ESTIMATE /*| FFTW_MEASURE*/);
	fftw_execute(plan);
	fftw_destroy_plan(plan);
	fftw_cleanup();
}

using namespace std;
#define TAM_INPUT 100
int main(int argc, char** argv) {
	//setlocale(LC_ALL, "");
	
	cout << "Bem vindo ao conversor \n ";
	cout << "Esse é o padrão: EPOCH-TEMPO-ID_SENSOR \n ";
	cout << "Digite o nome do arquivo a converter: ";

	char nome[TAM_INPUT];
	cin >> nome;
	
	long long Epoch;
	int qtdLinhasTxt = 0;	
	double TempoAquisicao = 0;

	double* VetorX;
	double* VetorY;
	double* VetorZ;

	fftw_complex* VetorXFFT;
	fftw_complex* VetorYFFT;
	fftw_complex* VetorZFFT;

	//Epoch
	//Duração da coleta: xxxx em ms
	//ID Sensor	
	//Exemplo de nome "1602245833-2715-NAO7856"
	string s;

	for (int i = 0; i < TAM_INPUT; i++) {
		if ((nome[i] > 0x20) && (nome[i] < 0x7E)) { //Verifica se o arquivo está no range de caracteres "digitáveis"
			s = s + nome[i];
		}
	}

	s = s + "-";
	string delimiter = "-";

	size_t pos = 0;
	string temp;
	cout << "**********************************************\n";
	while ((pos = s.find(delimiter)) != string::npos) {
		temp = s.substr(0, pos);
		
		if(is_number(temp)){ //Verifica se é um número
			
			if (temp.length() > 8) {
				Epoch = stoi(temp);
				 time_t result = Epoch;
				 cout <<"Data de coleta dos dados: " << asctime(localtime(&result));
			}
			if ((temp.length() > 2) && (temp.length() < 6)) {
				TempoAquisicao = stoi(temp);
				cout << "Tempo de Aquisicao: " << TempoAquisicao << "ms\n";
			}
						
		}
		else{
		cout << "Dados do Sensor: " << temp << "\n";
		}
		
		s.erase(0, pos + delimiter.length());
	}
	
	fstream newfile;
	cout << "**********************************************\n";

	newfile.open(nome, ios::in);

	string tp;
	string delimitador = ",";

	if (newfile.is_open())//Verifica se o arquivo foi aberto
	{
		

		while (getline(newfile, tp)) {
			qtdLinhasTxt++;			
		}
		qtdLinhasTxt++;
		newfile.close();
	}
	else
	{
		cout << "[pt-br] O arquivo nao existe \n";
		cout << "[en] The file not exist \n";
		system("pause");
		return 0;
	}

	
	
	cout << "\n Linha encontradas: " << qtdLinhasTxt << "\n";
	newfile.open(nome, ios::in);
	

	
	VetorX = (double*)fftw_malloc(sizeof(double) * (uint32_t)(qtdLinhasTxt));
	VetorY = (double*)fftw_malloc(sizeof(double) * (uint32_t)(qtdLinhasTxt));
	VetorZ = (double*)fftw_malloc(sizeof(double) * (uint32_t)(qtdLinhasTxt));
	
	VetorXFFT = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (qtdLinhasTxt /* / (uint32_t)2 + (uint32_t)1*/));
	VetorYFFT = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (qtdLinhasTxt /* / (uint32_t)2 + (uint32_t)1*/));
	VetorZFFT = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (qtdLinhasTxt /* /(uint32_t)2 + (uint32_t)1*/));
	

	if (newfile.is_open()) {

		uint32_t linha = 0;
		unsigned int xyz = 0;
		char temporario[] = "";

		while (getline(newfile, tp))
		{ //read data from file object and put it into string.
			tp = tp + ",";
			
			xyz = 0;
			while ((pos = tp.find(delimitador)) != string::npos)
			{				
				temp = tp.substr(0, pos);
				
				double dummy = atof(temp.c_str());
				//cout << dummy << "\n";
				switch (xyz)
				{
					default:
					
						break;
					case 0:
						VetorX[linha] = dummy;
						break;

					case 1:
						VetorY[linha] = dummy;
						break;

					case 2:
						VetorZ[linha] = dummy;
						break;
				}
				
				

				xyz++;

				tp.erase(0, pos + delimiter.length());
			}
			//  cout << "\n";
			linha++;
		}
		newfile.close(); //close the file object.
	}
	else
	{
		cout << "[pt-br] O arquivo nao existe \n";
		cout << "[en] The file not exist \n";
		system("pause");
		return 0;
	}

	if (TempoAquisicao == 0) {
		TempoAquisicao = (double)2.715;
		cout << "Tempo não lido, adotado padrão:" << TempoAquisicao << "s\n";
	}
	else {
		TempoAquisicao = TempoAquisicao / 1000;
		cout << "Tempo utilizado:" << TempoAquisicao << "s\n";
	}



	#ifdef DEBUG
	//Para debbug, gera um arquivo com os dados lidos para verficar se houve algum erro na importação
	ofstream outPre("preProcesso.txt");
	for (int i = 1; i < qtdLinhasTxt; i++) {

		for (int xyz = 0; xyz <= 2; xyz++)
		{
			switch (xyz)
			{
			default:
				break;
			case 0:
				outPre << setprecision(15) << (VetorX[i]/*[REAL]*/);
				break;
			case 1:
				outPre << setprecision(15) << (VetorY[i]/*[REAL]*/);
				break;
			case 2:
				outPre << setprecision(15) << (VetorZ[i]/*[REAL]*/);
				break;
			}

			if (xyz != 2)
			{
				outPre << ",";
			}
		}

		outPre << ",";
		outPre << setprecision(20) << (double)((double)1 / (double)(TempoAquisicao) / (double)((double)((double)qtdLinhasTxt-i) + 1));
		outPre << "\n";

	}
	cout << "Salvando Arquivo...\n";
	outPre.close();
#endif
	
	
	cout << "\nExecutando FFT...\n";
	cout << "Processando X...\n";
	fft_real(VetorX, VetorXFFT, qtdLinhasTxt);	
	
	cout << "Processando Y...\n";
	fft_real(VetorY, VetorYFFT, qtdLinhasTxt);
	
	cout << "Processando Z...\n";
	fft_real(VetorZ, VetorZFFT, qtdLinhasTxt);
	
	cout << "Gerando Arquivo...\n";
	ofstream out("output.txt");
	for (int i = 1; i < (qtdLinhasTxt/2); i++) {

		for (int xyz = 0; xyz <= 2; xyz++)
		{
			switch (xyz)
			{
			default:
				break;
			case 0:
				out << setprecision(20) << (VetorXFFT[i][REAL]);
				break;
			case 1:
				out << setprecision(20) << (VetorYFFT[i][REAL]);
				break;
			case 2:
				out << setprecision(20) << (VetorZFFT[i][REAL]);
				break;
			}
			
			if (xyz != 2)
			{//Adiciona uma vírgula entre os dados dos vetores.
				out << ",";
			}
		}
		//Após ler os vetores X, Y e Z adiciona uma vírgula para a frequência
		out << ",";
		
		out << setprecision(20) << (double)((double)1 / ((double)(TempoAquisicao) / (double)((double)(/*qtdLinhasTxt -*/ i) + 1)));
		out << "\n";

	}
	cout << "Salvando Arquivo...\n";
	out.close();

	//double seconds = difftime(timer, mktime(&y2k));
	//cout << "Executado em " << setprecision(5) <<(tempo_decorrido_final - tempo_decorrido) << " segundos \n";
	system("pause");

	//Liberando os espaços de memória, previamente alocados
	fftw_free(VetorX);
	fftw_free(VetorY);
	fftw_free(VetorZ);
	fftw_free(VetorXFFT);
	fftw_free(VetorYFFT);
	fftw_free(VetorZFFT);
	return 0;
}