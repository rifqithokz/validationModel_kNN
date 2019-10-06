/*
1) Hold out							>> DONE
2) Random Subsampling				>> DONE
3) K-Fold Cross Validation			>> DONE
4) Leave-one-out cross validation	>> DONE
5) Bootstrap						>> DONE

cara pakai :
1. pilih menu validation model yang ingin digunakan
2. masukkan nilai/parameter yang dibutuhkan

setiap selesai running aplikasi, akan muncul beberapa file csv, yaitu :
1. file data tes
	berisi sekumpulan data tes yang dilakukan selama eksperimen
2. file data training
	berisi sekumpulan data training yang dilakukan selama eksperimen
3. file hasil eksperimen
	berisi sekumpulan hasil eksperimen, mulai dari :
	- index data tes
	- hasil uji data tes (sesuai dengan class label nya atau tidak)
	- rasio error setiap eksperimen
	- rasio error rata-rata
*/

#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <math.h>

#include "csv.h"
#include "kNN.h"
#include "holdOut.h"
#include "randomSubSampling.h"
#include "k_Fold.h"
#include "LOO.h"
#include "bootstrap.h"

using namespace std;

string fileDataSupervised = "dataSupervised.csv";
string fileClassLabel = "classLabel.csv";

int jlhLabel = 3;

int main() {
	csv CSV;
	kNN knn;
	holdOut ho;
	randomSubSampling rss;
	k_Fold kf;
	LOO loo;
	bootstrap bs;

	cout << "===================================================\n";
	cout << "=                 Validation Model                =\n";
	cout << "=                        <>                       =\n";
	cout << "=               k-Nearest Neighbours              =\n";
	cout << "===================================================\n\n";

	if (!CSV.fileExists(fileDataSupervised.c_str())) {
		cout << "file \"" << fileDataSupervised << "\" not found!!!" << endl;
		return -1;
	}
	else if (!CSV.fileExists(fileClassLabel.c_str())) {
		cout << "file \"" << fileClassLabel << "\" not found!!!" << endl;
		return -1;
	}

	cout << " Validation model : \n" <<
		"1. Hold Out\n" <<
		"2. Random Subsampling\n" <<
		"3. K-Fold Cross Validation\n" <<
		"4. Leave-One-Out Cross Validation\n" <<
		"5. Bootstraps\n" <<
		"6. Keluar dari aplikasi\n\n"
		"Pilih validation model yang ingin anda jalankan : ";

	int pilihan;
	cin >> pilihan;

	cout << "\n\n";

	if ((pilihan > 6) || (pilihan < 1)) {
		cout << "pilihan tidak sesuai!!!\n\n";
		system("pause");
		return 0;
	}
	else if (pilihan == 1) {
		//tampilan pengaturan
		cout << "Persentase data uji (%) : ";
		int percentage;
		cin >> percentage;
		cout << "\nData yang akan diuji :\n" <<
			"-1     >>  Data tes dipilih secara random\n" <<
			" 0     >>  Semua data tes akan diuji\n" <<
			" 1...n >>  Hanya data tes ke - n, yang akan diuji\n" <<
			"masukkan pilihan anda : ";
		int datake;
		cin >> datake;
		cout << "\nMasukkan nilai konstanta k untuk Nearest Neighbour : ";
		int k=0;
		cin >> k;
		if (k < 1) {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}
		cout << "\nAnda ingin menampilkan hasil pengujian?(y/t) : ";
		bool show=false;
		char inp;
		cin >> inp;
		if (inp == 'y' || inp == 'Y')show = true;
		else if (inp == 't' || inp == 'T')show = false;
		else {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}

		//jalankan holdout
		ho.init(fileDataSupervised, fileClassLabel);
		if(ho.generateDataTraining(percentage))ho.run(k,datake,show);	//-1=random,0=all data, 1-dst = data idx ke 0-dst
	}
	else if (pilihan == 2) {
		//tampilan pengaturan
		cout << "Jumlah eksperimen yang akan dilakukan : ";
		int jlhUji;
		cin >> jlhUji;
		cout << "\nJumlah data tes setiap eksperimen : ";
		int jlhData;
		cin >> jlhData;
		if (jlhData < 1) {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}
		cout << "\nMasukkan nilai konstanta k untuk Nearest Neighbour : ";
		int k = 0;
		cin >> k;
		if (k < 1) {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}
		cout << "\nAnda ingin menampilkan hasil pengujian?(y/t) : ";
		bool show = false;
		char inp;
		cin >> inp;
		if (inp == 'y' || inp == 'Y')show = true;
		else if (inp == 't' || inp == 'T')show = false;
		else {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}

		//jalankan random subsampling
		if (rss.init(fileDataSupervised, fileClassLabel, jlhUji, jlhData)) {
			rss.run(k,show);
		}
	}
	else if (pilihan == 3) {
		//tampilan pengaturan
		cout << "Jumlah eksperimen yang akan dilakukan : ";
		int jlhUji;
		cin >> jlhUji;
		if (jlhUji < 1) {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\nJumlah data tes > jumlah data training.\n\n";
			system("pause");
			return 0;
		}
		cout << "\nMasukkan nilai konstanta k untuk Nearest Neighbour : ";
		int k = 0;
		cin >> k;
		if (k < 1) {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}
		cout << "\nAnda ingin menampilkan hasil pengujian?(y/t) : ";
		bool show = false;
		char inp;
		cin >> inp;
		if (inp == 'y' || inp == 'Y')show = true;
		else if (inp == 't' || inp == 'T')show = false;
		else {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}
		//jalankan k-fold cross validation
		if (kf.init(fileDataSupervised, fileClassLabel, jlhUji)) {
			kf.run(k, show);
		}
	}
	else if (pilihan == 4) {
		//tampilan pengaturan
		cout << "\nMasukkan nilai konstanta k untuk Nearest Neighbour : ";
		int k = 0;
		cin >> k;
		if (k < 1) {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}
		cout << "\nAnda ingin menampilkan hasil pengujian?(y/t) : ";
		bool show = false;
		char inp;
		cin >> inp;
		if (inp == 'y' || inp == 'Y')show = true;
		else if (inp == 't' || inp == 'T')show = false;
		else {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}
		//jalankan LOO cross validation
		if (loo.init(fileDataSupervised, fileClassLabel)) {
			loo.run(1, false);
		}
	}
	else if (pilihan == 5) {
		//tampilan pengaturan
		cout << "\nMasukkan persentase sampling data training dari data supervised : ";
		int persentase = 0;
		cin >> persentase;
		cout << "\nJumlah eksperimen yang akan dilakukan : ";
		int jlhUji = 0;
		cin >> jlhUji;
		cout << "\nMasukkan nilai konstanta k untuk Nearest Neighbour : ";
		int k = 0;
		cin >> k;
		if (k < 1) {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}
		cout << "\nAnda ingin menampilkan hasil pengujian?(y/t) : ";
		bool show = false;
		char inp;
		cin >> inp;
		if (inp == 'y' || inp == 'Y')show = true;
		else if (inp == 't' || inp == 'T')show = false;
		else {
			cout << "ERROR!!!\nnilai yang anda masukkan salah.\n\n";
			system("pause");
			return 0;
		}
		//jalankan bootstrap
		if (bs.init(fileDataSupervised, fileClassLabel, persentase, jlhUji)) {
			bs.run(k, show);
		}
	}
	else {
		cout << "===================================================\n";
		cout << "=                                                 =\n";
		cout << "=                   Terimakasih                   =\n";
		cout << "=                                                 =\n";
		cout << "===================================================\n\n";
	}

	system("pause");
	return 0;
}