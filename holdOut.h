/*
data tes diambil dari n data terakhir dari masing-masing class
misalkan percentageDataTes nya 20
data supervised 150 data

data tes = 20*150/100 = 30 data
data training = 150-30 = 120 data

data tes 30 data itu terdiri dari :
- 10 data terakhir class 0
- 10 data terakhir class 1
- 10 data terakhir class 2

ketentuan :
1) data yang ada di file dataSupervised.csv, harus disusun dan diurutkan berdasarkan class labelnya terlebih dahulu!
*/
#pragma once
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>

#include "csv.h"
#include "kNN.h"

using namespace std;

class holdOut
{
public:
	void init(string fDataSupervised, string fClassLabel);
	bool generateDataTraining(int percentageDataTes);

	void run(int K, int dataKe, bool showMsg);

private:
	csv CSV;
	kNN knn;

	vector<string> label;
	vector<vector<double>> dataSupervised;
	vector<vector<double>> dataTraining;
	vector<vector<double>> dataTes;
	vector<vector<string>> hasilEksperimen;

	int jlhLabel;
	bool nextProcess;
	string fNameDataTraining = "dataTraining_HoldOut.csv";
	string fNameDataTes = "dataTes_HoldOut.csv";
	string fNameHasilUji = "hasilUji_holdOut";

	template<typename T>
	vector<vector<T>> initialize2DVector(vector<vector<T>> vec, int nRow, int nCol) {
		for (int i = 0; i < nRow; i++) {
			vector<T> row;
			for (int j = 0; j < nCol; j++) {
				row.push_back(0);
			}
			vec.push_back(row);
		}

		return vec;
	}
	int runKnn(int K, int dataKe, bool showMsg);
	template<class vectorIn>
	vector<string> tostringVec(vectorIn vec)
	{
		vector<string> output;
		for (auto& col : vec) {
			output.push_back(to_string(col));
		}
		return output;
	}
	void tunggu(bool clearScreen) {
		cout << "Tekan tombol \"Enter\" untuk melanjutkan";
		cin.ignore();
		if (clearScreen)system("cls");
		else cout << "\n\n==============================================\n";
	}
};

void holdOut::init(string fDataSupervised, string fClassLabel) {
	dataSupervised.clear();
	label.clear();
	dataSupervised = CSV.read2DDouble(fDataSupervised, false);
	label = CSV.read1DString(fClassLabel, false);

	jlhLabel = label.size();
	nextProcess = true;
}

void holdOut::run(int K, int dataKe, bool showMsg) {	//dataKe<0 = random, 0 = all data, 1 dst = data idx 0 dst
	int jlh = dataTes.size();
	int jlhUjiData;

	hasilEksperimen.clear();
	hasilEksperimen.push_back({ "Eksperimen ke 1" });

	knn.setLabel(label);
	
	vector<int> hasilOutput;

	if (dataKe > jlh) {
		cout << "ERROR!!!\nnilai yang anda masukkan melebihi jumlah data tes!!!\n\n";
		return;
	}

	if (dataKe < 0) {
		//random
		srand(time(NULL));
		int random = rand() % jlh;
		cout << random << endl;
		hasilOutput.push_back(runKnn(K, random, showMsg));
		jlhUjiData = 1;
		hasilEksperimen.push_back({ "Data uji ke-", to_string(random) });
	}
	else if (dataKe == 0) {
		//tes semua data
		for (int i = 1; i <= jlh; i++) {
			hasilOutput.push_back(runKnn(K, i, showMsg));
			if(showMsg)tunggu(false);
		}
		jlhUjiData = jlh;
		hasilEksperimen.push_back({ "Data uji ke-", "semua" });
	}
	else
	{
		hasilOutput.push_back(runKnn(K, dataKe, showMsg));
		jlhUjiData = 1;
		hasilEksperimen.push_back({ "Data uji ke-", to_string(dataKe) });
	}

	cout << "\n\nhasil pengujian sesuai urutan data tes (0=NOT OK , 1=OK) :\n";
	int x = 0;
	for (auto& col : hasilOutput) {
		cout << col << ", ";
		if (col == 0)x++;
	}

	double rasioError = ((double)x / jlhUjiData);
	cout << "\n\nmissclassified = " << x << "\njumlah data test : " << jlhUjiData << endl;
	cout << "rasio error : " << rasioError << endl << endl;


	tunggu(false);	//true>>clearscreen, false>>tampilan sebelumnya tetap utuh

	//log hasil output
	hasilEksperimen.push_back({ "Hasil uji :" });
	hasilEksperimen.push_back(tostringVec(hasilOutput));
	hasilEksperimen.push_back({ "Rasio error eksperimen" , to_string(rasioError) });
	hasilEksperimen.push_back({});

	//simpan log uji
	CSV.Write2CSVfrom2DVector(fNameHasilUji + "_" + to_string(K) + "-NN.csv", hasilEksperimen);
	cout << "lihat log hasil uji di : " << fNameHasilUji + "_" + to_string(K) + "-NN.csv" << endl << endl;
	hasilEksperimen.clear();
}

int holdOut::runKnn(int K, int dataKe, bool showMsg) {
	vector<double> dataTesting;

	for (int i = 0; i < (dataTes[dataKe - 1].size() - 1); i++) {
		dataTesting.push_back(dataTes[dataKe - 1][i]);
	}
	//for (auto& dt : dataTesting)cout << dt << ", ";
	//cout << endl;
	
	int classSeharusnya = dataTes[dataKe - 1][(dataTes[dataKe - 1].size()) - 1];
	int output = knn.run(dataTraining, dataTesting, K, showMsg);

	//bandingkan hasil output klasifikasi dengan class seharusnya
	if (output == classSeharusnya)return 1;
	else return 0;
}

bool holdOut::generateDataTraining(int percentageDataTes) {
	vector<vector<int>> jlhDataperClass;
	int i, j;

	dataTraining.clear();
	dataTes.clear();

	if (nextProcess) {
		if (percentageDataTes >= 50) {
			cout << "ERROR!!!\nPersentase data tes harus lebih rendah dari data training (<50%)!!!" << endl;
			nextProcess = false;
			return false;
		}
		else {
			cout << "Mengelompokkan data training dan data testing";

			//data tes akan diambil sebanyak n % bagian akhir dari data supervised, dari masing2 class label
			//sisanya akan dimasukkan menjadi data training
			
			////cek masing-masing class punya berapa data
			int x = dataSupervised[0].size() - 1;
			cout << ".";

			//init jlhDataperClass
			for (i = 0; i < label.size(); i++) {
				vector<int> lbl;
				lbl.push_back(i);
				lbl.push_back(0);
				jlhDataperClass.push_back(lbl);
			}
			cout << ".";

			//hitung jumlah data untuk masing-masing class label
			for (i = 0; i < dataSupervised.size(); i++) {
				for (j = 0; j < jlhDataperClass.size(); j++) {
					if (dataSupervised[i][x] == jlhDataperClass[j][0])jlhDataperClass[j][1]++;
				}
			}
			cout << ".";

			//mulai split dataSupervised, sesuai persentase ke data training dan data tes
			//misal, persentase datates 20% dari 50 data supervised per class
			//20*50/100 = 10 dataTes, sisanya 50-10=40 data training
			i = jlhDataperClass.size();
			vector<vector<int>> nSplitData = holdOut::initialize2DVector(nSplitData,i,2);	//index 0-> dataTraining, index 1-> dataTes
			for (i = 0; i < nSplitData.size(); i++) {
				nSplitData[i][1] = ((percentageDataTes * jlhDataperClass[i][1]) / 100);
				nSplitData[i][0] = jlhDataperClass[i][1] - nSplitData[i][1];
			}
			//for (auto& row : nSplitData) {
			//	cout << "data tes : " << row[0] << ", data training : " << row[1] << endl;
			//}
			cout << ".";

			int y=0, z=0;
			bool flagSplit = true;
			x = nSplitData[y][z];
			for (i = 0; i < dataSupervised.size(); i++) {
				if (i < x) {//masukkan data training untuk class
					if (flagSplit) dataTraining.push_back(dataSupervised[i]);
					else dataTes.push_back(dataSupervised[i]);
				}
				else {
					if (z==0)z++;
					else {
						y++;
						z = 0;
					}

					x += nSplitData[y][z];

					if (flagSplit)flagSplit = false;
					else flagSplit = true;

					if (flagSplit) dataTraining.push_back(dataSupervised[i]);
					else dataTes.push_back(dataSupervised[i]);
				}
			}
			cout << ".";

			CSV.Write2CSVfrom2DVector(fNameDataTraining, dataTraining);
			CSV.Write2CSVfrom2DVector(fNameDataTes, dataTes);

			cout << "\n\n--SELESAI--\n\n" << endl;
			cout << "Silahkan cek file anda berikut ini : \n1) " << fNameDataTraining <<
					"\n2) " << fNameDataTes << endl << endl;
		}
	}
	return true;
}