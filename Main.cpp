/*ѕри условии того, что перемещение по ленте - это очень затратна€ операци€ и,
* помимо прочего, мы не можем произвольно обращатьс€ к ее элементам,
* самой выигрышной на мой взгл€д сортировкой будет шейкерна€ сортировка.
* Ќе нужно будет много "пробегать", чтобы переставить какие-либо числа, ведь
* переставл€ть нужно соседние элементы.
* 
* Ѕольшую сложность вызывает работа с файлами, т.к. можно записывать только в конец файла,
* в отличие от ленты, где мы можем перезаписать любой символ. „тобы реализовать такой же функционал ленты,
* придетс€ полностью перезаписывать файл (что очень не хотелось бы делать)
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

//ѕерестановка элементов в ленте требует 2 дополнительные маленькие ленты
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
    //ѕосле копировани€ ленты, указатель оказалс€ на последнем элементе, поэтому идем с конца
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