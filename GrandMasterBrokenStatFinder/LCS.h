#pragma once

#include <vector>
#include <Windows.h>
#include <map>

class LCS
{
private:
	enum DifferenceType
	{
		REMOVED = -1,
		NO_DIFFERENCE = 0,
		ADDED = 1,
	};

	// Methods
public:
	LCS( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second );
	unsigned int GetLCSLength( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second );
	void PrintFileWithDiff( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second );
	void Diff( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second, const bool _print_diff );
	void PrintBrokenGrandmasterStats( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second );

	//std::string ReadFirstLCS( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second );
	//std::vector< std::string > ReadAllLCS( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second );

private:
	void Diff( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second, const size_t _index_first, const size_t _index_second, const bool _print_diff );
	void SaveDifference( const size_t index, const DifferenceType difference );
	const bool ExtractStatId( const std::string& _line, std::string& _stat_out );

	//std::string ReadFirstLCSRecurse( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second, const int _index_first, const int _index_second );
	//std::vector< std::string > ReadAllLCSRecurse( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second, const int _index_first, const int _index_second );

	// Members
private:
	unsigned m_index_start;
	unsigned m_index_end_first;
	unsigned m_index_end_second;

	std::vector< size_t > m_hashed_data_first;
	std::vector< size_t > m_hashed_data_second;
	std::vector< std::vector< unsigned > > m_lcs_storage;
	std::map< std::pair< size_t, DifferenceType >, bool > m_differences;

	HANDLE m_hConsole;
};