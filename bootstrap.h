/*
langkah di bootstrap :
1. tentukan jumlah data training yang akan digunakan. 
	misalkan 60% atau 80% dari data supervised. misal jumlah data supervised = 150 data
	data training = 80% x jumlah data supervised = 120 data
2. pilih secara random 120 data training dari data supervised yang kita punya
3. dari 120 data tersebut akan ada kemungkinan beberapa data yang sama
4. data-data yang ada pada data supervised, tetapi tidak terpilih masuk menjadi data training, dijadikan sebagai data tes
	contoh :
	ternyata dari 120 data training, ada 10 data yang terpilih 2 kali. ini sama artinya dengan data supervised yang
	terpilih menjadi data training sebenarnya hanya 110 data.
	sisanya ada 150-110=40 data >> 40 data ini dijadikan sebagai data tes.
	tetapi jika dari 120 data training ternyata tidak ada data yang terpilih lebih dari 1 kali, berarti
	data tes nya adalah 150-120=30 data

referensi : https://machinelearningmastery.com/a-gentle-introduction-to-the-bootstrap-method/
*/

#pragma once
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "csv.h"
#include "kNN.h"

using namespace std;

class bootstrap
{
public:
	bool init(string fDataSupervised, string fClassLabel, int percentageDataTraining, int jlhEksperimen);
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
	vector<int> idxDataTes;
	vector<int> idxDataTraining;

	int jlhLabel;
	int jlhExperiment;	//banyaknya eksperimen yang dilakukan
	int jlhDataSekaliTes;
	int jlhDataTraining;
	bool nextProcess;
	string fNameDataTraining = "dataTraining_bootstrap_";
	string fNameDataTes = "dataTes_bootstrap_";
	string ext = ".csv";
	string fNameHasilUji = "hasilUji_bootstrap";

	bool generateDataTraining(int experimentKe);
	void splitDataSupervised(int jlhRandom);
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
	void clearVector();
	void logData(int act, int K, int eksperimenKe);
};

bool bootstrap::init(string fDataSupervised, string fClassLabel, int percentageDataTraining, int jlhEksperimen) {
	dataSupervised.clear();
	label.clear();
	dataTraining.clear();
	dataTes.clear();
	hasilEksperimen.clear();
	totalError.clear();
	idxDataTes.clear();
	idxDataTraining.clear();

	dataSupervised = CSV.read2DDouble(fDataSupervised, false);
	label = CSV.read1DString(fClassLabel, false);

	jlhLabel = label.size();
	nextProcess = true;
	jlhExperiment = jlhEksperimen;

	jlhDataTraining = (percentageDataTraining * dataSupervised.size()) / 100;

	if ((percentageDataTraining<51)||(jlhEksperimen<1)) {
		cout << "ERROR!!!\nKesalahan pada nilai masukan persentase data training yang digunakan," <<
				"\natau pada nilai masukan jumlah eksperimen yang akan dilakukan.\n\n";
		nextProcess = false;
		return false;
	}

	return true;
}

void bootstrap::run(int K, bool showMsg) {
	//K -> konstanta Nearest neighbours

	logData(0, K, 0);

	totalError.clear();
	hasilEksperimen.clear();

	for (int i = 0; i < jlhExperiment; i++) {
		//generate dulu model data tes dan data trainingnya
		generateDataTraining(i + 1);

		int jlh = dataTes.size();

		//set label untuk kNN nya
		knn.setLabel(label);

		vector<int> hasilOutput;
		//tes semua data yang ada di data tes
		for (int i = 1; i <= jlh; i++) {
			hasilOutput.push_back(runKnn(K, i, showMsg));
			if (showMsg)tunggu(false);
		}

		cout << "\n\ndata tes index ke : \n";
		for (auto& col : idxDataTes) {
			cout << col << ", ";
		}
		cout << "\n\nhasil pengujian sesuai urutan data tes (0=NOT OK , 1=OK) :\n";
		int x = 0;
		for (auto& col : hasilOutput) {
			cout << col << ", ";
			if (col == 0)x++;
		}

		double rasioError = ((double)x / jlh);
		cout << "\n\nmissclassified = " << x << "\njumlah data test : " << jlh << endl;
		cout << "rasio error : " << rasioError << endl << endl;


		if(showMsg)tunggu(false);	//true>>clearscreen, false>>tampilan sebelumnya tetap utuh

		//log hasil output
		hasilEksperimen.push_back({ "Eksperimen ke " , to_string(i + 1) });
		hasilEksperimen.push_back({ "index data tes di data supervised" });
		hasilEksperimen.push_back(tostringVec(idxDataTes));
		hasilEksperimen.push_back({ "Hasil uji :" });
		hasilEksperimen.push_back(tostringVec(hasilOutput));
		hasilEksperimen.push_back({ "Rasio error eksperimen ke-" + to_string(i + 1) , to_string(rasioError) });
		hasilEksperimen.push_back({});

		totalError.push_back(rasioError);
		logData(1, K, i + 1);
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

int bootstrap::runKnn(int K, int dataKe, bool showMsg) {
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

bool bootstrap::generateDataTraining(int experimentKe) {
	int i, j;

	if (nextProcess) {
		cout << "Mengelompokkan data training dan data testing - " << experimentKe << "\n\n";

		//data training akan diambil sebanyak jlhDataTraining secara random dari data supervised,
		//sisanya yang tidak terpilih akan dimasukkan menjadi data tes

		//generate random index untuk ambil data training dari data supervised sebanyak jlhDataTrain
		//kemudian split menjadi data training dan data tes
		splitDataSupervised(jlhDataTraining);
		cout << "." << endl;

		//simpan data tes dan data training ke csv
		//CSV.Write2CSVfrom2DVector(fNameDataTraining + to_string(experimentKe) + ext, dataTraining);
		//CSV.Write2CSVfrom2DVector(fNameDataTes + to_string(experimentKe) + ext, dataTes);

		//cout << "\n\n--SELESAI--\n\n" << endl;
		//cout << "Silahkan cek file anda berikut ini : \n- " << fNameDataTraining + to_string(experimentKe) + ext <<
		//	"\n- " << fNameDataTes + to_string(experimentKe) + ext << endl << endl;
		
	}
	return true;
}

void bootstrap::splitDataSupervised(int jlhRandom) {
	dataTraining.clear();
	dataTes.clear();
	idxDataTes.clear();
	idxDataTraining.clear();
	cout << ".";

	vector<int> vecDataTraining;	//dipakai untuk simpan data index data training
	vector<int> _vecDataTraining;	//dipakai untuk simpan data index data training, tanpa ada yang sama 
	double r;
	int maxIdx = dataSupervised.size() - 1;

	srand(rand());

	for (int i = 0; i < jlhRandom; i++) {
		r = rand() % maxIdx;
		vector<int>::iterator it = find(vecDataTraining.begin(), vecDataTraining.end(), r);
		if (it == vecDataTraining.end()) {
			_vecDataTraining.push_back(r);
		}
		vecDataTraining.push_back(r);
		idxDataTraining.push_back(r);

		//masukkan data training sesuai hasil random index
		dataTraining.push_back(dataSupervised[r]);
	}
	cout << ".";
	//cout << endl;
	//for (auto& col : vec) {
	//	cout << col << ", ";
	//}
	//cout << endl << endl;

	sort(vecDataTraining.begin(), vecDataTraining.end());
	sort(_vecDataTraining.begin(), _vecDataTraining.end());
	cout << ".";

	/*cout << "Complete data training : \n";
	for (auto& col : vecDataTraining) {
		cout << col << ", ";
	}
	cout << endl << endl;
	
	cout << "Real data training, without duplication : \n";
	for (auto& col : _vecDataTraining) {
		cout << col << ", ";
	}
	cout << endl << endl;*/

	//ambil index yang tidak terpilih di data training, jadikan sebagai data tes
	int j = 0;
	for (int i = 0; i < dataSupervised.size(); i++) {
		if (i == _vecDataTraining[j])j++;
		else {
			idxDataTes.push_back(i);

			//masukkan data tes, sesuai data yang tidak terpilih menjadi data training
			dataTes.push_back(dataSupervised[i]);
		}
	}
	cout << ".";

	/*cout << "Complete data tes : \n";
	for (auto& col : idxDataTes) {
		cout << col << ", ";
	}
	cout << endl << endl;*/
}

void bootstrap::clearVector() {
	dataTes.clear();
	dataTraining.clear();
	idxDataTes.clear();
	idxDataTraining.clear();
}

void bootstrap::logData(int act, int K, int eksperimenKe) {
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