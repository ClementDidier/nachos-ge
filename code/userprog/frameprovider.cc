#include "frameprovider.h"

FrameProvider::FrameProvider(){
	frameMap = new BitMap(NumPhysPages);

}

FrameProvider::~FrameProvider(){
	delete frameMap;
}

int FrameProvider::FrameProvider::GetEmptyFrame(){
	int pageIndex = frameMap->Find();
	if(pageIndex == -1){
		DEBUG('a', "Impossible de récupérer la page physique\n");
		ASSERT(FALSE);
	}
	bzero(machine->mainMemory + pageIndex * PageSize, PageSize);
	return pageIndex;
}

void FrameProvider::ReleaseFrame(int frame){
	frameMap->Clear(frame);
}

int FrameProvider::NumAvailFrame(){
	return frameMap->NumClear();
}