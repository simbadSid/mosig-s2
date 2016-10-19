/*
 * mutualExclusionList.cc
 *
 *  Created on: Jan 25, 2016
 *      Author: littlegirle
 */

#include "mutualExclusionList.h"



//+b simbadSid 25.01.2016




//----------------------------------------------------------------------
// MutualExclusionList::MutualExclusionList
//      Allocate and initialize the data structures needed for a
//      synchronized list, empty to start with.
//      Elements can now be added to the list.
//----------------------------------------------------------------------

MutualExclusionList::MutualExclusionList()
{
    list = new List ();
    lock = new Lock ("list lock");
}

//----------------------------------------------------------------------
// MutualExclusionList::~MutualExclusionList
//      De-allocate the data structures created for synchronizing a list.
//----------------------------------------------------------------------

MutualExclusionList::~MutualExclusionList()
{
    delete list;
    delete lock;
}

//----------------------------------------------------------------------
// MutualExclusionList::Append
//      Append an "item" to the end of the list.
//
//      "item" is the thing to put on the list, it can be a pointer to
//              anything.
//----------------------------------------------------------------------

void MutualExclusionList::Append (void *item)
{
    lock->Acquire ();		// enforce mutual exclusive access to the list
    list->Append (item);
    lock->Release ();
}

//----------------------------------------------------------------------
// MutualExclusionList::Remove
//      Remove an "item" from the beginning of the list.
// Returns:
//      The removed item.
//----------------------------------------------------------------------

void *MutualExclusionList::Remove ()
{
    void *item;

    lock->Acquire ();										// enforce mutual exclusion
    item = list->Remove ();
    ASSERT (item != NULL);
    lock->Release ();
    return item;
}

//----------------------------------------------------------------------
// MutualExclusionList::Mapcar
//      Apply function to every item on the list.  Obey mutual exclusion
//      constraints.
//
//      "func" is the procedure to be applied.
//----------------------------------------------------------------------

void
MutualExclusionList::Mapcar (VoidFunctionPtr func)
{
    lock->Acquire ();
    list->Mapcar (func);
    lock->Release ();
}
