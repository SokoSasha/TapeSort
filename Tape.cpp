#include "Tape.h"

/* ��� ��� �� �� ����� ��������� "���������" ��������������� �� ����� � �����-�����,
* �� ���������, ��� ���� ��������� ��������� ����� ������ (���� ����� �������� � ������), ������ �� ��������� �� ����
* 
* |1 23 771 837182 ... |
*      ^
* ��������� �� 771
* 
* � ������, ���� ��������� ��������� � ������ ����� ����� ���� 2 ��������:
*	- ���� border == 0, ��������� �� �������� ������ �� ����;
*	- ���� border == -1, �� ��������� �� ������ ����� ����� �� ������� ��������.
* 
*/

int Tape::OIdelay = 0;
int Tape::moveDelay = 0;
int Tape::memory = 0;

Tape::Tape(int len, const char filename[]) {
	file.open(filename, std::ios::in | std::ios::out | std::ios::app | std::ios::binary);

	if (!file.is_open()) {
		eraseFile();
		if (!file.is_open()) {
			std::cout << "Cannot open file!" << std::endl;
			exit(-1);
		}
	}
	length = len;
	file_name = filename;
	border = 0;
}

Tape::Tape(int len, const char filename[], int mDelay, int ioDelay, int mem) : Tape(len, filename) {
	ITape::moveDelay = mDelay;
	ITape::OIdelay = ioDelay;
	ITape::memory = mem;
}

Tape::~Tape() {
	file.close();
}

//������� ���� ������� � ���� "1 2 3 ..."
//������ ������� ����������� ����� �� ���������� ����� � ������������������ ����,
//������� ��� �������������, �.�. � �����
//�����-���� ������������ � ����� ���, ������ ��� ������ int ������������ ����� ����� 4 �����
std::size_t translateToTape(std::string input, std::string output) {
	std::fstream inFile;
	if (!inFile.is_open()) {
		inFile.open(input, std::ios::in | std::ios::app);
	}

	std::fstream outFile;
	if (!outFile.is_open()) {
		outFile.open(output, std::ios::out | std::ios::trunc | std::ios::binary);
	}

	inFile.seekg(0);
	outFile.seekg(0);

	Int tmp;
	while(!inFile.eof()){
		inFile >> std::skipws >> tmp.integer;
		outFile.write(tmp.bytes, 4);
	}

	outFile.seekg(0, std::ios::end);
	auto g = outFile.tellg();
	return g / 4;

	inFile.close();
	outFile.close();
}

void translateFromTape(std::string input, std::string output) {
	std::fstream inFile;
	if (inFile.is_open())
		inFile.close();
	inFile.open(input, std::ios::in | std::ios::app | std::ios::binary);

	std::fstream outFile;
	if (outFile.is_open())
		outFile.close();
	outFile.open(output, std::ios::out | std::ios::trunc);

	inFile.clear();
	outFile.clear();
	inFile.seekg(0);
	outFile.seekp(0);

	Int tmp;
	char* chk = new char[1];
	while (!inFile.eof()) {
		inFile.read(tmp.bytes, 4);

		auto g = inFile.tellg();
		inFile.read(chk, 1);
		if (inFile.eof()) {
			outFile << tmp.integer << " ";
			break;
		}
		else
			inFile.seekg(g);

		outFile << tmp.integer << " ";
	}

	inFile.close();
	outFile.close();
}

// ���������� ��������� ���������� ���� � ����������� ������
void Tape::copyBytes(Tape& to, int count) {
	int del = count;
	if (to.length == 1)
		to.file.seekg(0);
	if (length == 1)
		file.seekg(0);

	auto g = file.tellg();
	auto tg = to.file.tellg();

	int size = memory > count ? count : memory;

	char* tmp = new char[size];
	while (count) {
		file.read(tmp, size);
		to.file.write(tmp, size);
		count -= size;
		if (count < size)
			size = count;
	}
	delete[] tmp;
	
	if (to.length == 1)
		to.file.seekg(0);
	if (length == 1)
		file.seekg(0);
}

void Tape::readTape(Tape& to) {
	to.eraseFile();

	if (border == 1 or border == -1)
		return;

	auto g = file.tellg();

	copyBytes(to, 4);

	if (file.fail()) {
		if (file.bad()) {
			std::cout << "Reading error" << std::endl;
			exit(-1);
		}
		file.clear();
	}
	file.seekg(g);
	to.file.seekg(0);
	Sleep(OIdelay * 2);
}

void Tape::Left() {
	if (border == -1)
		return;

	//���� �� ��� � ������ ����� � �������� ��������� �����
	if (file.tellg() == 0) {
		//���� ����� ������, �� ��� ��� ������ ���������
		border = -1 + (used == 0);
		file.seekg(0);
		Sleep(moveDelay);
		return;
	}

	file.seekg(-4, std::ios::cur);

	Sleep(moveDelay);
	border = 0;
}

void Tape::Right() {
	if (border == 1)
		return;

	if (border == -1) {
		border = 0;
		return;
	}

	auto g = file.tellg();

	file.seekg(0, std::ios::end);
	auto tg = file.tellg();
	file.seekg(g);

	file.seekg(4, std::ios::cur);

	//���������, �� ������� �� �� � ���� �����
	g = file.tellg();
	if (g == tg) {
		border = 1;
		Sleep(moveDelay);
		return;
	}

	/**************************************************/
	//std::cout << file_name << " Right: " << std::endl;
	/**************************************************/

	Sleep(moveDelay);
	border = 0;
}

// �� ���� ����� �������� ����� �������, ��� �� ����� ������������ �������� � ����� �� ������,
// � ������� �� �����, ������� �� ����� ������ ��������� (���� ��������� �������)
// ����� �����-����� �������� ���������� ��������� ������, ��� ���������� ����� ����� ����� ������� �������������� ����,
// �.�. ���������� ���������� ����� � ������ � ���������� �� ������� � ���������� ����������.
// ������, ������� ����� ��������� ��� �� ������ ������ �����
// 
// � �� ���� �������, ������������� �� ����� ������ �������� "���� ����������� �� ������������� ����������� ������ � �� ����� M ����",
// ������� �� ������ ������ ����������� ���� ������

void Tape::writeTape(Tape& from) {
	auto tg = from.file.tellg();
	auto g = file.tellg();

	//���� ������� �� ������ ���� �����, �� ������ ���������� ����
	if (border == 1) {
		if (used >= length) {
			std::cout << "Out of tape" << std::endl;
			system("pause");
			return;
		}

		from.copyBytes(*this, 4);

		border = 0;
		used++;
		file.seekg(g);
		Sleep(OIdelay * 2);
		return;
	}
	
	//���� ������� �� ����� ���� �����, �� ����� ���������� ���� ���� � �����,
	//�������� � ������ ���� �����, � ����� �������� �����
	if (border == -1) {
		if (used >= length) {
			std::cout << "Out of tape" << std::endl;
			system("pause");
			return;
		}

		used++;

		//���� ����������� ������ �� �������, �� ���� ������� ����������� � �����-�����
		if (memory < (length + 1) * 4) {
			Tape bufferTape(length, "tmp/Buffer.tape");
			bufferTape.eraseFile();

			copyBytes(bufferTape, length * 4);
			eraseFile();

			from.copyBytes(*this, 4);
			bufferTape.copyBytes(*this, length * 4);

			file.seekg(0);
			bufferTape.eraseFile();
			Sleep(OIdelay * 2);
			return;
		}

		char* Buffer = new char[4 * length];
		from.file.read(Buffer, 4);

		file.seekg(0);
		file.read(Buffer, 4 * length);

		eraseFile();

		from.copyBytes(*this, 4);
		file.write(Buffer, length * 4);

		delete[] Buffer;
		file.seekg(0);
		Sleep(OIdelay * 2);
		return;
	}

	//������, ���� ����� �������� ����� � ������������ ������ �����

	//���� �� ����������� ����� �� ������ ����������� ������
	if (memory < (length + 1) * 4) {
		Tape bufferTape(length, "tmp/Buffer.tape");
		bufferTape.eraseFile();

		file.seekg(0);
		copyBytes(bufferTape, std::size_t(g)); //��������� ����� ����� ������ � �����

		Tape trash(1, "tmp/trash.tape");
		trash.eraseFile();
		copyBytes(trash, 4);	//���������� ������, ������� ����� ��������

		copyBytes(bufferTape, (length - 1) * 4 - std::size_t(g));	//��������� � ����� ��������� ������

		eraseFile();				//������� ����
		bufferTape.file.seekg(0);

		bufferTape.copyBytes(*this, std::size_t(g));	//���������� ����� �� ������ ������ ����� �����
		from.copyBytes(*this, 4);						//���������� ����� � ������
		bufferTape.copyBytes(*this, (length - 1) * 4 - std::size_t(g));	//���������� ��� ���������

		file.seekg(g);
		bufferTape.eraseFile();
		trash.eraseFile();
		Sleep(OIdelay * 2);
		return;
	}

	//���� ����������� ������ �������, �� ������ �������� ����� ����
	char* Buffer = new char[4 * (length + 1)];

	file.seekg(0);
	file.read(Buffer, std::size_t(g));	//���������� ����� �� ������ ����� � ��

	from.file.read(Buffer + g, 4);	//������ ����� ���� ��
	from.file.seekg(0);

	Right();	//���������� ���������� �����
	file.read(Buffer + std::size_t(g) + 4, (length - 1) * 4 - std::size_t(g));	//���������� ��� ���������

	eraseFile();
	file.write(Buffer, 4 * length);	//���������� ��� � �����

	delete[] Buffer;
	
	file.seekg(g);

	Sleep(OIdelay * 2);
}

void Tape::eraseFile() {
	file.close();
	file.open(file_name, std::ios::out | std::ios::trunc);
	file.close();
	file.open(file_name, std::ios::out | std::ios::in | std::ios::app | std::ios::binary);
	file.clear();
	file.seekg(0);
}

std::size_t Tape::getLen() {
	return length;
}

void Tape::copyTape(Tape& to) {
	file.seekg(0);
	to.file.seekg(0);

	copyBytes(to, to.length * 4);

	to.used = length;

	file.close();
	to.file.close();

	file.open(file_name, std::ios::in | std::ios::out | std::ios::app);
	to.file.open(to.file_name, std::ios::in | std::ios::out | std::ios::app | std::ios::binary);

	file.seekg(0, std::ios::end);
	auto g = file.tellg();
	g -= 4;
	file.seekg(g);

	to.file.seekg(0, std::ios::end);
	g = to.file.tellg();
	g -= 4;
	to.file.seekg(g);

	int Delay = OIdelay * length * 2 + moveDelay * (length - 1) * 2;
	Sleep(Delay);
}

//���������, ��� ��������� ����� ���������� �� � ����������� ������, �.�.
//�� �� ����� ������� ������ ��� 1 ���� �� ����� �� ���. �.�. ��������, ����
//� ��� ����� 1 ���� ����������� ������, �� �������� 2 ����� ����� ������ ����������
bool Tape::isGreater(Tape& to) {
	auto g = file.tellg(),
		tg = to.file.tellg();

	Int num, toNum;
	file.read(num.bytes, 4);
	to.file.read(toNum.bytes, 4);

	file.seekg(g);
	to.file.seekg(tg);

	return num.integer > toNum.integer;

	//�������� �������� �� ������
	Sleep(OIdelay * 2);
}