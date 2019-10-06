/*
class kNN.h
class ini dapat digunakan untuk klasifikasi nearest neighbours (find-s) dengan nilai K sesuai yang diinginkan.
K harus > 0

class ini membutuhkan beberapa data awal, seperti
1) vector<string> namaLabel
	berisi penamaan class label secara urut
	agar output klasifikasi bisa disesuaikan dengan nama class label yang diinginkan

2) vector<vector<double>> dataTraining
	berisi data training untuk klasifikasi, yang terdiri dari n data
	dengan urutan :
	f1,f2,f3,...,fn,classLabel

	*note : classLabel harus di beri nama alias menggunakan angka 0 dst, sesuai banyaknya class label.
			dan usahakan urutannya sama seperti data yang ada pada vector<string> namaLabel

3) vector<double> dataTes
	berisi fitur yang akan di tes, dengan format data :
	f1,f2,f3,...,fn

4) int K
	berisi nilai K yang akan digunakan, berpengaruh terhadap banyaknya peringkat/neighbours yang diambil

cara menggunakan :
1) add library
2) deklarasi object untuk library
3) panggil fungsi setLabel
4) panggil fungsi run
*/
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>

#include <math.h>

using namespace std;

class kNN
{
public:
	vector<vector<double>> dataLabel;
	vector<string> namaLabel;

	void setLabel(vector<string> label);
	int run(vector<vector<double>> dataTraining, vector<double> dataTes, int K, bool showMsg);
	static bool urutkan(const vector<double>& a, const vector<double>& b);

private:
	bool decrement = false;
	bool increment = true;
	static bool tipeSorting;
	static int idx_element;

	void resetDataLabel();
};

bool kNN::tipeSorting = true;
int kNN::idx_element = 0;

bool kNN::urutkan(const vector<double>& a, const vector<double>& b) {
	if (tipeSorting)return(a[idx_element] < b[idx_element]);
	else return(a[idx_element] > b[idx_element]);
}

void kNN::setLabel(vector<string> label) {
	dataLabel.clear();
	namaLabel = label;
	if (namaLabel.size() < 1) {
		cout << endl << "Silahkan masukkan semua nama label terlebih dahulu!!!" << endl;
		return;
	}

	for (int i = 0; i < namaLabel.size(); i++) {
		vector<double> row;
		row.push_back(i);
		row.push_back(0);
		dataLabel.push_back(row);
	}
}

int kNN::run(vector<vector<double>> dataTraining, vector<double> dataTes, int K, bool showMsg) {
	double euclideanDist;
	int i;

	if(showMsg)system("cls");

	resetDataLabel();
	///////////////////////////////hitung jarak euclidean////////////////////////////////////////
	//jarak = ((x-x1)^2) + ((y-y1)^2) + ...
	vector<vector<double>> dataTrain;
	int idx = 0;
	dataTrain = dataTraining;
	if (showMsg)cout << endl;
	for (auto& row : dataTrain) {
		double jarak = 0;
		for (i = 0; i < dataTes.size(); i++) {
			jarak += pow((dataTrain[idx][i] - dataTes[i]), 2);
		}
		euclideanDist = sqrt(jarak);

		dataTrain[idx].push_back(euclideanDist);
		idx++;
	}

	////////////////////Urutkan berdasarkan jarak terdekat dalam bentuk ranking//////////////////
	//cout << endl;
	//cout << "Sebelum diurutkan : " << endl;
	//for (auto& row : dataTrain) {
	//	for (auto& val : row)
	//		cout << val << "  ";
	//	cout << endl;
	//}

	//mengurutkan berdasarkan elemen terakhir (jarak) yang ada di dalam vector.
	//sorting nya increment, mulai dari jarak yang terdekat/terkecil
	idx_element = dataTrain[0].size() - 1;
	tipeSorting = increment;
	sort(dataTrain.begin(), dataTrain.end(), urutkan);

	//cout << endl;
	//cout << "Setelah diurutkan : " << endl;
	//for (auto& row : dataTrain) {
	//	for (auto& val : row)
	//		cout << val << "  ";
	//	cout << endl;
	//}

	///////////////////////////////ambil NN sesuai jumlah parameter K////////////////////////////
	if (showMsg)cout << endl;
	vector<vector<double>> rangking;
	for (i = 0; i < K; i++) {
		rangking.push_back(dataTrain[i]);
	}
	if (showMsg) {
		cout << endl;
		cout << "Data rangking NN : " << endl;

		//atur tampilan header
		int colSize = dataTrain[0].size();
		for (int j = 0; j < colSize; j++) {
			cout << "--------";
		}
		cout << "--" << endl;
		for (int j = 0; j < colSize; j++) {
			if (j == colSize - 1)cout << "| jarak |";
			else if (j == colSize - 2)cout << "| label  ";
			else cout << "|  f" << j + 1 << "\t";
		}
		cout << endl;
		for (int j = 0; j < colSize; j++) {
			cout << "--------";
		}
		cout << "--" << endl;

		//tampilkan rangking
		for (auto& row : rangking) {
			for (auto& val : row)
				cout << "  " << val << "\t";
			cout << endl;
		}
	}

	/////////////////////////////////Voting Class sesuai rangking NN/////////////////////////////
	if (showMsg)cout << endl;
	idx = 0;
	//index class pada vector
	int posClass = dataTraining[0].size() - 1;
	for (auto& row : rangking) {
		//nilai i, mewakili nama alias dari class label, dimulai dari 0, sebanyak jumlah class label
		for (int i = 0; i < namaLabel.size(); i++) {
			if (row[posClass] == i)dataLabel[i][1]++;
		}
	}
	if (showMsg) {
		cout << endl;
		cout << "Data Voting rangking NN : " << endl;
		for (int i = 0; i < dataLabel.size(); i++) {
			cout << "Class " << dataLabel[i][0] << " (" << namaLabel[dataLabel[i][0]] << ")\t: " << dataLabel[i][1] << endl;
		}
	}

	///////////////////////////////Hasil keputusan sesuai rangking NN////////////////////////////
	if (showMsg) cout << endl;
	//mengurutkan berdasarkan elemen ke-1 (hasil voting) yang ada di dalam vector.
	//sorting nya decrement, mulai dari hasil voting terbanyak
	idx_element = 1;
	tipeSorting = decrement;
	sort(dataLabel.begin(), dataLabel.end(), urutkan);

	if (showMsg) {
		cout << "Fitur >> (";
		for (i = 0; i < dataTes.size(); i++) {
			cout << dataTes[i];
			if (i < dataTes.size() - 1)cout << ", ";
		}
		//cout << ") merupakan class : [" << dataLabel[0][0] << "] " << namaLabel[dataLabel[0][0]] << endl;
		cout << ") merupakan class : [" << namaLabel[dataLabel[0][0]] << "] " << endl;

		cout << endl << endl;
	}

	//kirim balik nilai class hasil klasifikasi
	return dataLabel[0][0];
}

void kNN::resetDataLabel() {
	idx_element = 0;
	tipeSorting = increment;
	sort(dataLabel.begin(), dataLabel.end(), urutkan);

	for (int i = 0; i < dataLabel.size(); i++) {
		dataLabel[i][1] = 0;
	}

	for (int i = 0; i < dataLabel.size(); i++) {
		dataLabel[i][1] = 0;
	}
}
