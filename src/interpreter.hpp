#pragma once
#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include <string>
#include <list>
#include <vector>
#include <sstream> // Writer

// Reads the data and uses it
class Reader
{
	friend class Interpreter;
protected:
	
	virtual ~Reader() {}
	
	// Current commands retreived by engine
	virtual void onSetupMapSuperRegion(int super_region, int bonus) {}
	virtual void onSetupMapRegion(int region, int super_region) {}
	virtual void onSetupMapNeighbor(int region, const std::vector<int> & neighbors) {}
	virtual void onSetupMapWasteland(int region) {}
	virtual void onPickStartingRegions(float time, const std::vector<int> & regions) {}
	virtual void onPickStartingRegion(float time, const std::vector<int> & regions) {}
	virtual void onSettingsTimebank(float time) {}
	virtual void onSettingsTimePerMove(float time) {}
	virtual void onSettingsMaxRounds(int rounds) {}
	virtual void onSettingsYourBot(const std::string & name) {}
	virtual void onSettingsOpponentBot(const std::string & name) {}
	virtual void onSettingsStartingArmies(int amount) {}
	virtual void onUpdateMap(int region, const std::string & name, int armies) {}
	virtual void onOpponentPlaceArmies(const std::string & name, int region, int amount) {}
	virtual void onOpponentAttackTransfer(const std::string & name, int source_region, int target_region,int amount) {}
	virtual void onGoPlaceArmies(float time) {}
	virtual void onGoAttackTransfer(float time) {}
};

// Sends data to the engine
class Writer
{
	friend class Interpreter;
protected:
	Writer();
	
	// Sending data
	void SetStartingRegions(const std::vector<int> & regions);
	void PickStartingRegion(int region);
	void PlaceArmies(const std::string & name, int region, int amount);
	void AttackTransfer(const std::string & name, int source_region, int target_region, int amount);
	void NoMoves();
	void RandomMove();
	
	// Flush the current buffer
	void Send();
	
private:
	std::stringstream m_buffer;
	class Interpreter * m_interpreter;
};

// Defines which timemode to use
enum TimeModes
{
	TIME_MODE_SECONDS,
	TIME_MODE_MILLISECONDS,
	TIME_MODE_MICROSECONDS,
	TIME_MODE_NANOSECONDS
};

// Handles requests in both directions towards the engine
class Interpreter
{
	friend class Writer;
public:
	Interpreter();
	
	void AddReader(Reader * reader);
	void AddWriter(Writer * writer);

	void Update();
	
	void SetTimeMode(TimeModes time_mode);
	
protected:
	std::string GetMessage();
	std::vector<std::string> GetMessageVector();
	void SendMessage(const std::string &);
	
	void ReceiveCommand(const std::vector<std::string> & cmd);
	
	float ConvertTime(int ms);
	
private:
	std::list<Reader *> m_readers;
	
	TimeModes m_time_mode;
};

#endif // INTERPRETER_HPP