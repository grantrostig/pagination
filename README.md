# pagination via a paginator.

A modern c++ class to control output to the console so that data does not scroll past the user without user interaction.

Inspired by more command as in: cat /etc/passwd | more

Analysis:

Provide functionality to prompt the user then the output within a transaction exceeds the capacity of the screen of the computer terminal.

If more than one screen has been prompted, during that prompt, 
the user may reply to the prompt by pressing a key to display the prior screen full of output. The prior two resulting actions shall be handled by the paginator.

At the prompt, the user can also command various actions to be performed by the application program which has called the paginator.  These commands are expressed via return values, and or status fields/bitmaps persisted as part of the paginator, and accessible after its use.  The return codes might indicate the users desire for one of the following: 
1) Ask for help via perhaps the [F1] key, 
2) terminate the output transaction and return to the place in the program where it was commissioned, 
3) Terminate the entire application in with functionality to optionally save data, 
4) Terminate immediately without any prompts and not save the data (an emergency measure).  

Perhaps the help key functionality should be handled by the paginator?  Of note is that the help text itself may be larger than the screen and so also require pagination!

The functionality should be as simple to use as possible, and require as few lines of code per output operation as possible.  
Preferably the usage should be as similar as possible to std::cout.

A transaction consists of one or more line items.  A line item consists of a logical whole, such as a customer record, 
for example consisting of a customer number, name, address, and perhaps a long text field, such as a description.

It is preferable to have the user prompted between line items.  In the case where one line item, or even just one data field, 
exceeds the total capacity of the screen, a prompt would need to be issued.

There must be some way of designating the beginning, as well as the end of a transaction, to assist pagination.
It is likely an operation to separate line items, and between data items will also be required.  
These are termed: separator operators.  
These operators would also provide for "reset", "clear screen" and "clear to end of screen".

Design:

I) Simple Class: Seemingly the simplest approach is to create a single large complex class, as a global or a Singleton.
It would have a fully formatted string passed to it.  It would then paginate the output.  
This approach would likely require four statements to output one line item, or data field.  
It would also require member functions to perform the function the separator operators.

II) Inherit or overload from std::ostreams or most likely its base.  It is possible that the entire 
inheritance hierarchy would need to be duplicated, renamed and compiled rather than some simpler 
procedure.  Not sure how this would work if at all, in a practical manner, but perhaps 
somehow the .flush() of the internal output buffer could be used or adapted.

III) Wrapper Class: Create a class that defines all, or many of the operators and manipulators that std::ostream implements.  
Within that class, perhaps std::cout could be called.

IV) Template Parameter:  Explore and potentially utilize the ostream<Tchar> approach, where Tchar in a novel class that 
somehow implements the required functionality.  Or perhaps there is another candidate such as std::traits, or other.  
Perhaps a function object could be passed in?
  
V) Calling into the base: Directly calling a method near the top of the class hierarchy.  
The likely target would be the output buffer.
It is possible that it would be a private or protected data or function member.  
This is unlikely to be possible, but should be considered.

Implementation:

The code of this repository is a growing body which illustrates some ideas on the data structures that may be used in some capacity.  
It gives hints on some of the above alternative design approaches, 
where some approaches are commented out. It also gives some hints on possible syntax and usage.
