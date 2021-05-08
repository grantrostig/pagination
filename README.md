# pagination via a paginator.

a Modern c++ class to control output to console so that data does not scroll past the user.

Inspired by more command as in: cat /etc/passwd | more
Anaylsis:

Provide functionality to prompt the user then the output within a transaction exceeds the capacity of the screen of the computer terminal.

If more than one screen has been prompted, during that prompt, 
the user by reply to the prompt by pressing a key to display the prior screen full of output.

The functionality should be as simple to use as possible, and require as few lines of code per output operation as possible.  
Preferably the usage should be as similar as possible to std::cout.

A transaction consists of one or more line items.  A line item consists of a logical whole, such as a customer record, 
for example consisting of a customer number, name, address, and perhaps a long text field, such as a description.

It is preferable to have the user prompted between line items.  In the case where one line item, or even just one data field, 
exceeds the total capacity of the screen, a prompt would need to be issued.

There must be some way of designating the beginning, as well as the end of a transaction, to assist pagination.
It is likely an operation to separate line items, and between data items will also be required.  These are termed: separator operators.  
These operators would also profide for "reset", "clear screen" and "clear to end of screen".

Design:

I) Simple Class: Seemingly the simplest approach is to create a single large complex class, as a global or a Singleton.
It would have a fully formated string passed to it.  It would then paginate the output.  
This approach would likely require four statements to output one line item, or data field.  
It would also require member functions to perform the function the separator operators.

II)


