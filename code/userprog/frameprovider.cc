#include "frameprovider.h"

#define RANDOMLY TRUE

FrameProvider::FrameProvider()
{
	frameMap = new BitMap(NumPhysPages);

}

FrameProvider::~FrameProvider()
{
	delete frameMap;
}

int FrameProvider::FrameProvider::GetEmptyFrame()
{
	int pageIndex = -1;

	// Selection de l'index du cadre aléatoirement ou par first find
	if(RANDOMLY)
	{
		/**************** Aléatoire *****************/
		RandomInit(1);
		int availIndex[NumPhysPages];
		int i, index = 0;
		for(i = 0; i < NumPhysPages; i++)
		{
			if(!frameMap->Test(i))
			{
				availIndex[index++] = i;
			}
		}

		if(index > 0)
		{
			pageIndex = availIndex[Random() % index];
			frameMap->Mark(pageIndex);
		}
		/********************************************/
	}
	else
	{
		pageIndex = frameMap->Find();
	}

	if(pageIndex == -1)
	{
		DEBUG('a', "Impossible de récupérer la page physique\n");
		ASSERT(FALSE);
	}
	bzero(machine->mainMemory + pageIndex * PageSize, PageSize);
	return pageIndex;
}

void FrameProvider::ReleaseFrame(int frame)
{
	frameMap->Clear(frame);
}

int FrameProvider::NumAvailFrame()
{
	return frameMap->NumClear();
}

int FrameProvider::ForkExec(char *s)
{
	scheduler->ReadyToRun(currentThread);
	
	OpenFile *executable = fileSystem->Open (s);
	Thread* t = new Thread("forked thread");
	AddrSpace *space;

    if (executable == NULL)
      {
	  printf ("Unable to open file %s\n", s);
	  return -1;
      }

    space = new AddrSpace (executable);
    
    space->InitRegisters ();
    space->RestoreState ();
    t->space = space;

    delete executable;

    scheduler->ReadyToRun (t);

    //scheduler->Run(t);
    //currentThread = t;
    scheduler->Print();
    printf("\n");

    //currentThread = t;
    //machine->Run();
    return 0;
}