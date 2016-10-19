#ifndef KEY_LIST_H
#define	KEY_LIST_H

#include "utility.h"


// +b simbadSid 10.01.2016
class KeyList
{
	public:
		KeyList();
		KeyList(KeyList *list);
		~KeyList(){};												// Only free 1 cell.  To free all the memory use FreeAllList
		void	Prepend			(int KEY, void *ELEM);
		bool	Remove			(int KEY, void **ELEM);
		bool	IsEmpty			();
		bool	IsInList		(int KEY, void **ELEM);
		int		GetNbrElement	();
		void	PrintList		();
		void	FreeAllList		();
	    void	Mapcar 			(VoidFunctionPtr func);				// Apply the given function to all the elements of the list

	private:
		int		key;
		void	*elem;
		KeyList	*next;

};
// +e simbadSid 10.01.2016



#endif
