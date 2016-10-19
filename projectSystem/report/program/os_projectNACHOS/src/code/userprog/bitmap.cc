// bitmap.c 
//      Routines to manage a bitmap -- an array of bits each of which
//      can be either on or off.  Represented as an array of integers.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "bitmap.h"

//----------------------------------------------------------------------
// BitMap::BitMap
//      Initialize a bitmap with "nitems" bits, so that every bit is clear.
//      it can be added somewhere on a list.
//
//      "nitems" is the number of bits in the bitmap.
//----------------------------------------------------------------------

BitMap::BitMap (int nitems)
{
    numBits = nitems;
    numWords = divRoundUp (numBits, BitsInWord);
    map = new unsigned int[numWords];
    for (int i = 0; i < numBits; ++i) {
    	Clear(i);
    }
    numClear = nitems;
}

//----------------------------------------------------------------------
// BitMap::~BitMap
//      De-allocate a bitmap.
//----------------------------------------------------------------------

BitMap::~BitMap ()
{
  // LB: Missing [] in delete directive
  //  delete map;
  delete [] map;
  // End of modification
}

//----------------------------------------------------------------------
// BitMap::Set
//      Set the "nth" bit in a bitmap.
//
//      "which" is the number of the bit to be set.
//----------------------------------------------------------------------

void
BitMap::Mark (int which)
{
// +b simbadSid 20.01.2016
	--numClear;
// +e simbadSid 20.01.2016
    ASSERT (which >= 0 && which < numBits);
    map[which / BitsInWord] |= 1 << (which % BitsInWord);
}

//----------------------------------------------------------------------
// BitMap::Clear
//      Clear the "nth" bit in a bitmap.
//
//      "which" is the number of the bit to be cleared.
//----------------------------------------------------------------------

void
BitMap::Clear (int which)
{
// +b simbadSid 20.01.2016
	++numClear;
// +e simbadSid 20.01.2016
    ASSERT (which >= 0 && which < numBits);
    map[which / BitsInWord] &= ~(1 << (which % BitsInWord));
}

//----------------------------------------------------------------------
// BitMap::Test
//      Return TRUE if the "nth" bit is set.
//
//      "which" is the number of the bit to be tested.
//----------------------------------------------------------------------

bool BitMap::Test (int which)
{
    ASSERT (which >= 0 && which < numBits);

    if (map[which / BitsInWord] & (1 << (which % BitsInWord)))
    	return TRUE;
    else
    	return FALSE;
}

//----------------------------------------------------------------------
// BitMap::Find
//      Return the number of the first bit which is clear.
//      As a side effect, set the bit (mark it as in use).
//      (In other words, find and allocate a bit.)
//
//      If no bits are clear, return -1.
//----------------------------------------------------------------------

int BitMap::FindAndMark ()
{
    for (int i = 0; i < numBits; i++) {
		if (!Test (i)) {
			  Mark (i);
			  return i;
		  }
    }
    return -1;
}

// +b simbadSid 15.01.2015

//----------------------------------------------------------------------
// BitMap::Find first
//      Return the number of the first bit which is clear and has nbrBits free before.
//      If no bits are clear, return -1.
//----------------------------------------------------------------------

int
BitMap::FindFirst (int nbrBits)
{
	ASSERT(nbrBits >= 0);
	int j=0, i=0;

	for (i = 0; i <= numBits-nbrBits; i++)
	{
		if (!Test (i))
		{
			for (j=1; j<nbrBits; j++)
			{
				if (Test(i+j)){
					break;
				}
			}
			if (j == nbrBits){
				return i;
			}
			i += j-1;
		}
    }
    return -1;
}

//----------------------------------------------------------------------
// BitMap::FindLast
//      Return the number of the last bit which is clear and has nbrBits free before.
//      If no bits are clear, return -1.
//----------------------------------------------------------------------

int
BitMap::FindLast (int nbrBits)
{
	ASSERT(nbrBits >= 0);
	int j=0, i=0;

	for (i = numBits-1; i > nbrBits-1; i--)
	{
		if (!Test (i)) {
			for (j=1; j<nbrBits; j++)
			{
				if (Test(i-j)) {
					break;
				}
			}
			if (j == nbrBits) {
				return i;
			}
			i -= j+1;
		}
    }
    return -1;
}
// +e simbadSid 15.01.2015

//----------------------------------------------------------------------
// BitMap::NumClear
//      Return the number of clear bits in the bitmap.
//      (In other words, how many bits are unallocated?)
//----------------------------------------------------------------------

int
BitMap::NumClear ()
{
    return numClear;
}

// +b simbadSid 15.01.2015
// ----------------------------------------------------------------------
// Return the number of free bits and put them indexes in freeBitIndex.
// ----------------------------------------------------------------------
int BitMap::GetFreeBits(int freeBitIndex[])
{
	int count = 0;

	for (int i = 0; i < numBits; i++)
	{
		if (Test (i)) {
			continue;
		}
		freeBitIndex[count] = i;
		count++;
	}
	return count;
}
// +e simbadSid 15.01.2015

//----------------------------------------------------------------------
// BitMap::Print
//      Print the contents of the bitmap, for debugging.
//
//      Could be done in a number of ways, but we just print the #'s of
//      all the bits that are set in the bitmap.
//----------------------------------------------------------------------

void
BitMap::Print ()
{
    printf ("Bitmap set:\n");
    for (int i = 0; i < numBits; i++)
	if (Test (i)) {
	    printf ("%d, ", i);
	}
    printf ("\n");
}

// These aren't needed until the FILESYS assignment

//----------------------------------------------------------------------
// BitMap::FetchFromFile
//      Initialize the contents of a bitmap from a Nachos file.
//
//      "file" is the place to read the bitmap from
//----------------------------------------------------------------------

void
BitMap::FetchFrom (OpenFile * file)
{
    file->ReadAt ((char *) map, numWords * sizeof (unsigned), 0);
}

//----------------------------------------------------------------------
// BitMap::WriteBack
//      Store the contents of a bitmap to a Nachos file.
//
//      "file" is the place to write the bitmap to
//----------------------------------------------------------------------

void
BitMap::WriteBack (OpenFile * file)
{
    file->WriteAt ((char *) map, numWords * sizeof (unsigned), 0);
}