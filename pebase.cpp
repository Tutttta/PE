#include "pebase.h"
#include <qDebug>
//extern PIMAGE_FILE_HEADER pFileHdr;
//extern PIMAGE_SECTION_HEADER pSecHdr;
//extern PBYTE pAddr;

PEBase::PEBase(QObject *parent) : QObject(parent)
{

}

DWORD PEBase::_RVAToOffset(PBYTE pAddr, DWORD dwRVA)
{
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)pAddr;
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((PBYTE)pAddr + pDosHdr->e_lfanew);
    PIMAGE_FILE_HEADER pFileHdr = (PIMAGE_FILE_HEADER)&(pNtHdr->FileHeader);
    PIMAGE_SECTION_HEADER pSecHdr = (PIMAGE_SECTION_HEADER)((PBYTE)&(pNtHdr->OptionalHeader) + pFileHdr->SizeOfOptionalHeader);
    DWORD dwFileOfst;


    for (int i = 0; i < pFileHdr->NumberOfSections; ++i) {
//        QString addr = QString::asprintf("0x%08X", pSecHdr->VirtualAddress);
//        qDebug() << "第" << i + 1 << "个节表虚拟地址: " << addr;
        if (dwRVA >= pSecHdr->VirtualAddress && dwRVA < pSecHdr->VirtualAddress + pSecHdr->Misc.VirtualSize) {
            dwFileOfst = dwRVA - pSecHdr->VirtualAddress + pSecHdr->PointerToRawData;
            return(dwFileOfst);
        }
        PBYTE p = (PBYTE)pSecHdr + sizeof(IMAGE_SECTION_HEADER);
        pSecHdr = (PIMAGE_SECTION_HEADER)p;
    }

    return(-1);
}
