#pragma once

#include <vector>
#include <Windows.h>

class LCS
{
	// Methods
public:
	LCS( const std::string& _data_first, const std::string& _data_second );
	unsigned int GetLCSLength( const std::string& _data_first, const std::string& _data_second );
	std::string ReadFirstLCS( const std::string& _data_first, const std::string& _data_second );
	std::vector< std::string > ReadAllLCS( const std::string& _data_first, const std::string& _data_second );
	void PrintDiff( const std::string& _data_first, const std::string& _data_second );

private:
	std::string ReadFirstLCSRecurse( const std::string& _data_first, const std::string& _data_second, const int _index_first, const int _index_second );
	std::vector< std::string > ReadAllLCSRecurse( const std::string& _data_first, const std::string& _data_second, const int _index_first, const int _index_second );
	void PrintDiffRecurse( const std::string& _data_first, const std::string& _data_second, const int _index_first, const int _index_second );

	// Members
private:
	unsigned m_index_start;
	unsigned m_index_end_first;
	unsigned m_index_end_second;
	std::vector< std::vector< unsigned int > > m_lcs_storage;

	HANDLE m_hConsole;
};