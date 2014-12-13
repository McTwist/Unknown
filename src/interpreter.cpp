#include "interpreter.hpp"

// This file is so full of std library that it makes me sick
// Note: Throwing in exceptions would make it even more fun

#include <iostream>
#include <sstream>
#include <iterator>

#include <stdlib.h>

#include "debug.hpp"

// Split string by delimeter put into input vector
inline std::vector<std::string> & split(const std::string & s, char delim, std::vector<std::string> & elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
		elems.push_back(item);
	return elems;
}

// Split string by delimeter to retrieve created vector
inline std::vector<std::string> split(const std::string & s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

// Implode vector with delimeter put into input string
inline std::string & implode(const std::vector<std::string> & elems, char delim, std::string & s)
{
	for (std::vector<std::string>::const_iterator it = elems.begin(); it != elems.end(); ++it)
	{
		s += *it;
		if (it + 1 != elems.end())
			s += delim;
	}
	return s;
}

// Implode vector with delimeter to retrieve created string
inline std::string implode(const std::vector<std::string> & elems, char delim)
{
	std::string s;
	return implode(elems, delim, s);
}

// Convert to integer
inline std::vector<int> atoi(const std::vector<std::string> & s)
{
	std::vector<int> elems;
	// Make it more effective
	elems.reserve(s.size());
	// Convert all elements
	for (std::vector<std::string>::const_iterator it = s.begin(); it != s.end(); ++it)
		elems.push_back(atoi(it->c_str()));
	return elems;
}

Interpreter::Interpreter()
: m_time_mode(TIME_MODE_MILLISECONDS)
{
}

// Add reader to be called
void Interpreter::AddReader(Reader * reader)
{
	m_readers.push_back(reader);
}

// Add a writer that can call
void Interpreter::AddWriter(Writer * writer)
{
	writer->m_interpreter = this;
}

// Update internal input and output and send to requested objects
void Interpreter::Update()
{
// Use vector
#if 1
	std::vector<std::string> tokens = GetMessageVector();
	
	// Avoid empty
	if (tokens.empty())
		return;
	
// Use string
#else
	// Get next message
	std::string msg = GetMessage();
	
	// Avoid empty
	if (msg.empty())
		return;
	
	// Split 
	std::vector<std::string> tokens = split(msg, ' ');
#endif
	
	ReceiveCommand(tokens);
}

// Set time mode for conversion
void Interpreter::SetTimeMode(TimeModes time_mode)
{
	m_time_mode = time_mode;
}

// Read a message from an input
std::string Interpreter::GetMessage()
{
	std::string msg;
	// Read whole line (Don't care if it doesn't take whole line)
	getline(std::cin, msg);
	// Didn't worked, only took words and not whole lines
	//std::cin >> msg;
	return msg;
}

// Read a message from an input
std::vector<std::string> Interpreter::GetMessageVector()
{
	// Get Message
	std::stringstream ss(GetMessage());
	// Prepare vector
	std::vector<std::string> out;
	// Copy
	std::copy(
		std::istream_iterator<std::string>(ss),
		std::istream_iterator<std::string>(),
		std::back_inserter(out));
	
	return out;
}

// Send message
void Interpreter::SendMessage(const std::string & msg)
{
	std::cout << msg;
	std::cout << std::flush; // Flush the buffer
}

// Recieve commands
void Interpreter::ReceiveCommand(const std::vector<std::string> & cmds)
{
	int size = cmds.size();
	std::string cmd = cmds[0];
	
	// Note: This might be made more dynamically so you can change it easier later on
	
	// Prepare iterator
	std::list<Reader *>::iterator it;
	
	if (cmd == "setup_map")
	{
		// Avoid crash
		if (size < 2)
			return;
		std::string setup = cmds[1];
		// setup_map super_regions [-i -i ...]
		if (setup == "super_regions")
		{
			int super_region = 0, bonus = 0;
			for (int i = 2; i + 1 < size; i += 2)
			{
				super_region = atoi(cmds[i].c_str());
				bonus = atoi(cmds[i+1].c_str());
				// Send in the data
				for (it = m_readers.begin(); it != m_readers.end(); ++it)
					(*it)->onSetupMapSuperRegion(super_region, bonus);
			}
		}
		// setup_map regions [-i -i ...]
		else if (setup == "regions")
		{
			int region = 0, super_region = 0;
			for (int i = 2; i + 1 < size; i += 2)
			{
				region = atoi(cmds[i].c_str());
				super_region = atoi(cmds[i+1].c_str());
				
				// Send in the data
				for (it = m_readers.begin(); it != m_readers.end(); ++it)
					(*it)->onSetupMapRegion(region, super_region);
			}
		}
		// setup_map neighbors [-i [-i,...] ...]
		else if (setup == "neighbors")
		{
			for (int i = 2; i + 1 < size; i += 2)
			{
				int region = atoi(cmds[i].c_str());
				// Get neighbors
				std::vector<int> neighbors = atoi(split(cmds[i+1], ','));
				
				// Send in the data
				for (it = m_readers.begin(); it != m_readers.end(); ++it)
					(*it)->onSetupMapNeighbor(region, neighbors);
			}
		}
		// setup_map wastelands [-i ...]
		else if (setup == "wastelands")
		{
			for (int i = 2; i < size; ++i)
			{
				int region = atoi(cmds[i].c_str());
				
				// Send in the data
				for (it = m_readers.begin(); it != m_readers.end(); ++it)
					(*it)->onSetupMapWasteland(region);
			}
		}
	}
	// pick_starting_regions -t [-i ...]
	else if (cmd == "pick_starting_regions")
	{
		// Avoid crash
		if (size < 2)
			return;
		float time = ConvertTime(atoi(cmds[1].c_str()));
		std::vector<int> regions = atoi(std::vector<std::string>(cmds.begin()+2, cmds.end()));
		
		// Send in the data
		for (it = m_readers.begin(); it != m_readers.end(); ++it)
			(*it)->onPickStartingRegions(time, regions);
	}
	// pick_starting_region -t [-i ...]
	else if (cmd == "pick_starting_region")
	{
		// Avoid crash
		if (size < 2)
			return;
		float time = ConvertTime(atoi(cmds[1].c_str()));
		std::vector<int> regions = atoi(std::vector<std::string>(cmds.begin()+2, cmds.end()));
		
		// Send in the data
		for (it = m_readers.begin(); it != m_readers.end(); ++it)
			(*it)->onPickStartingRegion(time, regions);
	}
	else if (cmd == "settings")
	{
		// Avoid crash
		if (size < 2)
			return;
		std::string settings = cmds[1];
		// settings timebank -i
		if (settings == "timebank")
		{
			float time = ConvertTime(atoi(cmds[2].c_str()));
			// Send in the data
			for (it = m_readers.begin(); it != m_readers.end(); ++it)
				(*it)->onSettingsTimebank(time);
		}
		// settings time_per_move -i
		else if (settings == "time_per_move")
		{
			float time = ConvertTime(atoi(cmds[2].c_str()));
			// Send in the data
			for (it = m_readers.begin(); it != m_readers.end(); ++it)
				(*it)->onSettingsTimePerMove(time);
		}
		// settings max_rounds -i
		else if (settings == "max_rounds")
		{
			int rounds = atoi(cmds[2].c_str());
			// Send in the data
			for (it = m_readers.begin(); it != m_readers.end(); ++it)
				(*it)->onSettingsMaxRounds(rounds);
		}
		// settings your_bot -b
		else if (settings == "your_bot")
		{
			// Send in the data
			for (it = m_readers.begin(); it != m_readers.end(); ++it)
				(*it)->onSettingsYourBot(cmds[2]);
		}
		// settings opponent_bot -b
		else if (settings == "opponent_bot")
		{
			// Send in the data
			for (it = m_readers.begin(); it != m_readers.end(); ++it)
				(*it)->onSettingsOpponentBot(cmds[2]);
		}
		// settings starting_armies -i
		else if (settings == "starting_armies")
		{
			int amount = atoi(cmds[2].c_str());
			// Send in the data
			for (it = m_readers.begin(); it != m_readers.end(); ++it)
				(*it)->onSettingsStartingArmies(amount);
		}
	}
	// update_map [-i -b -i ...]	
	else if (cmd == "update_map")
	{
		int region = 0, amount = 0;
		for (int i = 1; i + 2 < size; i += 3)
		{
			region = atoi(cmds[i].c_str());
			amount = atoi(cmds[i+2].c_str());
			// Send in the data
			for (it = m_readers.begin(); it != m_readers.end(); ++it)
				(*it)->onUpdateMap(region, cmds[i+1], amount);
		}
	}
	// opponent_moves [-m ...]
	else if (cmd == "opponent_moves")
	{
		// Note: Pretty simple implementation, but the rest is not necessary anyway
		// The comma is ignored
		std::string action;
		int region = 0, amount = 0; // place_armies
		int source_region = 0, target_region = 0; // attack/transfer
		for (int i = 1; i + 1 < size;)
		{
			action = cmds[i+1];
			// [-b place_armies -i -i, ...]
			if (action == "place_armies")
			{
				// Check
				if (i + 4 >= size)
					break;
				
				region = atoi(cmds[i+2].c_str());
				amount = atoi(cmds[i+3].c_str());
				// Send in the data
				for (it = m_readers.begin(); it != m_readers.end(); ++it)
					(*it)->onOpponentPlaceArmies(cmds[i], region, amount);
				
				i += 4;
			}
			// [-b attack/transfer -i -i -i, ...]
			else if (action == "attack/transfer")
			{
				// Check
				if (i + 5 >= size)
					break;
				
				source_region = atoi(cmds[i+2].c_str());
				target_region = atoi(cmds[i+3].c_str());
				amount = atoi(cmds[i+4].c_str());
				// Send in the data
				for (it = m_readers.begin(); it != m_readers.end(); ++it)
					(*it)->onOpponentAttackTransfer(cmds[i], source_region, target_region, amount);
				
				i += 5;
			}
		}
	}
	else if (cmd == "go")
	{
		// Avoid crash
		if (size < 2)
			return;
		std::string go = cmds[1];
		// go place_armies -t
		if (go == "place_armies")
		{
			float time = ConvertTime(atoi(cmds[2].c_str()));
			// Send in the data
			for (it = m_readers.begin(); it != m_readers.end(); ++it)
				(*it)->onGoPlaceArmies(time);
		}
		// go attack/transfer -t
		else if (go == "attack/transfer")
		{
			float time = ConvertTime(atoi(cmds[2].c_str()));
			// Send in the data
			for (it = m_readers.begin(); it != m_readers.end(); ++it)
				(*it)->onGoAttackTransfer(time);
		}
	}
}

// Convert to an another time
float Interpreter::ConvertTime(int ms)
{
	switch (m_time_mode)
	{
		case TIME_MODE_SECONDS:
			return ms * 0.001f;
		default:
		case TIME_MODE_MILLISECONDS:
			return ms * 1.0f;
		case TIME_MODE_MICROSECONDS:
			return ms * 1000.0f;
		case TIME_MODE_NANOSECONDS:
			return ms * 1000000.0f;
	}
}

Writer::Writer()
: m_interpreter(0)
{
}

// Set starting regions
void Writer::SetStartingRegions(const std::vector<int> & regions)
{
	// Could check size, but that should be done outside of this
	for (std::vector<int>::const_iterator it = regions.begin(); it != regions.end(); ++it)
	{
		m_buffer << *it;
		// Add spaces
		if (it + 1 != regions.end())
			m_buffer << ' ';
	}
}
// Pick starting region
void Writer::PickStartingRegion(int region)
{
	// Note: Could make so it is stackable
	m_buffer << region;
}

// Place armies
void Writer::PlaceArmies(const std::string & name, int region, int amount)
{
	// [-b place_armies -i -i, ...]
	if (!m_buffer.str().empty())
		m_buffer << ", ";
	m_buffer << name << " place_armies " << region << ' ' << amount;
}

// Attack or transfer armies
void Writer::AttackTransfer(const std::string & name, int source_region, int target_region, int amount)
{
	// [-b attack/transfer -i -i -i, ...]
	if (!m_buffer.str().empty())
		m_buffer << ", ";
	m_buffer << name << " attack/transfer " << source_region << ' ' << target_region << ' ' << amount;
}

// Ignore this round and let it pass
void Writer::NoMoves()
{
	m_buffer << "No moves";
}

// Force a random move
void Writer::RandomMove()
{
	// This is defined in the engine
	NoMoves();
}

// Flush the buffer
void Writer::Send()
{
	// Avoid crashing
	// Note: May apply some other object to make sure all writers are invalid
	// Note2: May even skip the object and write directly
	if (!m_interpreter)
		return;
	// New line (Required)
	m_buffer << '\n';
	// Send it
	m_interpreter->SendMessage(m_buffer.str());
	// Clear to make room for next one
	m_buffer.str("");
	m_buffer.clear();
}
