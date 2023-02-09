/*��� ������� ����, ��� ����������� �� ����� - ��� ����� ��������� �������� �,
* ������ �������, �� �� ����� ����������� ���������� � �� ���������,
* ����� ���������� �� ��� ������ ����������� ����� ��������� ����������.
* �� ����� ����� ����� "���������", ����� ����������� �����-���� �����, ����
* ������������ ����� �������� ��������.
* 
* ������� ��������� �������� ������ � �������, �.�. ����� ���������� ������ � ����� �����,
* � ������� �� �����, ��� �� ����� ������������ ����� ������. ����� ����������� ����� �� ���������� �����,
* �������� ��������� �������������� ���� (��� ����� �� �������� �� ������)
*/
#define _CRT_SECURE_NO_WARNINGS

#include "Tape.h"
#include <sstream>

std::size_t N = 10;
std::size_t M = 10;


std::string processInput(char* txt) {
	std::string tape, stxt(txt);
	int dot = stxt.find("txt");
	if (dot != -1) {
		tape = stxt.substr(0, dot) + "tape";
		N = translateToTape(stxt, tape);
		return tape;
	}

	return stxt;
}

std::string processOutput(char* txt) {
	std::string tape, stxt = txt;
	int dot = stxt.find("txt");
	if (dot != -1) {
		tape = stxt.substr(0, dot) + "tape";
		std::fstream tmp(tape, std::ios::out | std::ios::trunc);
		tmp.close();
		return tape;
	}

	return stxt;
}

//������������ ��������� � ����� ������� 2 �������������� ��������� �����
void swapWithLeft(Tape& cur, Tape& prev) {
	Tape tmp(1, processOutput((char*)"tmp/tmp.txt").c_str());
	tmp.eraseFile();
    
	cur.readTape(tmp);
	cur.writeTape(prev);

	cur.Left();
	cur.writeTape(tmp);

	cur.Right();

	tmp.eraseFile();
}

void swapWithRight(Tape& cur, Tape& prev) {
	Tape tmp(1, processOutput((char*)"tmp/tmp.tape").c_str());
	tmp.eraseFile();

	cur.readTape(tmp);
	cur.writeTape(prev);

	cur.Right();
	cur.writeTape(tmp);

	cur.Left();

	tmp.eraseFile();
}

void shakerSort(Tape& tape) {
	Tape buf(1, processOutput((char*)"tmp/buf.tape").c_str());
	buf.eraseFile();
    //����� ����������� �����, ��������� �������� �� ��������� ��������, ������� ���� � �����
    bool swapped = true;
    std::size_t end = tape.getLen() - 1;

    while (swapped) {
        for (int i = 0; i < end; i++) {
			if (!swapped)
				tape.readTape(buf);
			swapped = false;
            tape.Left();
            if (tape.isGreater(buf)) {
                swapWithRight(tape, buf);
                swapped = true;
            }
        }
        tape.Right();

        if (!swapped)
            break;

        swapped = false;
        end--;

        for (int i = 0; i < end; i++) {
			if (!swapped)
				tape.readTape(buf);
			swapped = false;
            tape.Right();
            if (buf.isGreater(tape)) {
                swapWithLeft(tape, buf);
                swapped = true;
            }
        }

        tape.Left();
        end--;
    }
}

int main(int argc, char** argv) {

	std::fstream config;
	config.open("config.txt", std::ios::in | std::ios::app);
	int mDel = 0, oiDel = 0;

	std::string params;
	std::getline(config, params);
	mDel = stoi(params.substr(20));

	std::getline(config, params);
	oiDel = stoi(params.substr(12));

	argv[1] = (char*)"input.txt";
	argv[2] = (char*)"output.txt";

	std::string in = processInput(argv[1]);
	std::string out = processOutput(argv[2]);

	Tape input(N, in.c_str(), mDel, oiDel, M);
	Tape output(N, out.c_str());

	input.copyTape(output);

	shakerSort(output);

	translateFromTape("output.tape", "output.txt");
}