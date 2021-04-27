/* pagination
 * to allow a user to paginate console output, similar to how Unix "more" does it.
 * author Grant Rostig
 * under the: un-license
 */
#include <iostream>
#include <vector>
#include <queue>
#include <variant>
#include <cassert>
#include <sstream>
#include <ostream>
#include <algorithm>
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
    std::string 	content_desired {};		// Content of what we say to the user, as we notify them of more output coming from their prior request.
    std::string 	content_min 	{};		// the least we can work with in bad situations.
    //Phrase 		content_desired {};
    //Phrase 		content_min 	{};		// the least we can work with in bad situations.
    bool 			is_seen 		{false};  // has been seen by user on console. 			assert(is_printed == true && is_acknowledged == true);
    bool 			is_acknowledged {false};  // has been printed to console, not paper. 	assert(is_printed == true);
    bool 			is_printed 		{false};  // has been printed to console, not paper.
};

class PaginIOPhrase {			// Pagination prompting details that may be needed/used depending on the room on the console.
public:
    std::string 	content_desired {};		// Content of what we say to the user, as we notify them of more output coming from their prior request.
    std::string 	content_min 	{};		// the least we can work with in bad situations.
    std::string 	prompt_desired 	{};		// prompt is like what we seen at bash shell, like: "$ X", where X is the blinking cursor.
    std::string 	prompt_min 		{}; 	// probably not needed.
    //Phrase 		content_desired {};		// Content of what we say to the user, as we notify them of more output coming from their prior request.
    //Phrase	 	content_min 	{};		// the least we can work with in bad situations.
    //Phrase 		prompt_desired 	{};		// prompt is like what we seen at bash shell, like: "$ X", where X is the blinking cursor.
    //Phrase 		prompt_min 		{}; 	// probably not needed.
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

using PaginatorHistory = std::deque< DisplayInfoUnit >;
// Global utility class to control output to the user console and prompt user when screen is full and more data is to be displayed.  Could become a singleton later.
class Paginator {
public:
    static ComputerDisplay 		display;
    static PaginatorHistory 	history;
    std::string					user_input {};
    void 	reset_display() {
        display = {}; 			// todo: does this load capacity numbers?
    };
    void 	prompt_user() {
        cout<< " CR to continue:";
        cin >> user_input;
        reset_display();
    };
    bool 	check_prompt( DisplayInfoUnit display_info ) {
        // in fact instead of 2000 we would search history to calculate space available on screen by looking at history of un-acknowledged current content on screen in this printing transaction.
        if (  display_info.out_phrase.content_desired.length()
              + display_info.pagin_IO_phrase.content_desired.length() > 2000 ) {
            prompt_user();
            return true;
        }
        return false;
    };
    void process_display_usage( DisplayInfoUnit display_info ) {
        bool is_prompted = check_prompt( display_info );
        if ( is_prompted ) {
            display_info.out_phrase.is_acknowledged = true;
            display_info.out_phrase.is_printed = true;
            display_info.out_phrase.is_seen = true;
            struct is_ACKed {
                bool operator()( DisplayInfoUnit const & u) {
                    return u.out_phrase.is_acknowledged;
                };
            };
            struct mark_ACKed {
                DisplayInfoUnit operator()( DisplayInfoUnit & u) {
                    u.out_phrase.is_acknowledged = true;
                    return u;
                };
            };
            std::transform( history.begin(), std::find_if(history.begin(), history.end(), is_ACKed()),
                            history.begin(), mark_ACKed() );
        }
        history.push_front( display_info );
    };
    void 	display_print( DisplayInfoUnit display_info ) {
        process_display_usage( display_info );
        auto v = display_info.out_phrase.content_desired;
        std::cout << v;
    }
};
ComputerDisplay 	Paginator::display { {0,0}, {} };  //  this is a DEFINITION. todo: What does this line do? In globla space?  In main()?
PaginatorHistory 	Paginator::history {};

int main() {
    Paginator 		paginator {};
    /*
    ComputerDisplay Paginator::display {};  // todo: What does this line do? In globla space?  In main()?
    Paginator 		paginator 		{};

    OutPhrase 		out_1 		{{{"Your bank balance is: $50."}, {0,1} }};
    PaginIOPhrase 	pagin_1 	{
        {{"Press [ENTER] to continue"}, {30,1}},
        {{"more..."}, 					{7,0}},
        {{"$$$  "}, 					{5,0}},
        {{"$  "}, 					    {3,0}}
    };
    */
    OutPhrase 		out_1 			{"Your bank balance is: $50."};
    PaginIOPhrase 	pagin_1 		{{"Press [ENTER] to continue"},
        {"more..."},
        {">$>  "},
        {"$  "}
    };
    DisplayInfoUnit 	output_unit 	{ out_1, pagin_1 };
    std::ostringstream 	oss 			{};
    oss << "my text, followed by my number: " << 42.0 << std::endl;
    output_unit.out_phrase.content_desired = (oss).str().c_str();
    //output_unit.out_phrase.content_desired = ( oss << "my text, followed by my number: " << 42.0 << std::endl ).str().c_str();
    // todo:?? above line error: ‘std::basic_ostream<char>::__ostream_type’ {aka ‘class std::basic_ostream<char>’} has no member named ‘str’

    paginator.display_print( output_unit );
    cout << "###\n";
}
