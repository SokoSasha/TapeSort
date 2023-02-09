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

std::size_t translateToTape(std::string input, std::string output);	//Перевод с "человеческого" на "ленточный"
void translateFromTape(std::string input, std::string output);		//Перевод с "ленточного" на "человеческий"

template <typename T>
// Интерфейс ленты
class ITape {
public:	
	virtual std::size_t getLen() = 0;		//Возвращает заданную длину ленты
	virtual ~ITape() {}

	virtual void readTape(T& to) = 0;		//Чтение из текущей ячейки
	virtual void Left() = 0;				//Один шаг влево. Если шаг можно сделать - возвращает 1, если нет - 0
	virtual void Right() = 0;				//Один шаг вправо. Возвращаемые значения аналогичны moveRight
	virtual void writeTape(T& fr) = 0;	//Запись в текущую ячейку
	virtual void copyTape(T& to) = 0;
	virtual void eraseFile() = 0;
	virtual bool isGreater(T& to) = 0;

protected:
	char border;					//Флаг, на случай если нужно записать что-то по краям ленты
	std::string file_name;
	static int moveDelay, OIdelay;	//Задержка перемещения и чтения/записи (одна для всех лент)
	static int memory;				//Объем оперативной памяти (в байтах)
	std::size_t length,				//Длина ленты
				used;				//Количество занятых ячеек		
	std::fstream file;				//Файл ленты

	virtual void copyBytes(T& to, int count) = 0;
};

// Сама лента
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

