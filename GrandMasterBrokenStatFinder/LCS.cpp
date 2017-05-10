#include "LCS.h"

#include <algorithm>
#include <iostream>
#include <functional>
#include <cctype>

// Compute Longest Common Subsequence 
// Algorithm as per https://en.wikipedia.org/wiki/Longest_common_subsequence_problem

namespace
{
	// Trim left
	inline std::string& LTrim( std::string& s, const char* t = " \t\n\r\f\v" )
	{
		s.erase( 0, s.find_first_not_of( t ) );
		return s;
	}

	// Trim right
	inline std::string& RTrim( std::string& s, const char* t = " \t\n\r\f\v" )
	{
		s.erase( s.find_last_not_of( t ) + 1 );
		return s;
	}

	// Trim
	inline std::string& Trim( std::string& s, const char* t = " \t\n\r\f\v" )
	{
		return LTrim( RTrim( s, t ), t );
	}

	const std::string virtual_prefix( "<Cell column=\"is virtual\" value=\"True\"/>" );
	const std::string id_prefix( "<Cell column=\"Id\" value=\"" );
};

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

void LCS::PrintFileWithDiff( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second )
{
	// Escape if the same
	if( m_lcs_storage.size( ) < 2 && m_lcs_storage[0].size( ) < 2 )
	{
		std::cout << "No differences found";
		return;
	}

	// Print skipped over lines that weren't changed at the start & end of the file
	SetConsoleTextAttribute( m_hConsole, 7 );

	for( size_t i = _data_first.size( ) - 1; i >= m_index_end_first; --i )
		std::cout << _data_first[i].c_str( ) << std::endl;

	Diff( _data_first, _data_second, m_index_end_first, m_index_end_second, true );
	SetConsoleTextAttribute( m_hConsole, 7 );

	for( int i = ( int )m_index_start - 1; i >= 0; --i )
		std::cout << _data_first[i].c_str( ) << std::endl;
}

void LCS::Diff( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second, const bool _print_diff )
{
	// Escape if the same
	if( m_lcs_storage.size( ) < 2 && m_lcs_storage[0].size( ) < 2 )
	{
		std::cout << "No differences found";
		return;
	}

	// Print skipped over lines that weren't changed at the start & end of the file
	SetConsoleTextAttribute( m_hConsole, 7 );
	Diff( _data_first, _data_second, m_index_end_first, m_index_end_second, _print_diff );
	SetConsoleTextAttribute( m_hConsole, 7 );
}

void LCS::PrintBrokenGrandmasterStats( const std::vector<std::string>& _data_first, const std::vector<std::string>& _data_second )
{
	std::cout << "Stats with changed id:" << std::endl;
	int difference_offset = 0;

	for( auto iter = m_differences.begin( ); iter != m_differences.end( ); ++iter )
	{
		auto item = *iter;

		if( !item.second )
		{
			difference_offset += ( int )item.first.second;
			const auto regular_index = item.first.first;
			const auto adjusted_index = regular_index + difference_offset - ( int )item.first.second;
			const auto matching_change = m_differences.find( std::make_pair( adjusted_index, DifferenceType( item.first.second * -1 ) ) );

			// If the counter is 0 we know that we have a change from both the old and the new (meaning it was a change & not just an add or removal)
			if( matching_change != m_differences.end( ) && ( int )item.first.second + matching_change->first.second == 0 )
			{
				iter->second = true;
				matching_change->second = true;
				difference_offset -= ( int )item.first.second;
				std::string stat_name_first;
				std::string stat_name_second;

				if( ExtractStatId( _data_first[regular_index], stat_name_first ) &&
					ExtractStatId( _data_second[adjusted_index], stat_name_second ) )
				{
					SetConsoleTextAttribute( m_hConsole, 12 );
					std::cout << stat_name_first.c_str( );
					SetConsoleTextAttribute( m_hConsole, 7 );
					std::cout << "   ->   ";
					SetConsoleTextAttribute( m_hConsole, 10 );
					std::cout << stat_name_second.c_str( ) << std::endl;
				}
			}
		}
	}

	SetConsoleTextAttribute( m_hConsole, 7 );
	std::cout << std::endl << "Stats that have become virtual:" << std::endl;

	// Stat has become virtual now
	for( const auto& item : m_differences )
	{
		// 1 means that this difference was added in the new file, we check to see if this line is the "is virtual" line
		if( item.first.second == ADDED )
		{
			std::string line( _data_second[item.first.first] );
			Trim( line );
	
			if( line == virtual_prefix )
			{
				std::string stat_name;
				int iIndex = ( int )item.first.first - 1;
	
				// This is a virtual line, let's now check incrementally upwards until we find the stat
				while( iIndex > 0 )
				{
					std::string current_row( _data_first[iIndex] );
					Trim( current_row );
	
					// Extract the stat (returns true if this is a stat line)
					if( ExtractStatId( current_row, stat_name ) )
					{
						// No difference means this stat has become virtual
						if( m_differences.find( std::make_pair( iIndex, ADDED ) ) == m_differences.end( ) &&
							m_differences.find( std::make_pair( iIndex, REMOVED ) ) == m_differences.end( ) )
						{
							SetConsoleTextAttribute( m_hConsole, 10 );
							std::cout << stat_name.c_str( ) << std::endl;
						}
	
						break;
					}

					--iIndex;
				} 
			}
		}
	}

	SetConsoleTextAttribute( m_hConsole, 7 );
}

const bool LCS::ExtractStatId( const std::string& _line, std::string& _stat_out )
{
	std::string stat_line( _line );
	Trim( stat_line );
	
	if( stat_line.substr( 0, id_prefix.length( ) ) == id_prefix )
	{
		_stat_out = stat_line.substr( id_prefix.length( ), stat_line.length( ) - id_prefix.length( ) - 3 );
		return true;
	}

	return false;
}

void LCS::Diff( const std::vector< std::string >& _data_first, const std::vector< std::string >& _data_second, const size_t _index_first, const size_t _index_second, const bool _print_diff )
{
	size_t index_first = _index_first;
	size_t index_second = _index_second;

	while( true )
	{
		if( index_first > m_index_start && index_second > m_index_start && m_hashed_data_first[index_first - 1] == m_hashed_data_second[index_second - 1] )
		{
			SetConsoleTextAttribute( m_hConsole, 7 );
			if( _print_diff )
				std::cout << _data_first[index_first - 1].c_str( ) << std::endl;
			index_first--;
			index_second--;
		}
		else
		{
			const auto cell_left = ( index_first > m_index_start ? m_lcs_storage[index_first - 1 - m_index_start][index_second - m_index_start] : 0 );
			const auto cell_up = ( index_second > m_index_start ? m_lcs_storage[index_first - m_index_start][index_second - 1 - m_index_start] : 0 );
			bool should_break = true;

			if( index_second > m_index_start && ( index_first <= m_index_start || cell_up >= cell_left ) )
			{
				SetConsoleTextAttribute( m_hConsole, 10 );
				if( _print_diff )
					std::cout << _data_second[index_second - 1].c_str( ) << std::endl;
				SaveDifference( index_second - 1, ADDED );
				index_second--;
				should_break = false;
			}
			if( index_first > m_index_start && ( index_second <= m_index_start || cell_up < cell_left ) )
			{
				SetConsoleTextAttribute( m_hConsole, 12 );
				if( _print_diff )
					std::cout << _data_first[index_first - 1].c_str( ) << std::endl;
				SaveDifference( index_first - 1, REMOVED );
				index_first--;
				should_break = false;
			}

			if( should_break )
				break;
		}
	}
}

void LCS::SaveDifference( const size_t index, const DifferenceType difference )
{
	m_differences.insert( std::make_pair( std::make_pair( index, difference ), false ) );
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