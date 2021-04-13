/* pagination
 * to allow a user to paginate console output, similar to how Unix "more" does it.
 * author Grant Rostig
 * under the: un-license */
#include <iostream>
#include <vector>
#include <variant>
#include <cassert>
#include <sstream>
#include <ostream>
using 				std::cout, std::cin, std::cerr;
using Character 		= char;				// should be internationalized
using Word 				= std::string;  	// = Character, need struct with: "is_hypthenateable" or { int offset_of_hyphen_insertion_for_print //if > 0 }
using Punctuation		= Character;
using WrappingPosition 	= bool;			// could be any type, I just need a type to be present or not be.  This is added to a Phrase Content Component to denote wrapping.
using PhraseContentComponent =  std::variant< std::monostate, Word, Punctuation, WrappingPosition >; // todo: do I really need std::monostate?
constexpr Punctuation hyphen 	= '-'; 	// should be internationalized
constexpr Punctuation separator = ' '; 	// just a space, but maybe needed??

class ScreenSize {
public:
    int chars {80};
    int lines {24};
};

class PaginationSize {  		// characteristics of Phrase to be printed.  todo: Should this be a separate class, or a using = of Screen Size, since it has same data members?
public:
    int chars {};
    int lines {};
};

class Phrase {					// a series of words, ie. a sentence
public:
    std::vector< PhraseContentComponent > 	content {};
    PaginationSize 							size	{0,0};
};

class OutPhrase {				// a Phrase to be printed to the console for the user to read.
public:
    std::string 	content_desired {};
    //Phrase 		content_desired {};
    //Phrase 		content_min 		{};		// the least we can work with in bad situations.
    bool 			is_seen 		{false};  // has been seen by user on console. 			assert(is_printed == true && is_acknowledged == true);
    bool 			is_acknowledged {false};  // has been printed to console, not paper. 	assert(is_printed == true);
    bool 			is_printed 		{false};  // has been printed to console, not paper.
};

class PaginIOPhrase {				// Pagination prompting details that may be needed/used depending on the room on the console.
public:
    Phrase 			content_desired {};		// Content of what we say to the user, as we notify them of more output coming from their prior request.
    Phrase 			content_min 	{};		// the least we can work with in bad situations.
    Phrase 			prompt_desired 	{};		// prompt is like what we seen at bash shell, like: "$ X", where X is the blinking cursor.
    Phrase 			prompt_min 		{}; 	// probably not needed.
};

class DisplayInfoUnit { // std::assert( out_phrase.size() > 0 ); // represents a packaged OutPhrase for use by the paginator.
public:
    OutPhrase 		out_phrase 		{};  // An idea we want to print to console.
    PaginIOPhrase 	pagin_IO_phrase	{};  // The text that would be displayed if user was prompted for pagination.
};

class ComputerDisplay {
public:
    ScreenSize 	capactity 	{};
    ScreenSize 	current  	{0,0};  // space currently used on the user visible part of the console. this might be several touples using desired or min content sizes.
};

void display_print( ComputerDisplay display, DisplayInfoUnit display_info ) {
   auto v = display_info.out_phrase.content_desired;
   std::cout << v;
}

int main() {
    //OutPhrase 		out_1 		{{{"Your bank balance is: $50."}, {0,1} }};
    OutPhrase 		out_1 		{"Your bank balance is: $50."};
    PaginIOPhrase 	pagin_1 	{
        {{"Press [ENTER] to continue"}, {30,1}},
        {{"more..."}, 					{7,0}},
        {{"$$$  "}, 					{5,0}},
        {{"$  "}, 					    {3,0}}
    };
    DisplayInfoUnit output_unit { out_1, pagin_1 };
    ComputerDisplay display 	{};

    std::ostringstream oss {};
    oss << "my text, followed by my number: " << 42.0 << std::endl;
    output_unit.out_phrase.content_desired = oss.str().c_str();

    display_print( display, output_unit );

    cout << "###\n";
}
