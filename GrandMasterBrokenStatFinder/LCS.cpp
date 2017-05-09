#include "LCS.h"

#include <algorithm>
#include <iostream>

// Compute Longest Common Subsequence 
// Algorithm as per https://en.wikipedia.org/wiki/Longest_common_subsequence_problem

LCS::LCS( const std::string& _data_first, const std::string& _data_second )
	: m_index_start( 1 )
	, m_index_end_first( _data_first.size( ) )
	, m_index_end_second( _data_second.size( ) )
	, m_hConsole( GetStdHandle( STD_OUTPUT_HANDLE ) )
{
	// Trim start & end to try save space
	while( m_index_start < m_index_end_first && m_index_start < m_index_end_second && _data_first[m_index_start] == _data_second[m_index_start] )
		m_index_start++;
	m_index_start--;

	while( m_index_start < m_index_end_first && m_index_start < m_index_end_second && _data_first[m_index_end_first - 1] == _data_second[m_index_end_second - 1] )
	{
		m_index_end_first--;
		m_index_end_second--;
	}

	// Reserve memory for the table
	m_lcs_storage.resize( m_index_end_first - m_index_start + 1 );
	for( unsigned i = 0; i < m_index_end_first - m_index_start + 1; ++i )
		m_lcs_storage[i].resize( m_index_end_second - m_index_start + 1 );

	// Compute table
	GetLCSLength( _data_first, _data_second );
}

unsigned int LCS::GetLCSLength( const std::string& _data_first, const std::string& _data_second )
{
	for( unsigned i = 0; i < m_index_end_first - m_index_start; ++i )
	{
		for( unsigned j = 0; j < m_index_end_second - m_index_start; ++j )
		{
			if( _data_first[i + m_index_start] == _data_second[j + m_index_start] )
				m_lcs_storage[i + 1][j + 1] = m_lcs_storage[i][j] + 1;
			else
				m_lcs_storage[i + 1][j + 1] = max( m_lcs_storage[i + 1][j], m_lcs_storage[i][j + 1] );
		}
	}

	return m_lcs_storage.back( ).back( );
}

std::string LCS::ReadFirstLCS( const std::string& _data_first, const std::string& _data_second )
{
	return ReadFirstLCSRecurse( _data_first, _data_second, _data_first.size( ), _data_second.size( ) );
}

std::string LCS::ReadFirstLCSRecurse( const std::string& _data_first, const std::string& _data_second, const int _index_first, const int _index_second )
{
	if( _index_first == 0 || _index_second == 0 )
		return "";

	if( _data_first[_index_first - 1] == _data_second[_index_second - 1] )
		return ReadFirstLCSRecurse( _data_first, _data_second, _index_first - 1, _index_second - 1 ) + _data_first[_index_first - 1];

	if( m_lcs_storage[_index_first - 1 - m_index_start][_index_second - m_index_start] > m_lcs_storage[_index_first - m_index_start][_index_second - 1 - m_index_start] )
		return ReadFirstLCSRecurse( _data_first, _data_second, _index_first - 1, _index_second );
	
	return ReadFirstLCSRecurse( _data_first, _data_second, _index_first, _index_second - 1 );
}

std::vector< std::string > LCS::ReadAllLCS( const std::string& _data_first, const std::string& _data_second )
{
	return ReadAllLCSRecurse( _data_first, _data_second, _data_first.size( ), _data_second.size( ) );
}

std::vector< std::string > LCS::ReadAllLCSRecurse( const std::string& _data_first, const std::string& _data_second, const int _index_first, const int _index_second )
{
	if( _index_first == 0 || _index_second == 0 )
	{
		std::vector< std::string > results;
		results.push_back( "" );
		return results;
	}

	if( _data_first[_index_first - 1] == _data_second[_index_second - 1] )
	{
		auto results = ReadAllLCSRecurse( _data_first, _data_second, _index_first - 1, _index_second - 1 );

		for( auto& str : results )
			str += _data_first[_index_first - 1];

		return results;
	}
	else
	{
		std::vector< std::string > results;

		if( m_lcs_storage[_index_first - 1 - m_index_start][_index_second - m_index_start] >= m_lcs_storage[_index_first - m_index_start][_index_second - 1 - m_index_start] )
		{
			auto new_results = ReadAllLCSRecurse( _data_first, _data_second, _index_first - 1, _index_second );
			results.insert( results.end( ), new_results.begin( ), new_results.end( ) );
		}

		if( m_lcs_storage[_index_first - m_index_start][_index_second - 1 - m_index_start] >= m_lcs_storage[_index_first - 1 - m_index_start][_index_second - m_index_start] )
		{
			auto new_results = ReadAllLCSRecurse( _data_first, _data_second, _index_first, _index_second - 1 );
			results.insert( results.end( ), new_results.begin( ), new_results.end( ) );
		}

		return results;
	}
}

void LCS::PrintDiff( const std::string& _data_first, const std::string& _data_second )
{
	PrintDiffRecurse( _data_first, _data_second, _data_first.size( ), _data_second.size( ) );
	SetConsoleTextAttribute( m_hConsole, 7 );
}

void LCS::PrintDiffRecurse( const std::string& _data_first, const std::string& _data_second, const int _index_first, const int _index_second )
{
	if( _index_first > 0 && _index_second > 0 && _data_first[_index_first - 1] == _data_second[_index_second - 1] )
	{
		PrintDiffRecurse( _data_first, _data_second, _index_first - 1, _index_second - 1 );
		SetConsoleTextAttribute( m_hConsole, 7 );
		std::cout <<  _data_first[_index_first - 1];
	}
	else if( _index_second > 0 && ( _index_first == 0 ||
		m_lcs_storage[_index_first - m_index_start][_index_second - 1 - m_index_start] >= m_lcs_storage[_index_first - 1 - m_index_start][_index_second - m_index_start] ) )
	{
		PrintDiffRecurse( _data_first, _data_second, _index_first, _index_second - 1 );
		SetConsoleTextAttribute( m_hConsole, 10 );
		std::cout << _data_second[_index_second - 1];
	}
	else if( _index_first > 0 && ( _index_second == 0 ||
		m_lcs_storage[_index_first - m_index_start][_index_second - 1 - m_index_start] < m_lcs_storage[_index_first - 1 - m_index_start][_index_second - m_index_start] ) )
	{
		PrintDiffRecurse( _data_first, _data_second, _index_first - 1, _index_second );
		SetConsoleTextAttribute( m_hConsole, 12 );
		std::cout << _data_first[_index_first - 1];
	}
}