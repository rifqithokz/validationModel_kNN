#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class csv
{
public:
	bool setTipeData(string tipedata) {
		if (tipedata == "double")tipeData = 0;
		else if (tipedata == "int")tipeData = 1;
		else
		{
			cout << endl << "Maaf, tipe data yang anda inginkan tidak tersedia!" << endl << endl;
			return false;
		}
		return true;
	}	

	bool fileExists(const char* fname)
	{
		FILE* file;
		if (file = fopen(fname, "r"))
		{
			fclose(file);
			return (true);
		}
		return (false);
	}

	vector<vector<int>> read2DInt(string filename, bool showLoadedData) {
		vector<vector<int>> vec;
		string line;
		ifstream f_data(filename);

		while (getline(f_data, line)) {
			string val;
			vector<int> row;
			stringstream s(line);
			while (getline(s, val, ',')) {
				row.push_back(stoi(val));
			}
			vec.push_back(row);
		}
		f_data.close();

		if (showLoadedData) {
			cout << "complete loaded data" << endl << endl;
			for (auto& row : vec) {
				for (auto& val : row)
					cout << val << "  ";
				cout << endl;
			}
		}

		return vec;
	}

	vector<vector<double>> read2DDouble(string filename, bool showLoadedData) {
		vector<vector<double>> vec;
		string line;
		ifstream f_data(filename);

		while (getline(f_data, line)) {
			string val;
			vector<double> row;
			stringstream s(line);
			while (getline(s, val, ',')) {
				row.push_back(stod(val));
			}
			vec.push_back(row);
		}
		f_data.close();

		if (showLoadedData) {
			cout << "complete loaded data" << endl << endl;
			for (auto& row : vec) {
				for (auto& val : row)
					cout << val << "  ";
				cout << endl;
			}
		}

		return vec;
	}

	vector<vector<string>> read2DString(string filename, bool showLoadedData) {
		vector<vector<string>> vec;
		string line;
		ifstream f_data(filename);

		while (getline(f_data, line)) {
			string val;
			vector<string> row;
			stringstream s(line);
			while (getline(s, val, ',')) {
				row.push_back(val);
			}
			vec.push_back(row);
		}
		f_data.close();

		if (showLoadedData) {
			cout << "complete loaded data" << endl << endl;
			for (auto& row : vec) {
				for (auto& val : row)
					cout << val << "  ";
				cout << endl;
			}
		}

		return vec;
	}

	vector<string> read1DString(string filename, bool showLoadedData) {
		vector<string> vec;
		string line;
		ifstream f_data(filename);

		while (getline(f_data, line)) {
			string val;
			vector<string> row;
			stringstream s(line);
			while (getline(s, val, ',')) {
				vec.push_back(val);
			}
		}
		f_data.close();

		if (showLoadedData) {
			cout << "complete loaded data" << endl << endl;
			for (auto& val : vec)
				cout << val << "  ";
			cout << endl;
		}

		return vec;
	}

	//pakai template, walaupun tipe data vectornya int, double dll bisa tetep diproses
	template <typename T>
	void Write2CSVfrom1DVector(string fileName, vector<T> data)
	{
		// file pointer 
		fstream fout;

		// opens an existing csv file or creates a new file. 
		// by default ios::out mode, automatically deletes 
		// the content of file. To append the content, open in ios:app 
		//fout.open(fileName, ios::out | ios::app);
		fout.open(fileName, ios::out);

		int vecSize = data.size();
		for (int i = 0; i < vecSize; i++) {
			if(i<(vecSize-1))fout << data[i] << ",";
			else fout << data[i] << "\n";
		}
		fout.close();
	}

	template <typename T>
	void Write2CSVfrom2DVector(string fileName, vector<vector<T>> data)
	{
		// file pointer 
		fstream fout;

		fout.open(fileName, ios::out);

		int vecRowSize = data.size();
		int vecColSize = data[0].size();

		/*for (int i = 0; i < vecRowSize; i++) {
			for (int j = 0; j < vecColSize; j++) {
 				if (j < (vecColSize - 1))fout << data[i][j] << ",";
				else fout << data[i][j] << "\n";
			}
		}*/
		for (auto& row : data) {
			for (auto& col : row) {
				fout << col << ",";
			}
			fout << "\n";
		}
		fout.close();
	}

private:
	int tipeData=0;	//0=double, 1=integer
};
