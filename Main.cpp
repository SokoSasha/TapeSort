/*При условии того, что перемещение по ленте - это очень затратная операция и,
* помимо прочего, мы не можем произвольно обращаться к ее элементам,
* самой выигрышной на мой взгляд сортировкой будет шейкерная сортировка.
* Не нужно будет много "пробегать", чтобы переставить какие-либо числа, ведь
* переставлять нужно соседние элементы.
* 
* Большую сложность вызывает работа с файлами, т.к. можно записывать только в конец файла,
* в отличие от ленты, где мы можем перезаписать любой символ. Чтобы реализовать такой же функционал ленты,
* придется полностью перезаписывать файл (что очень не хотелось бы делать)
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

//Перестановка элементов в ленте требует 2 дополнительные маленькие ленты
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
    //После копирования ленты, указатель оказался на последнем элементе, поэтому идем с конца
    bool swapped = true;
    std::size_t end = tape.getLen() - 1;

    while (swapped) {
		swapped = false;
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

	if (argc != 3) {
		std::cout << "Wrong command" << std::endl << "Use default setings" << std::endl;
		argv[1] = (char*)"input.txt";
		argv[2] = (char*)"output.txt";
	}

	std::fstream config;
	config.open("config.txt", std::ios::in | std::ios::app);
	int mDel = 0, oiDel = 0;

	std::string params;
	std::getline(config, params);
	mDel = stoi(params.substr(20));

	std::getline(config, params);
	oiDel = stoi(params.substr(12));

	std::string in = processInput(argv[1]);
	std::string out = processOutput(argv[2]);

	Tape input(N, in.c_str(), mDel, oiDel, M);
	Tape output(N, out.c_str());

	input.copyTape(output);

	shakerSort(output);

	translateFromTape("output.tape", "output.txt");
}
