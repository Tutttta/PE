#include "impaddrtbl.h"

//PIMAGE_DOS_HEADER m_pDosHdr = NULL;
//PIMAGE_FILE_HEADER m_pFileHdr = NULL;
extern PIMAGE_OPTIONAL_HEADER pOptHdr;
//PIMAGE_NT_HEADERS m_pNtHeaders = NULL;
//PIMAGE_SECTION_HEADER m_pSecHdr = NULL;
//PBYTE m_pAddr = NULL;

ImpAddrTbl::ImpAddrTbl()
{
    m_pImplDesc = (PIMAGE_IMPORT_DESCRIPTOR)pOptHdr->DataDirectory[1].VirtualAddress; // 获取第一个IMAGE_IMPORT_DESCRIPTOR的地址

}

/*
 *
 *
 *
 *
 */
void ImpAddrTbl::ShowImportedDllsName()
{
    qDebug() << "In ShowImportedDllsName";
    int iNumOfImgImplDesc = 0;
    PIMAGE_IMPORT_DESCRIPTOR pTmpImplDesc = m_pImplDesc;
    qDebug() << QString::asprintf("0x%08X", m_pImplDesc);
    while (pTmpImplDesc->Characteristics) {
        qDebug() << pTmpImplDesc->Name;
        pTmpImplDesc += 1;
    }
}


