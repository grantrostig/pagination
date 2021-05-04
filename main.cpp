/* pagination
 * to allow the programmer to paginate console output, similar to how Unix "cat /etc/passwd | more" does it.
 * author Grant Rostig
 * under the: un-license
 */
// #include "main.h"
#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <queue>
#include <variant>
#include <cassert>
#include <sstream>
#include <ostream>
#include <algorithm>
#include <cstdlib>
using std::cout, std::cin, std::cerr;
using Character 				= char;			// should be internationalized
using Word 						= std::string;  // = Character, need struct with: "is_hypthenateable" or { int offset_of_hyphen_insertion_for_print //if > 0 }
using Punctuation				= Character;
using WrappingPosition 			= bool;			// could be any type, I just need a type to be present or not be.  This is added to a Phrase Content Component to denote wrapping.
using InteractionResult 		= int;			// user intention
using PhraseContentComponent 	= std::string; //using PhraseContentComponent =  std::variant< std::monostate, Word, Punctuation, WrappingPosition >; // todo: do I really need std::monostate?
constexpr Punctuation hyphen 	= '-'; 	// should be internationalized
constexpr Punctuation separator = ' '; 	// just a space, but maybe needed??
using DisplayRawUnit		    = std::string;  	/// exactly the characters printed to the display within one screen-full or lesser. This object is what the paginator derived from a DisplayInfoUnit.  NOTE this does not support the user changing the display geometry.
using PaginatorSessionHistory   = std::vector< DisplayRawUnit >;

class ScreenSize {		/// may change between invokations of paginator within a session.
public:
    size_t num_chars {80};		/// number of printable fixed size characters that fit on one horizontal line.
    size_t num_lines {24};		/// number of printable fixed size text lines that fit on the vertical dimension of a computer screen.
};

class PaginationDimension {  		/// characteristics of Phrase to be printed.  todo: Should this be a separate class, or a "using =" of Screen Size, since it has same data members? Also possible I might need different classs members.
public:
    size_t num_chars {};		/// total number of chars in message.  todo: Need to handle multibyte characters.
    size_t lines_min {};		/// programmer must define the minium number of lines the output should take. Generally only one.  Of course it might take more if the output wraps around based on screen x dimension.
};

class Phrase {					/// a series of words, ie. a sentence, or a paragraph??, or a document??
public:
    PhraseContentComponent	content 	{}; //std::vector< PhraseContentComponent > content {};
    PaginationDimension 	dimension	{0,1};
};

class OutPhrase {				/// a "Phrase" string to be printed to the console for the user to read.
public:
    Phrase 	content_desired {};		// Content of what we say to the user, as we notify them of more output coming from their prior request.
    Phrase 	content_min 	{};		// the least we can work with in bad situations., as an alternative to the above.  Imagine output to a tiny display screen.
};

class PaginIOPhrase {			/// Pagination prompting details that may be needed/used depending on the room on the console.
public:
    std::string 	prompt_desired 	{};		// prompt is like what we seen at bash shell, like: "$ X", where X is the blinking cursor.
    std::string 	prompt_min 		{}; 	// probably not needed.
};

class DisplayInfoUnit { 		/// represents a packaged OutPhrase for use by the paginator.
    // std::assert( out_phrase.size() > 0 );
public:
    OutPhrase 		out_phrase 		{};  /// An text of idea we want to print to console.
    PaginIOPhrase 	prompt			{};  /// The text that would be displayed if user was prompted for pagination.
};

class ComputerDisplay {
public:
    ScreenSize 	capactity 			{};
    ScreenSize 	currently_used  	{0,0};  /// space currently used on the user visible part of the console. this might be several touples using desired or min content sizes.
};

class Paginator {
public:
    static ComputerDisplay 						display;
    //static bool									is_new_session;  // todo: perhaps history.empty() can handle this?
    static PaginatorSessionHistory 				history;			// Only holds current session.
    static PaginatorSessionHistory::iterator	history_itr;  		// Maybe this should be an index into the container if it is random-access.
    char									    user_input {};		// What the user replies to a pagination prompt.
    DisplayRawUnit								display_raw_unit {};
    void reset_display_counts() { 				/// ready to paginate one page, either the first, or subsequent in a session.
        display = {}; 							// todo: does this load capacity numbers?
    };
    /// user has seen all the output from a loop and will no longer be able to ask for history.  All output on screen is considered read by user.
    void reset_session() {
        history.clear();
        history_itr = history.end();		// todo: is there a better way to show the itr has been invalidated?
        reset_display_counts();
    };
    InteractionResult prompt_user() {
        cout<< "::Paginator says ENTER to continue::";
        //cin >> user_input;
        cin.get(user_input);
        reset_display_counts();
        return 0;  // user input result
    };

    using ProcessUnitResult = struct { DisplayRawUnit unit; size_t lines; size_t size_category; }; // todo:?? Is there a better way to handle function return values? Can't seem to do it on the same line, without std::pair? NOTE: I'm also assuming that I will used structured decomposition on invocation. This is probably worse: struct ProcessUnitResult { DisplayRawUnit unit; size_t lines; size_t size_category; };
    /// determines which content to use depending on capacity of the screen. First cut down content, then cut down prompt lengths. Need to fit at least one of each.
    ProcessUnitResult format_unit( DisplayInfoUnit const & diu ) {
        auto content_desired = diu.out_phrase.content_desired;
        auto content_min 	 = diu.out_phrase.content_min;
        auto prompt_desired  = diu.prompt.prompt_desired;
        auto prompt_min  	 = diu.prompt.prompt_min;

        /* PLEASE IGNORE commented code, it requires re-work.
         * currently I just return a MAGIC number for sizing.
         * todo: I know below is ugly, any ideas on how to make more attractive?
         * calculations need to be redone with lines_ceil
        std::div_t p = std::div( 90, 24);
        int lines_ceil = p.quot + ( p.rem == 0 ? 0 : 1);

        assert( content_min.dimension.lines_min <= display.capactity.num_lines );
        assert( content_min.content.size() + prompt_min.size() < display.capactity.num_chars );

        if ( content_desired.content.size() + prompt_desired.size() <= display.capactity.num_chars )
             //return  { content_desired.content + prompt_desired, content_desired.dimension.lines_min, 0};  // todo:?? How can I avoid a copy of these std::strings?  I don't want char *.
             return  { content_desired.content, content_desired.dimension.lines_min, 0};  // todo:?? How can I avoid a copy of these std::strings?  I don't want char *.
        if ( content_desired.content.size() + prompt_min.size()     <= display.capactity.num_chars )
             //return  { content_desired.content + prompt_desired, content_min.dimension.lines_min,     1};
             return  { content_desired.content, content_min.dimension.lines_min,     1};
        if ( content_min.content.size()     + prompt_desired.size() <= display.capactity.num_chars )
             //return  { content_min.content     + prompt_desired, content_min.dimension.lines_min,     2};
             return  { content_min.content    , content_min.dimension.lines_min,     2};
        //return  	 { content_min.content     + prompt_min,     content_min.dimension.lines_min,     3};
        return  	 { content_min.content  + prompt_min,     content_min.dimension.lines_min,     3};
        */
        return  	 { content_desired.content, content_desired.dimension.lines_min, 3};
    }
    InteractionResult output_display_info_unit ( DisplayInfoUnit display_info_unit ) {
        if ( history.empty() )
            reset_display_counts();
        auto [ unit, lines_min, size_category ] = format_unit( display_info_unit );
        InteractionResult interaction_result {0};
        if ( lines_min   > display.capactity.num_lines - display.currently_used.num_lines ||
             unit.size() > display.capactity.num_chars - display.currently_used.num_chars         )
                interaction_result = prompt_user();
        // act on result, which might include going back to prior page recursively!!
        if ( interaction_result == 1 )
                while ( output_prior_unit() ) {};
        history.push_back( unit );
        history_itr = history.end();
        cout << unit; // todo: << std::endl;
        display.currently_used.num_chars += 1;  // todo: magic number
        display.currently_used.num_lines += 1;  // todo: magic number
        return interaction_result;
    };
    InteractionResult output_prior_unit() {
        // we want to mutate the itr. before, since it is pointing beyond the one we want, similar to end() initially.
        //assert ( ("ERROR:bad iterator", history_itr >= history.begin() && history_itr <= history.end()) );
        if ( ! history.empty() && ( history_itr > history.begin() && history_itr <= history.end() )) {  // todo:?? can we do less_than on vector iterators?? Yes, since we assume it is a random access iterator??
            DisplayRawUnit unit = history.empty() ? *history_itr : *--history_itr;
            cout << unit;  // << std::endl; todo:
            return prompt_user();
        }
        else cout << "Sorry, no prior page!" << std::endl;
        return 0;
    }
};

ComputerDisplay 					Paginator::display 		{};  //  this is a DEFINITION, required here for a static in a class only?
//bool 								Paginator::is_new_session {true};
PaginatorSessionHistory 			Paginator::history 		{};
PaginatorSessionHistory::iterator	Paginator::history_itr 	{Paginator::history.end()};

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
    OutPhrase 		out_1 	{ {"Your bank balance is: $50."} };
    PaginIOPhrase 	pagin_1 {
        {"IOPhrase::Press [ENTER] to continue:"},
        {"IOPhrase::more..."},
        //{"IOPhrase::>$>  "},
        //{"IOPhrase::$  "}
    };
    DisplayInfoUnit 	display_info_unit 	{ out_1, pagin_1 };

    InteractionResult result;
    for ( int i = 0; i < 30; ++i ) {
        std::ostringstream oss {};
        oss << "my text, followed by my number: " << i << std::endl;
        display_info_unit.out_phrase.content_desired.content = oss.str();
        /*
        display_info_unit.out_phrase.content_desired.content = ( oss << "my text, followed by my number: " << 42.0 << std::endl).rdbuf() ;
        display_info_unit.out_phrase.content_desired.content = ( oss << "my text, followed by my number: " << 42.0 << std::endl ).str().c_str();
         todo:?? above line error: ‘std::basic_ostream<char>::__ostream_type’ {aka ‘class std::basic_ostream<char>’} has no member named ‘str’

        pcout 			<< clear_screen();  // OR pcout.clear_screen();
        pcout 			<< "my text, followed by my number: " << 42.0 << std::endl;
        switch (pcout.result)  // OR // next line
        */
        result = paginator.output_display_info_unit( display_info_unit );
    }
    switch (result ) {
    case 0: cout << "ok\n"; break;
    case 1: cout << "stop_current_operation by user request\n"; break;
    case 2: cout << "quit_program_immediately by user request\n"; break;
    }

    /* playing with invalid iterators
    std::vector<int> vec {0,1,2,3};
    //std::list<int> vec {0,1,2,3};
    //std::deque<int> vec {0,1,2,3};
    std::vector<int>::iterator itr {vec.end()};
    //std::list<int>::iterator itr {vec.end()};
    //std::deque<int>::iterator itr {vec.end()};
    if ( itr > vec.begin() && itr <= vec.end() ) {
        cout << "OK\n";
    }  // todo:?? can we do less_than on vector iterators??
    else { cout << "BAD\n"; }  // todo:?? can we do less_than on vector iterators??
    //cout << *itr <<std::endl;
    cout << *prev(itr) <<std::endl;
    cout << *prev(itr) <<std::endl;
    cout << *(--itr) <<std::endl;
    cout << *(--itr) <<std::endl;
    cout << *(--itr) <<std::endl;
    advance(itr,1);
    cout << *itr <<std::endl;
    advance(itr,1);
    cout << *itr <<std::endl;
    */

    cout << "###\n";
}
