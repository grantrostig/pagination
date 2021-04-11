/* pagination
 * to allow a user to paginate console output, similar to how Unix "more" does it.
 * author Grant Rostig
 * under the: un-license */
#include <iostream>
#include <vector>
#include <variant>
#include <cassert>
using 				std::cout, std::cin, std::cerr;
using Character 	= char;				// should be internationalized
using Word 			= std::string;  	// = Character, need struct with: "is_hypthenateable"
using Punctuation	= Character;
constexpr Punctuation hyphen 	= '-'; 	// should be internationalized
constexpr Punctuation separator = ' '; 	// just a space, but maybe needed??

class ScreenSize {
public:
    int chars {80};
    int lines {24};
};

class PaginationSize {
public:
    int chars {};
    int lines {};
};

class Phrase {
public:
    std::vector<std::variant< std::monostate, Word, Punctuation >> content {}; // todo: do I really need std::monostate?
    PaginationSize size		{0,0};
};

class OutPhrase {
public:
    Phrase content_desired 	{};
    //Phrase content_min 		{};		// the least we can work with in bad situations.
};

class PaginIOPhrase {
public:
    Phrase content_desired 	{};		// Content of what we say to the user, as we notify them of more output coming from their prior request.
    Phrase content_min 		{};		// the least we can work with in bad situations.
    Phrase prompt_desired 	{};		// prompt is like what we seen at bash shell, like: "$ X", where X is the blinking cursor.
    Phrase prompt_min 		{}; 	// probably not needed.
};

class DisplayInfoUnit { // std::assert( out_phrase.size() > 0 );
public:
   OutPhrase 		out_phrase 		{};  // An idea we want to print to console.
   PaginIOPhrase 	pagin_IO_phrase	{};  // The text that would be displayed if user was prompted for pagination.
   bool 			is_seen 		{false};
   bool 			is_acknowledged {false};
   bool 			is_printed 		{false};  // printed to console, not paper.
};

class ComputerDisplay {
public:
    PaginationSize capactity 	{};
    PaginationSize current  	{0,0};  // space currently used on the user visible part of the console. this might be several touples using desired or min content sizes.
};

int main() {
    ComputerDisplay display {};
    OutPhrase out_1 	{{{"Your bank balance is: $50."}, 	{0,1} }};

    OutPhrase prompt_1 	{{{"Press Enter to Continue"}, 		{30,1} }};
    //OutPhrase prompt_1a {{{"Press Enter to Continue"}, 		{30,1} }};
    OutPhrase prompt_2 	{{{"more..."}, 						{7,0} }};
    OutPhrase prompt_3 	{{{"$$$  "}, 						{5,0} }};
    OutPhrase prompt_4 	{{{"$  "}, 						    {3,0} }};
    DisplayInfoUnit output_unit {
        out_1
            //prompt_1
        //{{prompt_2}},
         //prompt_3
    };
    cout << "###\n";
}
