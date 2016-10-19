/*
 * KeyList.cc
 *
 *  Created on: Jan 18, 2016
 *      Author: littlegirle
 */

#include "keylist.h"

// +b simbadSid 10.01.2016


KeyList::KeyList()
{
	this->key	= -1;
	this->elem	= NULL;
	this->next	= NULL;
}

KeyList::KeyList(KeyList *list)
{
	this->key	= list->key;
	this->elem	= list->elem;
	this->next	= list->next;
}

void KeyList::Prepend(int KEY, void *ELEM)
{
	if (this->elem == NULL)										// Case: empty list
	{
		this->key	= KEY;
		this->elem	= ELEM;
		this->next	= NULL;
		return;
	}
	KeyList *NEXT;												// Case else: add the new at the beginning of the list
	NEXT		= new KeyList(this);
	this->key	= KEY;
	this->elem	= ELEM;
	this->next	= NEXT;
}

// ------------------------------------------------
// Looks for the thread tid in the list.
// If the thread is found, true is returned and the thread is put in the thread parameter (can be NULL).
// Else, false is returned.
// ------------------------------------------------
bool KeyList::Remove(int KEY, void **ELEM)
{
	if (this->elem	== NULL) return false;						// Case: empty list
	if (this->key	== KEY)										// Case: thread found
	{
		if (ELEM != NULL) *ELEM = this->elem;					//		Write the found thread in the output parameter
		if (this->next != NULL)									//		Case: the list contains more than 1 element
		{
			KeyList *tmpNext;
			tmpNext		= this->next;
			this->key	= this->next->key;
			this->elem	= this->next->elem;
			this->next	= this->next->next;
			delete tmpNext;
		}
		else this->elem = NULL;
		return true;
	}
	if (this->next == NULL)	return false;						// Case: end of list reached
	else					return this->next->Remove(KEY, ELEM);
}

bool KeyList::IsEmpty()
{
	return (this->elem == NULL);
}

// ------------------------------------------------
// Checks for the thread with the given tid in the list.
// If the outputThread is not null and the thread is found in the list, it is put in the outputThread variable.
// ------------------------------------------------
bool KeyList::IsInList (int KEY, void **ELEM)
{
	if (this->elem	== NULL) return false;
	if (this->key	== KEY)
	{
		if (ELEM != NULL) *ELEM = this->elem;
		return true;
	}
	if (this->next == NULL)	return false;
	else					return this->next->IsInList(KEY, ELEM);
}

int KeyList::GetNbrElement()
{
	if (this->elem	== NULL)	return 0;										// Case empty list
	if (this->next	== NULL)	return 1;
	else						return 1 + this->next->GetNbrElement();
}

void KeyList::PrintList()
{
	if (this->elem == NULL)
	{
		printf("\t(Empty list)\n");
		return;
	}
	printf("\tElement key = %d\n", this->key);
	if (this->next != NULL) this->next->PrintList();
}

void KeyList::FreeAllList()
{
	if (this->next == NULL) return;
	this->next->FreeAllList();
	delete this->next;
}

// +e simbadSid 10.01.2016

// +b simbadSid 25.01.2016
void KeyList::Mapcar(VoidFunctionPtr func)
{
	if (this->elem == NULL)	return;
	(*func) ((int) this->elem);
	if (this->next != NULL) this->next->Mapcar(func);
}
// +b simbadSid 25.01.2016
