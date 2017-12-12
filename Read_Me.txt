/**

*	@author Jason Fagerberg

*	@date December 10, 2017

*	@goal Sort file filled with strings using a merge sort

*		threads will be used to sort 16 equal segments

*		threads will be used for 8 merges of the 16 segments

*	@input file path as 'given_path'
*	@output sorted txt as 'given_path_s.txt
'
*	@assumptions file consists of a new string on each line, no holes in data

*	@dependencies string length =< 65

**/

Will take a file filled with one string per line, merge sort it, and give back sorted answer in a different file