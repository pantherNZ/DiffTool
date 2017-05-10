#pragma once

#include <vector>
#include <Windows.h>

class LCS
{
	// Methods
public:
	LCS( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second );
	unsigned int GetLCSLength( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second );
	//std::string ReadFirstLCS( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second );
	//std::vector< std::string > ReadAllLCS( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second );
	void PrintDiff( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second );

private:
	//std::string ReadFirstLCSRecurse( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second, const int _index_first, const int _index_second );
	//std::vector< std::string > ReadAllLCSRecurse( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second, const int _index_first, const int _index_second );
	void PrintDiffRecurse( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second, const size_t _index_first, const size_t _index_second );

	// Members
private:
	unsigned m_index_start;
	unsigned m_index_end_first;
	unsigned m_index_end_second;

	std::vector< size_t > m_hashed_data_first;
	std::vector< size_t > m_hashed_data_second;
	std::vector< std::vector< unsigned > > m_lcs_storage;

	HANDLE m_hConsole;
};