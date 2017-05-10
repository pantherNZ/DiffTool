#include <iostream>
#include <fstream>
#include <filesystem>

#include "LCS.h"

int Pause( const int return_code )
{
	int temp;
	std::cin >> temp;
	return return_code;
}

int main( const int argc, const char* argv[] )
{
	if( argc <= 1 )
	{
		std::cout << "Please enter the path to the Stats.xml file in your SVN as an argument to this program and try again.." << std::endl;	
		return Pause( 1 );
	}

	std::string stats_file_path( argv[argc - 1] );
	const std::string new_file_name = "Stats.xml";
	const std::string old_file_name = "Stats_old.xml";

	if( stats_file_path.substr( stats_file_path.length() - new_file_name.length( ), new_file_name.length( ) ) != new_file_name )
		stats_file_path += "\\" + new_file_name;

	std::ifstream new_stats_input;
	new_stats_input.open( stats_file_path );

	if( new_stats_input.fail( ) )
	{
		std::cout << "Argument for Stats.xml file is invalid!" << std::endl;
		std::cout << "Please enter the path to the Stats.xml file in your SVN as an argument to this program and try again.." << std::endl;
		return Pause( 1 );
	}
		
	std::ifstream old_stats_input;
	old_stats_input.open( "Stats_old.xml" );

	if( old_stats_input.fail( ) )
	{
		std::cout << "No previous stats file found for comparison, creating \"Stats_old.xml\".." << std::endl;
		const std::string create_file_path = std::experimental::filesystem::current_path( ).string( ) + "\\" + old_file_name;
		std::experimental::filesystem::copy_file( stats_file_path, create_file_path );
		return Pause( 0 );
	}

	// Stream files to data container
	std::vector< std::string > data_old;
	std::vector< std::string > data_new;
	data_old.reserve( 35000 );
	data_new.reserve( 35000 );
	{
		std::string temp_input;

		while( std::getline( old_stats_input, temp_input ) )
			data_old.push_back( temp_input );

		while( std::getline( new_stats_input, temp_input ) )
			data_new.push_back( temp_input );
	}

	LCS lcs( data_old, data_new );

	//std::string data_old( ( std::istreambuf_iterator<char>( old_stats_input ) ), std::istreambuf_iterator<char>( ) );
	//std::string data_new( ( std::istreambuf_iterator<char>( new_stats_input ) ), std::istreambuf_iterator<char>( ) );

	// Compute the LCS between the two inputs
	//const std::string str1( "main sentence hahaha" );
	//const std::string str2( "ain sentence Yhahaha" );
	//const std::string str1( "AGCAT" );
	//const std::string str2( "GAC" );
	//const std::string str1( "XMJYAUZ" );
	//const std::string str2( "MJAU" );	
	//LCS lcs( str1, str2 );

	//const auto result = lcs.ReadFirstLCS( data_old, data_new );
	//std::cout << "Longest Common Subsequence: " << result << std::endl << std::endl;

	//const auto results = lcs.ReadAllLCS( data_old, data_new );
	//std::cout << "Longest Common Subsequence: " << std::endl;
	//for( auto& s : results )
	//	std::cout << s << std::endl;
	//std::cout << std::endl;

	std::cout << "File diff" << std::endl;
	std::cout << "-----------------------" << std::endl << std::endl;
	lcs.PrintDiff( data_old, data_new );
	std::cout << std::endl << std::endl << "-----------------------" << std::endl;

	return Pause( 0 );
}