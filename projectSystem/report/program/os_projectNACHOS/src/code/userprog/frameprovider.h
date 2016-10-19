/*
 * frameprovider.h
 *
 *  Created on: Jan 17, 2016
 *      Author: littlegirle
 */


// simbadSid 18.01.2015

// +b simbadSid 18.01.2015
#ifndef FRAMEPROVIDER_H_
#define FRAMEPROVIDER_H_

#include "bitmap.h"


#define	DEFAULT_FRAME_ALLOCATION_POLICY	RandomFree

enum FrameAllocationPolicy
{
	FirstFree,											// Looks for the free frame with the smallest index
	LastFree,											// Looks for the free frame with the biggest index
	RandomFree											// Looks randomly for a free frame
};

class FrameProvider
{
	public:
		// Builder / destroyer
		FrameProvider(unsigned int nbrPhysicalFrame);
		FrameProvider(unsigned int nbrPhysicalFrame, FrameAllocationPolicy fap);
		~FrameProvider();

		// Methodes
		int				GetEmptyFrame	();
		int				GetEmptyFrame	(FrameAllocationPolicy fap);
		void			ReleaseFrame	(unsigned int frameIndex);
		unsigned int	NumAvailFrame	();

	private:
		// Attributs
		BitMap					*frameUsage;			// Indicates the state (allocated or not) of each physical frame
		FrameAllocationPolicy	frameAllocationPolicy;

		// Methodes
		int	GetEmptyFrame_FirstFree	();
		int	GetEmptyFrame_LastFree	();
		int	GetEmptyFrame_RandomFree();
};

#endif

// +e simbadSid 18.01.2015
