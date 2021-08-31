#ifndef PCOUT_H
#define PCOUT_H
#include <iostream>
#include <locale>
#include <cstdio>
#include <streambuf>
#include <iosfwd>
#include <fstream>
//#include <xutility>
// for write():
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

using 			CHAR_T = char;
constexpr int 	BUFFER_SIZE 	{80*24};

class Pcout_streambuf : public std::streambuf {
protected: // const size_t 	size 		{ 80*40 }; // static constexpr size_t size2 { 80*40 };  todo: why static required?
    std::array<CHAR_T, BUFFER_SIZE>  	buffer 		{}; // todo: std::string buffer (size); // std::string  	buffer 		{}; // todo: std::string buffer (size);
public:
    Pcout_streambuf() {
        setp( buffer.begin(), buffer.end() );  //  todo: does begin() give address of the char?  can it be relocated? no?,
    }
    ~Pcout_streambuf() {
        sync();
    }
protected:
    virtual int sync() override {
        if ( flush_buffer() == traits_type::eof() ) return -1;
        return 0;
    }
    virtual int_type overflow( int_type __c = traits_type::eof() ) override {
        if ( __c != traits_type::eof() ) {
            * pptr() = __c;  // todo: why int and not char?
            pbump( 1 );  // todo: magic number!
        }
        if ( flush_buffer() == traits_type::eof() ) return traits_type::eof(); // todo:??
        system("pause"); // todo: redo this.
        return __c;
    }
    int flush_buffer() {
        long buffer_used_size 	{ pptr() - pbase() };
        int file_descriptor 	{ STDOUT_FILENO	};
        if ( write( file_descriptor, buffer.begin() , buffer_used_size )) return EOF;
        pbump( -buffer_used_size );
        return ( buffer_used_size );
    }

};

static Pcout_streambuf pcout_streambuf;

static std::ostream pcout (&pcout_streambuf);

/* class Pcout {
public:
    Pcout();
};
*/

#endif // PCOUT_H
