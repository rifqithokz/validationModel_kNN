/*
langkah :
1. proses inisialisasi dengan fungsi init
2. generate random index, untuk mengambil data tes sesuai dengan jumlah yang diinginkan
	karena random, urutan indexnya di sorting increment. supaya gampang nanti pas ambil dari data supervised
3. proses pengambilan data tes dari data supervised sesuai proses langkah 2
4. data yang tidak terpilih menjadi data tes, kemudian dikelompokkan menjadi data training
	simpan data training dan data tes ke csv
5. jalankan k-NN sesuai data tes dan data training
	hasil eksperimen disimpan ke dalam file csv
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

class randomSubSampling
{
public:
	bool init(string fDataSupervised, string fClassLabel, int jlhEksperimen, int jlhDataTes);
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

	int jlhLabel;
	int jlhExperiment;	//banyaknya eksperimen yang dilakukan
	int jlhDataSekaliTes;
	bool nextProcess;
	string fNameDataTraining = "dataTraining_rss_";
	string fNameDataTes = "dataTes_rss_";
	string ext = ".csv";
	string fNameHasilUji = "hasilUji_rss";

	bool generateDataTraining(int jlhDataTes, int experimentKe);
	vector<int> getIdxDataTes(int jlhRandom);
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

bool randomSubSampling::init(string fDataSupervised, string fClassLabel, int jlhEksperimen, int jlhDataTes) {
	dataSupervised.clear();
	label.clear();
	logDataTes.clear();
	logDataTraining.clear();
	dataSupervised = CSV.read2DDouble(fDataSupervised, false);
	label = CSV.read1DString(fClassLabel, false);

	jlhLabel = label.size();
	nextProcess = true;
	jlhExperiment = jlhEksperimen;

	jlhDataSekaliTes = jlhDataTes;

	if (jlhDataTes >= (dataSupervised.size()/2)) {
		cout << "ERROR!!!\nJumlah data tes setiap eksperimen terlalu banyak!!!\nSilahkan sesuaikan dengan data supervised.\n\n";
		nextProcess = false;
		return false;
	}

	return true;
}

bool randomSubSampling::generateDataTraining(int jlhDataTes, int experimentKe) {
	int i, j;

	if (nextProcess) {
		if (jlhDataSekaliTes >= (dataSupervised.size() / 2)) {
			cout << "ERROR!!!\nJumlah data tes setiap eksperimen terlalu banyak!!!\nSilahkan sesuaikan dengan data supervised.\n\n";
			nextProcess = false;
			return false;
		}
		else {
			cout << "Mengelompokkan data training dan data testing - " << experimentKe << endl;

			//data tes akan diambil sebanyak jlhDataSekaliTes secara random dari data supervised,
			//sisanya akan dimasukkan menjadi data training

			//generate random index untuk ambil data tes dari data supervised sebanyak jlhDataSekaliTes
			idxDataTes = getIdxDataTes(jlhDataSekaliTes);
			cout << ".";

			//mulai split dataSupervised, sesuai jlhDataSekaliTes ke data training dan data tes
			//sesuai index yang didapat dari random
			j = 0;
			for (i = 0; i < dataSupervised.size(); i++) {
				if (i == idxDataTes[j]) {
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
			
			//simpan data tes dan data training ke csv
			//CSV.Write2CSVfrom2DVector(fNameDataTraining + to_string(experimentKe) + ext, dataTraining);
			//CSV.Write2CSVfrom2DVector(fNameDataTes + to_string(experimentKe) + ext, dataTes);

			//cout << "\n\n--SELESAI--\n\n" << endl;
			//cout << "Silahkan cek file anda berikut ini : \n- " << fNameDataTraining + to_string(experimentKe) + ext <<
			//	"\n- " << fNameDataTes + to_string(experimentKe) + ext << endl << endl;
		}
	}
	return true;
}

vector<int> randomSubSampling::getIdxDataTes(int jlhRandom) {
	vector<int> vec;
	double r;
	int maxIdx = dataSupervised.size() - 1;

	srand(rand());
	for (int i = 0; i < jlhRandom; i++) {
		r = rand()% maxIdx;
		vector<int>::iterator it = find(vec.begin(), vec.end(), r);
		if (it != vec.end()) {
			i--;
		}
		else {
			vec.push_back(r);
		}
	}

	//cout << endl;
	//for (auto& col : vec) {
	//	cout << col << ", ";
	//}
	//cout << endl << endl;

	sort(vec.begin(), vec.end());

	//for (auto& col : vec) {
	//	cout << col << ", ";
	//}
	//cout << endl << endl;
	
	return vec;
}

void randomSubSampling::run(int K, bool showMsg) {
	//K -> konstanta Nearest neighbours
	
	logData(0,K,0);

	for (int i = 0; i < jlhExperiment; i++) {
		//generate dulu model data tes dan data trainingnya
		generateDataTraining(jlhDataSekaliTes, i+1);

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

		double rasioError = ((double)x / (double)jlh);
		cout << "\n\nmissclassified = " << x << "\njumlah data test : " << jlh << endl;
		cout << "rasio error : " << rasioError << endl << endl;
		
		
		if(showMsg)tunggu(false);	//true>>clearscreen, false>>tampilan sebelumnya tetap utuh
		
		//log hasil output
		hasilEksperimen.push_back({ "Eksperimen ke " , to_string(i+1) });
		hasilEksperimen.push_back({ "index data tes di data supervised" });
		hasilEksperimen.push_back(tostringVec(idxDataTes));
		hasilEksperimen.push_back({ "Hasil uji :" });
		hasilEksperimen.push_back(tostringVec(hasilOutput));
		hasilEksperimen.push_back({ "Rasio error eksperimen ke-" + to_string(i+1) , to_string(rasioError)});
		hasilEksperimen.push_back({});

		totalError.push_back(rasioError);
		logData(1, K, i + 1);

		clearVector();
	}

	//hitung total error
	double errorRataRata=0;
	for (auto& col : totalError) {
		errorRataRata += col;
	}

	//bagi total error dengan jumlah pengujian
	errorRataRata = errorRataRata / jlhExperiment;

	//update log terakhir
	hasilEksperimen.push_back({});
	hasilEksperimen.push_back({});
	hasilEksperimen.push_back({"Error rata-rata : " , to_string(errorRataRata)});

	//simpan log uji
	CSV.Write2CSVfrom2DVector(fNameHasilUji + "_" + to_string(K) + "-NN" + ext, hasilEksperimen);
	cout << "lihat log hasil uji di : " << fNameHasilUji + "_" + to_string(K) + "-NN" + ext << endl << endl;
	hasilEksperimen.clear();
	totalError.clear();

	logData(2, K, 0);
}

int randomSubSampling::runKnn(int K, int dataKe, bool showMsg) {
	vector<double> dataTesting;

	for (int i = 0; i < (dataTes[dataKe - 1].size() - 1); i++) {
		dataTesting.push_back(dataTes[dataKe - 1][i]);
	}
	//for (auto& dt : dataTesting)cout << dt << ", ";
	//cout << endl;

	int classSeharusnya = dataTes[dataKe - 1][(dataTes[dataKe - 1].size())-1];
	int output = knn.run(dataTraining, dataTesting, K, showMsg);

	//bandingkan hasil output klasifikasi dengan class seharusnya
	if (output == classSeharusnya)return 1;
	else return 0;
}

void randomSubSampling::clearVector() {
	dataTes.clear();
	dataTraining.clear();
	idxDataTes.clear();
}

void randomSubSampling::logData(int act, int K, int eksperimenKe) {
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