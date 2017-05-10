#include "LCS.h"

#include <algorithm>
#include <iostream>
#include <functional>

// Compute Longest Common Subsequence 
// Algorithm as per https://en.wikipedia.org/wiki/Longest_common_subsequence_problem

LCS::LCS( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second )
	: m_index_start( 0 )
	, m_index_end_first( ( unsigned )_data_first.size( ) )
	, m_index_end_second( ( unsigned )_data_second.size( ) )
	, m_hConsole( GetStdHandle( STD_OUTPUT_HANDLE ) )
{
	// Hash the lines for fast evaluation
	m_hashed_data_first.reserve( 35000 );
	m_hashed_data_second.reserve( 35000 );

	std::hash< std::string > hash_function;

	for( auto& str : _data_first )
		m_hashed_data_first.push_back( hash_function( str ) );

	for( auto& str : _data_second )
		m_hashed_data_second.push_back( hash_function( str ) );

	// Trim start & end to try save space
	while( m_index_start < m_index_end_first && m_index_start < m_index_end_second && m_hashed_data_first[m_index_start] == m_hashed_data_second[m_index_start ] )
		m_index_start++;
	
	while( m_index_start < m_index_end_first && m_index_start < m_index_end_second && m_hashed_data_first[m_index_end_first - 1] == m_hashed_data_second[m_index_end_second - 1] )
	{
		m_index_end_first--;
		m_index_end_second--;
	}

	// Reserve memory for the table
	m_lcs_storage.resize( m_index_end_first - m_index_start + 1 );
	for( unsigned i = 0; i < m_index_end_first - m_index_start + 1; ++i )
		m_lcs_storage[i].resize( m_index_end_second - m_index_start + 1 );

	// Compute table
	GetLCSLength( m_hashed_data_first, m_hashed_data_second );
}

unsigned int LCS::GetLCSLength( const std::vector< size_t >& _data_first, const std::vector< size_t >& _data_second )
{
	for( unsigned i = 1; i < m_index_end_first - m_index_start + 1; ++i )
	{
		for( unsigned j = 1; j < m_index_end_second - m_index_start + 1; ++j )
		{
			if( i == 0 || j == 0 )
				m_lcs_storage[i][j] = 0;
			else if( _data_first[i + m_index_start - 1] == _data_second[j + m_index_start - 1] )
				m_lcs_storage[i][j] = m_lcs_storage[i - 1][j - 1] + 1;
			else
				m_lcs_storage[i][j] = max( m_lcs_storage[i][j - 1], m_lcs_storage[i - 1][j] );
		}
	}

	return m_lcs_storage.back( ).back( );
}

void LCS::PrintDiff( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second )
{
	// Escape if the same
	if( m_lcs_storage.size( ) < 2 && m_lcs_storage[0].size( ) < 2 )
	{
		std::cout << "No differences found";
		return;
	}

	// Print skipped over lines that weren't changed at the start & end of the file
	SetConsoleTextAttribute( m_hConsole, 7 );

	for( unsigned i = 0; i < m_index_start; ++i )
		std::cout << _data_first[i].c_str( ) << std::endl;

	PrintDiffRecurse( _data_first, _data_second, m_index_end_first, m_index_end_second );
	SetConsoleTextAttribute( m_hConsole, 7 );

	for( unsigned i = m_index_end_first; i < _data_first.size( ); ++i )
		std::cout << _data_first[i].c_str( ) << std::endl;
}

void LCS::PrintDiffRecurse( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second, const size_t _index_first, const size_t _index_second )
{
	if( _index_first > m_index_start && _index_second > m_index_start && m_hashed_data_first[_index_first - 1] == m_hashed_data_second[_index_second - 1] )
	{
		PrintDiffRecurse( _data_first, _data_second, _index_first - 1, _index_second - 1 );
		SetConsoleTextAttribute( m_hConsole, 7 );
		std::cout << _data_first[_index_first - 1].c_str( ) << std::endl;
	}
	else
	{
		const auto cell_left = ( _index_first > m_index_start ? m_lcs_storage[_index_first - 1 - m_index_start][_index_second - m_index_start] : 0 );
		const auto cell_up = ( _index_second > m_index_start ? m_lcs_storage[_index_first - m_index_start][_index_second - 1 - m_index_start] : 0 );

		if( _index_second > m_index_start && ( _index_first <= m_index_start || cell_up >= cell_left ) )
		{
			PrintDiffRecurse( _data_first, _data_second, _index_first, _index_second - 1 );
			SetConsoleTextAttribute( m_hConsole, 10 );
			std::cout << _data_second[_index_second - 1].c_str( ) << std::endl;
		}
		if( _index_first > m_index_start && ( _index_second <= m_index_start || cell_up < cell_left ) )
		{
			PrintDiffRecurse( _data_first, _data_second, _index_first - 1, _index_second );
			SetConsoleTextAttribute( m_hConsole, 12 );
			std::cout << _data_first[_index_first - 1].c_str( ) << std::endl;
		}
	}
}

/*
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

std::vector< std::string > LCS::ReadAllLCS( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second )
{
	return ReadAllLCSRecurse( _data_first, _data_second, _data_first.size( ), _data_second.size( ) );
}

std::vector< std::string > LCS::ReadAllLCSRecurse( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second, const int _index_first, const int _index_second )
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
}*/