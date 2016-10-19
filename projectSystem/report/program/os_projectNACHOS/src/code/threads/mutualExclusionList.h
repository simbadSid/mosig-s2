/*
 * mutualExclusionList.h
 *
 *  Created on: Jan 25, 2016
 *      Author: littlegirle
 */

#ifndef MUTUALEXCLUSIONLIST_H_
#define MUTUALEXCLUSIONLIST_H_

#include "copyright.h"
#include "list.h"
#include "synch.h"

//+b simbadSid 25.01.2016


//------------------------------------------------------------------------------
// The following class defines a "mutual exclusion list" -- a list for which:
// these constraints hold: One thread at a time can access list data structures
//------------------------------------------------------------------------------

class MutualExclusionList
{
  public:
	MutualExclusionList ();							// initialize a mutualExclusion list
    ~MutualExclusionList ();						// de-allocate a mutualExclusion list

    void Append (void *item);						// append item to the end of the list
    void *Remove ();								// remove the first item from the front of the list
    void MutualExclusionList::Mapcar (VoidFunctionPtr func);

  private:
	List * list;
	Lock *lock;										// enforce mutual exclusive access to the list
};


#endif /* MUTUALEXCLUSIONLIST_H_ */
