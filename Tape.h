#pragma once
#ifndef _TAPE_H_
#define _TAPE_H_
#include <iostream>
#include <fstream>
#include <string>
#include "windows.h"
#include <vector>

union Int {
	int integer;
	char bytes[4];
};

std::size_t translateToTape(std::string input, std::string output);	//������� � "�������������" �� "���������"
void translateFromTape(std::string input, std::string output);		//������� � "����������" �� "������������"

template <typename T>
// ��������� �����
class ITape {
public:	
	virtual std::size_t getLen() = 0;		//���������� �������� ����� �����
	virtual ~ITape() {}

	virtual void readTape(T& to) = 0;		//������ �� ������� ������
	virtual void Left() = 0;				//���� ��� �����. ���� ��� ����� ������� - ���������� 1, ���� ��� - 0
	virtual void Right() = 0;				//���� ��� ������. ������������ �������� ���������� moveRight
	virtual void writeTape(T& fr) = 0;	//������ � ������� ������
	virtual void copyTape(T& to) = 0;
	virtual void eraseFile() = 0;
	virtual bool isGreater(T& to) = 0;

protected:
	char border;					//����, �� ������ ���� ����� �������� ���-�� �� ����� �����
	std::string file_name;
	static int moveDelay, OIdelay;	//�������� ����������� � ������/������ (���� ��� ���� ����)
	static int memory;				//����� ����������� ������ (� ������)
	std::size_t length,				//����� �����
				used;				//���������� ������� �����		
	std::fstream file;				//���� �����

	virtual void copyBytes(T& to, int count) = 0;
};

// ���� �����
class Tape : public ITape<Tape> {
public:
	Tape(int len, const char filename[]);
	Tape(int len, const char filename[], int mDelay, int ioDelay, int mem);
	~Tape();

	std::size_t getLen() override;
	void Left() override;
	void Right() override;
	void readTape(Tape& to) override;
	void writeTape(Tape& from) override;
	void copyTape(Tape& to) override;
	void eraseFile() override;
	bool isGreater(Tape& to) override;

private:
	void copyBytes(Tape& to, int count) override;
};

#endif // !_TAPE_H_

