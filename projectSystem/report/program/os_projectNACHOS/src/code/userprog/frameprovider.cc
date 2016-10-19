/*
 * frameprovider.cc
 *
 *  Created on: Jan 17, 2016
 *      Author: littlegirle
 */



// +b simbadSid 18.01.2015


#include "frameprovider.h"
#include "system.h"


FrameProvider::FrameProvider(unsigned int nbrPhysicalFrame)
{
	ASSERT(nbrPhysicalFrame <= NumPhysPages);

	this->frameUsage			= new BitMap(nbrPhysicalFrame);
	this->frameAllocationPolicy	= DEFAULT_FRAME_ALLOCATION_POLICY;
}

FrameProvider::FrameProvider(unsigned int nbrPhysicalFrame, FrameAllocationPolicy fap)
{
	ASSERT(nbrPhysicalFrame <= NumPhysPages);

	this->frameUsage			= new BitMap(nbrPhysicalFrame);
	this->frameAllocationPolicy	= fap;
}

FrameProvider::~FrameProvider()
{
	delete this->frameUsage;
}

//--------------------------------------------------------------
// Return a free frame index.  The corresponding physical memory is set with 0;
// If no free frame is available, -1 is returned.
// The returned frame will not be retrieved by this function until it get released.
// The frame allocation policy is indicated by the
// local attribute FrameAllocationPolicy
//--------------------------------------------------------------
int FrameProvider::GetEmptyFrame()
{
	int frameIndex = -1;

	switch(this->frameAllocationPolicy)												// Find a free frame
	{
		case FirstFree:
			frameIndex = this->GetEmptyFrame_FirstFree();
			break;
		case LastFree:
			frameIndex = this->GetEmptyFrame_LastFree();
			break;
		case RandomFree:
			frameIndex = this->GetEmptyFrame_RandomFree();
			break;
		default:
			DEBUG('a', "*** Unhandled frame allocation policy ***");
			ASSERT(false);
			return -2;
	}

	if (frameIndex == -1) return frameIndex;
	this->frameUsage->Mark(frameIndex);

	char *physicalAddress = &(machine->mainMemory[frameIndex*PageSize]);
	bzero (physicalAddress, PageSize);												// zero out the physical page

	return frameIndex;
}

//--------------------------------------------------------------
// Return a free frame index using the indicated FrameAllocationPolicy.
// The corresponding physical memory is set with 0;
// If no free frame is available, -1 is returned.
// The frame allocation policy is indicated by the
// local attribute FrameAllocationPolicy
//--------------------------------------------------------------
int FrameProvider::GetEmptyFrame(FrameAllocationPolicy fap)
{
	FrameAllocationPolicy oldFap = this->frameAllocationPolicy;

	this->frameAllocationPolicy = fap;
	int res = this->GetEmptyFrame();
	this->frameAllocationPolicy = oldFap;

	return res;
}

//--------------------------------------------------------------
// Release a frame got through GetEmptyFrame.
// After this call, the given frame may be retrieved again by GetEmptyFrame.
//--------------------------------------------------------------
void FrameProvider::ReleaseFrame(unsigned int frameIndex)
{
	ASSERT(frameIndex < NumPhysPages);
	ASSERT(this->frameUsage->Test(frameIndex));			// Check that the frame was allocated

	this->frameUsage->Clear(frameIndex);
}

unsigned int FrameProvider::NumAvailFrame()
{
	return this->frameUsage->NumClear();
}

//--------------------------------------------------------------
// Frame allocation methodes
//--------------------------------------------------------------
int FrameProvider::GetEmptyFrame_FirstFree()
{
	return this->frameUsage->FindFirst(1);
}

int FrameProvider::GetEmptyFrame_LastFree()
{
	return this->frameUsage->FindLast(1);
}

int FrameProvider::GetEmptyFrame_RandomFree()
{
	int freeIndexes[NumPhysPages], nbrFreeIndexes;

	nbrFreeIndexes = this->frameUsage->GetFreeBits(freeIndexes);
	if (nbrFreeIndexes == 0) return -1;

	return freeIndexes[Random()%nbrFreeIndexes];
}
// +e simbadSid 18.01.2016
