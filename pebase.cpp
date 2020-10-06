#include "pebase.h"

//extern PIMAGE_FILE_HEADER pFileHdr;
//extern PIMAGE_SECTION_HEADER pSecHdr;
//extern PBYTE pAddr;

PEBase::PEBase(QObject *parent) : QObject(parent)
{

}

// Unfinished
DWORD PEBase::_RVAToOffset(PBYTE pAddr, DWORD dwRVA)
{
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)pAddr;
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((PBYTE)pAddr + pDosHdr->e_lfanew);
    PIMAGE_FILE_HEADER pFileHdr = (PIMAGE_FILE_HEADER)(pNtHdr->FileHeader);

    for (int i = 0; i < pFileHdr->NumberOfSections; ++i) {
        if (dwRVA >= pSecHdr->VirtualAddress && dwRVA < pSecHdr->VirtualAddress + pSecHdr->Misc.VirtualSize) {
            DWORD dwFileOfst = dwRVA -
        }
    }
}

DWORD PEBase::_RVAToOffset(DWORD dwRVA)
{

}
