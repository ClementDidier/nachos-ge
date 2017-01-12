#include "frameprovider.h"

FrameProvider::FrameProvider(){
	frameMap = new BitMap(NumPhysPages);

}

FrameProvider::~FrameProvider(){
	delete frameMap;
}

int FrameProvider::GetEmptyFrame(){
	int pageIndex = frameMap->Find();
	if(pageIndex == -1){
		DEBUG('a', "Nombre de pages physiques inssufisant");
		ASSERT(FALSE);
	}
	bzero(machine->mainMemory + pageIndex * PageSize, PageSize);
	return pageIndex;
}

void ReleaseFrame(int frame){
	frameMap->Clear(frame);
}

int NumAvailFrame(){
	return frameMap->NumClear();
}