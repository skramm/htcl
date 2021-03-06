/*
	This file is part of htcl,
	a command-line utility that generates html cloud tag with links on files, from a tag file

    Copyright (C) 2017 Sebastien Kramm

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#define BUF_SIZE 256

char g_path_sep = '/';
char g_field_sep = ';';
char g_tag_sep = ' ';
std::string g_common_path;

std::string fname_cloud( "htcl_cloud.html" );
std::string fname_links( "htcl_links.html" );

//------------------------------------------------------------------
struct File
{
	std::string _path;
	std::string _fname;
};
//------------------------------------------------------------------
struct Tag
{
	Tag( std::string n, File f ): _tag_name(n)
	{
		_vfiles.push_back( f );
	}
	std::string       _tag_name;   ///< tag name
	std::vector<File> _vfiles; ///< set of files associated with this tag
};
//------------------------------------------------------------------
/// Tokenizing string into vector of strings,
/// using \c delim as delimitor (needs BOOST)
std::vector<std::string>
Tokenize(
	const std::string&        input,           ///< The input string
	char                      delim,           ///< the delimiter character
	size_t                    min_nb_tokens )  ///< if not 0, output vector will be filled with empty fields until nb of fields reaches this value
{
	std::vector<std::string> v_output;
	std::string d;
	d += delim;
	boost::char_separator<char> sep( d.c_str(), "", boost::keep_empty_tokens );
	boost::tokenizer< boost::char_separator<char> > tokens( input, sep );

	BOOST_FOREACH(std::string t, tokens)
	{
		v_output.push_back( t );
	}
	if( min_nb_tokens )
		while( v_output.size() < min_nb_tokens )  // if line has not enough tokens, fill with empty strings
		{
			v_output.push_back( "" );
		}
	return v_output;
}

//------------------------------------------------------------------
/// Process one line of the input file: read file and path and list of associated tags
void
ProcessLine( const std::string& line, std::vector<Tag>& v_output )
{
	if( line.size() == 0 )
		return;

	if( line.at(0) == '#' ) // if line is a comment
		return;

	auto v_tokens = Tokenize( line, g_field_sep, 3 ); // tokenize the whole line and separate into 3 strings

	if( v_tokens.size() != 3 )
	{
		std::cerr << "error: line has not 3 tokens:\n*" << line << "*\n";
		throw "error: not enough tokens";
	}

	auto f_path = v_tokens[0];
	auto f_name = v_tokens[1];
	File fil{ f_path, f_name };

	auto v_tags_str = Tokenize( v_tokens[2], g_tag_sep, 0 ); // tokenize the space-separated tags
	for( const auto& tag_str: v_tags_str )
	{
		auto it = std::find_if(
			std::begin(v_output),
			std::end(v_output),
			[&tag_str]( const Tag& t ){ return t._tag_name == tag_str; } // lambda
		);
		if( it == std::end(v_output) )
			v_output.push_back( Tag( tag_str, fil ) );  // not found: new tag
		else
			it->_vfiles.push_back( fil );               // found; update tag by adding file
	}
}
//------------------------------------------------------------------
std::vector<Tag>
ReadInputFile( std::string fn )
{
	std::ifstream datafile;
	datafile.open( fn, std::ios_base::in );
	if( !datafile.is_open() )
	{
		std::cerr << " -unable to open file " << fn << '\n';
		throw "unable to open file";
	}
	std::vector<Tag> v_tags;
	char buf[BUF_SIZE];
	bool go_on(true);
	do
	{
//		linecount++;
		datafile.getline( buf, BUF_SIZE );
		std::string line(buf);
		if( datafile.eof() )
		{
//			std::cerr << " - Reached EOF in file\n";
			go_on = false;
		}
		else
		{
			if( !datafile.good() )
			{
				std::cerr << " - stop: bad datafile, failure\n";
				go_on = false;
			}
			else
			{
				ProcessLine( line, v_tags );
			}
		}
	}
	while( go_on );
//	CERR << " - success\n";
	return v_tags;
}
//------------------------------------------------------------------
void
GenerateTagCloud( const std::vector<Tag>& v_tags )
{
	std::ofstream f;
	f.open( fname_cloud );
	if( !f.is_open() )
	{
		std::cerr << "Error: unable to open file " << fname_cloud << '\n';
		throw "unable to open file";
	}

	f << "<div id='tag_cloud'>\n";
	for( const auto tag: v_tags )
	{
		f << "<a href='#t_" << tag._tag_name << "'>" << tag._tag_name << "</a>\n";
	}
	f << "</div>\n";
}
//------------------------------------------------------------------
void
GenerateTagLinks( const std::vector<Tag>& v_tags )
{
	std::ofstream f;
	f.open( fname_links );
	if( !f.is_open() )
	{
		std::cerr << "Error: unable to open file " << fname_links << '\n';
		throw "unable to open file";
	}

	f << "<div id='tag_links'>\n";
	f << "<ul>\n";
	for( const auto& tag: v_tags )
	{
		f << "<li id='t_" << tag._tag_name << "'>\n";
		f << tag._tag_name << ":\n";
		for( const auto& file: tag._vfiles )
		{
			f << "<a href='";
			if( !g_common_path.empty() )
				f << g_common_path << g_path_sep;
			f << file._path << g_path_sep
			  << file._fname
			  << "'>" << file._fname << "</a>\n";
		}
		f << "</li>\n";
	}
	f << "</ul></div>\n";
}
//------------------------------------------------------------------
void
Usage()
{
	std::cout << "htcl: Html Tag CLoud generator\n";
	std::cout << "usage: htcl <inputfile> [] [common_path]\n";
}
//------------------------------------------------------------------
int main( int argc, const char** argv )
{
	if( argc < 2 )
	{
		Usage();
		return 1;
	}
	if( argc > 2 )
	{
		assert( std::string(argv[2]).size() == 1 );
		g_field_sep = argv[2][0];
	}

	if( argc > 3 )
		g_common_path = argv[3];

	// 1 - read input file and build array of tags
	auto v_tags = ReadInputFile( argv[1] );

	std::sort(
		std::begin(v_tags),
		std::end(v_tags),
		[](const Tag& t1, const Tag& t2) { return t1._tag_name<t2._tag_name; }
	);

	GenerateTagCloud( v_tags );
	GenerateTagLinks( v_tags );
}
//------------------------------------------------------------------
// (eof)
