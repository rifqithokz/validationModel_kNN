#pragma once
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <math.h>
#include <stdlib.h>

#include "csv.h"
#include "kNN.h"

using namespace std;

class LOO
{
public:
	bool init(string fDataSupervised, string fClassLabel);
	void run(int K, bool showMsg);

private:
	csv CSV;
	kNN knn;

	vector<string> label;
	vector<vector<double>> dataSupervised;
	vector<vector<double>> dataTraining;
	vector<vector<double>> dataTes;
	vector<vector<string>> logDataTraining;
	vector<vector<string>> logDataTes;
	vector<vector<string>> hasilEksperimen;
	vector<double> totalError;	//penampung data error

	int flagIdx;
	int jlhLabel;
	int jlhExperiment;	//jumlah eksperimen yang dilakukan
	int jlhDataSekaliTes;
	bool nextProcess;
	string fNameDataTraining = "dataTraining_LOO_";
	string fNameDataTes = "dataTes_LOO_";
	string ext = ".csv";
	string fNameHasilUji = "hasilUji_LOO";

	bool generateDataTraining(int experimentKe);
	int runKnn(int K, int dataKe, bool showMsg);
	void clearVector();
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
	void logData(int act, int K, int eksperimenKe);
};

bool LOO::init(string fDataSupervised, string fClassLabel) {
	dataSupervised.clear();
	label.clear();
	dataSupervised = CSV.read2DDouble(fDataSupervised, false);
	label = CSV.read1DString(fClassLabel, false);

	jlhLabel = label.size();
	nextProcess = true;

	jlhExperiment = dataSupervised.size();
	jlhDataSekaliTes = 1;

	flagIdx = 0;

	return true;
}

void LOO::run(int K, bool showMsg) {
	//K -> konstanta Nearest neighbours

	logData(0, K, 0);

	for (int i = 0; i < jlhExperiment; i++) {
		//generate dulu model data tes dan data trainingnya
		generateDataTraining(i + 1);

		//set label untuk kNN nya
		knn.setLabel(label);

		vector<int> hasilOutput;
		//tes semua data yang ada di data tes
		hasilOutput.push_back(runKnn(K, 1, showMsg));
		if (showMsg)tunggu(false);

		cout << "\n\nhasil pengujian sesuai urutan data tes (0=NOT OK , 1=OK) :\n";
		int x = 0;
		for (auto& col : hasilOutput) {
			cout << col << ", ";
			if (col == 0)x++;
		}

		double rasioError = (double)x;
		cout << "\n\nmissclassified = " << x << "\njumlah data test : 1" << endl;
		cout << "rasio error : " << rasioError << endl << endl;


		if(showMsg)tunggu(false);	//true>>clearscreen, false>>tampilan sebelumnya tetap utuh

		//log hasil output
		hasilEksperimen.push_back({ "Eksperimen ke " , to_string(i + 1) });
		hasilEksperimen.push_back({ "index data tes di data supervised" });
		hasilEksperimen.push_back({ to_string(flagIdx - 1) });
		hasilEksperimen.push_back({ "Hasil uji :" });
		hasilEksperimen.push_back(tostringVec(hasilOutput));
		hasilEksperimen.push_back({ "Rasio error eksperimen ke-" + to_string(i + 1) , to_string(rasioError) });
		hasilEksperimen.push_back({});

		totalError.push_back(rasioError);
		logData(1, K, i+1);
		clearVector();
	}

	//hitung total error
	double errorRataRata = 0;
	for (auto& col : totalError) {
		errorRataRata += col;
	}

	//bagi total error dengan jumlah pengujian
	errorRataRata = errorRataRata / jlhExperiment;

	//update log terakhir
	hasilEksperimen.push_back({});
	hasilEksperimen.push_back({});
	hasilEksperimen.push_back({ "Error rata-rata : " , to_string(errorRataRata) });

	//simpan log uji
	CSV.Write2CSVfrom2DVector(fNameHasilUji + "_" + to_string(K) + "-NN" + ext, hasilEksperimen);
	cout << "lihat log hasil uji di : " << fNameHasilUji + "_" + to_string(K) + "-NN" + ext << endl << endl;
	hasilEksperimen.clear();
	totalError.clear();
	logData(2, K, 0);
}

int LOO::runKnn(int K, int dataKe, bool showMsg) {
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

bool LOO::generateDataTraining(int experimentKe) {
	int i, j;

	if (nextProcess) {
		cout << "Mengelompokkan data training dan data testing - " << experimentKe << endl;

		//data tes akan diambil sebanyak jlhDataSekaliTes secara random dari data supervised,
		//sisanya akan dimasukkan menjadi data training

		//mulai split dataSupervised, sesuai jlhDataSekaliTes ke data training dan data tes
		//sesuai index yang didapat dari random
		j = 0;
		for (i = 0; i < dataSupervised.size(); i++) {
			if (i == flagIdx) {
				//sesuai dengan index random, masukkan data ke data tes
				dataTes.push_back(dataSupervised[i]);
				j++;
			}
			else {
				//data yang tidak terpilih, masukkan ke data training
				dataTraining.push_back(dataSupervised[i]);
			}
		}
		cout << "." << endl;

		////simpan data tes dan data training ke csv
		//CSV.Write2CSVfrom2DVector(fNameDataTraining + to_string(experimentKe) + ext, dataTraining);
		//CSV.Write2CSVfrom2DVector(fNameDataTes + to_string(experimentKe) + ext, dataTes);

		//cout << "\n\n--SELESAI--\n\n" << endl;
		//cout << "Silahkan cek file anda berikut ini : \n- " << fNameDataTraining + to_string(experimentKe) + ext <<
		//	"\n- " << fNameDataTes + to_string(experimentKe) + ext << endl << endl;

		flagIdx++;
	}
	return true;
}

void LOO::clearVector() {
	dataTes.clear();
	dataTraining.clear();
}

void LOO::logData(int act, int K, int eksperimenKe) {
	if (act == 0) {
		logDataTes.clear();
		logDataTraining.clear();
	}
	else if (act == 1) {
		//simpan ke vector log
		//log data
		logDataTraining.push_back({ "Data training eksperimen ke : ", to_string(eksperimenKe) });
		logDataTraining.push_back({});
		for (auto& row : dataTraining) {
			logDataTraining.push_back(tostringVec(row));
		}
		logDataTraining.push_back({});
		logDataTraining.push_back({});

		logDataTes.push_back({ "Data tes eksperimen ke : ", to_string(eksperimenKe) });
		logDataTes.push_back({});
		for (auto& row : dataTes) {
			logDataTes.push_back(tostringVec(row));
		}
		logDataTes.push_back({});
		logDataTes.push_back({});
	}
	else if (act == 2) {
		//simpan ke csv
		CSV.Write2CSVfrom2DVector(fNameDataTraining + to_string(K) + "-NN" + ext, logDataTraining);
		CSV.Write2CSVfrom2DVector(fNameDataTes + to_string(K) + "-NN" + ext, logDataTes);
		logDataTraining.clear();
		logDataTes.clear();
	}
}