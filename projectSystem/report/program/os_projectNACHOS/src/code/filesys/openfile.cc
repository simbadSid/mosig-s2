// openfile.cc 
//	Routines to manage an open Nachos file.  As in UNIX, a
//	file must be open before we can read or write to it.
//	Once we're all done, we can close it (in Nachos, by deleting
//	the OpenFile data structure).
//
//	Also as in UNIX, for convenience, we keep the file header in
//	memory while the file is open.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "filehdr.h"
#include "openfile.h"
#include "system.h"


OpenFile::OpenFile()
{
    isClosed = true;
}

//----------------------------------------------------------------------
// OpenFile::OpenFile
// 	Open a Nachos file for reading and writing.  Bring the file header
//	into memory while the file is open.
//
//	"sector" -- the location on disk of the file header for this file
//----------------------------------------------------------------------

OpenFile::OpenFile(int sector, int _openedFileEntryID, bool _isForWrite)
{
	hdr = new FileHeader;
	hdr->FetchFrom(sector);
	seekPosition = 0;
	openedFileEntryID = _openedFileEntryID;
	isForWrite = _isForWrite;
    isClosed = false;
}

//----------------------------------------------------------------------
// OpenFile::~OpenFile
// 	Close a Nachos file, de-allocating any in-memory data structures.
//----------------------------------------------------------------------

OpenFile::~OpenFile()
{
	if (!isClosed) {
		if (fileSystem != NULL) {
			fileSystem->openedFileStructure->RemoveFile(openedFileEntryID);
		}
		delete hdr;
		isClosed = true;

	}
}

//----------------------------------------------------------------------
// OpenFile::Seek
// 	Change the current location within the open file -- the point at
//	which the next Read or Write will start from.
//
//	"position" -- the location within the file for the next Read/Write
//----------------------------------------------------------------------

void
OpenFile::Seek(int position)
{
    seekPosition = position;
}	

//----------------------------------------------------------------------
// OpenFile::Read/Write
// 	Read/write a portion of a file, starting from seekPosition.
//	Return the number of bytes actually written or read, and as a
//	side effect, increment the current position within the file.
//
//	Implemented using the more primitive ReadAt/WriteAt.
//
//	"into" -- the buffer to contain the data to be read from disk 
//	"from" -- the buffer containing the data to be written to disk 
//	"numBytes" -- the number of bytes to transfer
//----------------------------------------------------------------------

int
OpenFile::Read(char *into, int numBytes)
{
   int result = ReadAt(into, numBytes, seekPosition);
   seekPosition += result;
   return result;
}

int OpenFile::Write(const char *into, int numBytes)
{
	if (!isForWrite) {
		DEBUG('f', "File is opened for reading, not for writing\n");
	}
	int result = WriteAt(into, numBytes, seekPosition);
	seekPosition += result;
	return result;
}

//----------------------------------------------------------------------
// OpenFile::ReadAt/WriteAt
// 	Read/write a portion of a file, starting at "position".
//	Return the number of bytes actually written or read, but has
//	no side effects (except that Write modifies the file, of course).
//
//	There is no guarantee the request starts or ends on an even disk sector
//	boundary; however the disk only knows how to read/write a whole disk
//	sector at a time.  Thus:
//
//	For ReadAt:
//	   We read in all of the full or partial sectors that are part of the
//	   request, but we only copy the part we are interested in.
//	For WriteAt:
//	   We must first read in any sectors that will be partially written,
//	   so that we don't overwrite the unmodified portion.  We then copy
//	   in the data that will be modified, and write back all the full
//	   or partial sectors that are part of the request.
//
//	"into" -- the buffer to contain the data to be read from disk 
//	"from" -- the buffer containing the data to be written to disk 
//	"numBytes" -- the number of bytes to transfer
//	"position" -- the offset within the file of the first byte to be
//			read/written
//----------------------------------------------------------------------

int
OpenFile::ReadAt(char *into, int numBytes, int position)
{
    int fileLength = hdr->FileLength();
    int i, firstSector, lastSector, numSectors;
    char *buf;

    if ((numBytes <= 0) || (position >= fileLength))
    	return 0; 				// check request
    if ((position + numBytes) > fileLength)		
	numBytes = fileLength - position;
    DEBUG('f', "Reading %d bytes at %d, from file of length %d.\n", 	
			numBytes, position, fileLength);

    firstSector = divRoundDown(position, SectorSize);
    lastSector = divRoundDown(position + numBytes - 1, SectorSize);
    numSectors = 1 + lastSector - firstSector;

    // read in all the full and partial sectors that we need
    buf = new char[numSectors * SectorSize];
    for (i = firstSector; i <= lastSector; i++)	
        synchDisk->ReadSector(hdr->ByteToSector(i * SectorSize), 
					&buf[(i - firstSector) * SectorSize]);

    // copy the part we want
    bcopy(&buf[position - (firstSector * SectorSize)], into, numBytes);
    delete [] buf;
    return numBytes;
}

int
OpenFile::WriteAt(const char *from, int numBytes, int position)
{
	if (!isForWrite) {
		DEBUG('f', "File is opened for reading, not for writing\n");
	}
    int fileLength = hdr->FileLength();
    int i, firstSector, lastSector, numSectors;
    bool firstAligned, lastAligned;
    char *buf;

    if ((numBytes <= 0) || (position >= fileLength))
	return 0;				// check request
    if ((position + numBytes) > fileLength)
	numBytes = fileLength - position;
    DEBUG('f', "Writing %d bytes at %d, from file of length %d.\n", 	
			numBytes, position, fileLength);
    
    firstSector = divRoundDown(position, SectorSize);
    lastSector = divRoundDown(position + numBytes - 1, SectorSize);
    numSectors = 1 + lastSector - firstSector;

    buf = new char[numSectors * SectorSize];

    firstAligned = (position == (firstSector * SectorSize));
    lastAligned = ((position + numBytes) == ((lastSector + 1) * SectorSize));
    
// read in first and last sector, if they are to be partially modified
    if (!firstAligned)
        ReadAt(buf, SectorSize, firstSector * SectorSize);	
    if (!lastAligned && ((firstSector != lastSector) || firstAligned))
        ReadAt(&buf[(lastSector - firstSector) * SectorSize], 
				SectorSize, lastSector * SectorSize);	

// copy in the bytes we want to change 
    bcopy(from, &buf[position - (firstSector * SectorSize)], numBytes);

    
// write modified sectors back
    for (i = firstSector; i <= lastSector; i++)	
        synchDisk->WriteSector(hdr->ByteToSector(i * SectorSize), 
					&buf[(i - firstSector) * SectorSize]);
    // TODO take care here of IndirectLink

    delete [] buf;

    return numBytes;
}

//----------------------------------------------------------------------
// OpenFile::Length
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int OpenFile::Length()
{ 
    return hdr->FileLength(); 
}

//----------------------------------------------------------------------
// OpenFile::Close
// 	Close a Nachos file, de-allocating any in-memory data structures.
//----------------------------------------------------------------------

void OpenFile::Close()
{
	if (!isClosed) {
		if (fileSystem != NULL) {
			fileSystem->openedFileStructure->RemoveFile(openedFileEntryID);
		}
		delete hdr;
		isClosed = true;

	}
}

// One entry of opened files.
OpenedFileEntry::OpenedFileEntry() {
	// We never delete from our array entry. What we do is just set is as not free.
	isFreeSlot = true;
}

OpenedFileEntry::~OpenedFileEntry() {}

// This structure contains information about currently opened files.
OpenedFileStructure::OpenedFileStructure() {
	entries = new OpenedFileEntry[OPEN_FILES_NUMB];
}

// This structure contains information about currently opened files.
OpenedFileStructure::~OpenedFileStructure() {
	delete entries;
}

bool OpenedFileStructure::CanOpen(int sector, bool isForWrite) {
	for (int i = 0; i < OPEN_FILES_NUMB; ++i) {
		if ((!entries[i].isFreeSlot) and (entries[i].sector == sector)) {
			if (isForWrite or entries[i].isForWrite) {
				return false;
			}
		}
	}
	return true;
}


int OpenedFileStructure::AddFile(int sector, bool isForWrite) {
	IntStatus oldLevel = interrupt->SetLevel (IntOff);
	if (!CanOpen(sector, isForWrite)) {
		DEBUG('f', "Not allowed to open file \n");
		return -1;
	}
	int freeEntryInd;
	for (freeEntryInd = 0; freeEntryInd < OPEN_FILES_NUMB; ++freeEntryInd) {
		if (entries[freeEntryInd].isFreeSlot) {
			break;
		}
	}
	if (freeEntryInd == OPEN_FILES_NUMB) {
		DEBUG('f', "Maximum number of opened files is 10, can not open\n");
	}
	entries[freeEntryInd].sector = sector;
	entries[freeEntryInd].isForWrite = isForWrite;
	entries[freeEntryInd].isFreeSlot = false;
    (void) interrupt->SetLevel (oldLevel);
    return freeEntryInd;
}

void OpenedFileStructure::RemoveFile(int id) {
	if (id < 0 or id >= OPEN_FILES_NUMB) {
		return;
	}
	entries[id].isFreeSlot = true;
}
