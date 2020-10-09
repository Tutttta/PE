#include "impaddrtbl.h"

//PIMAGE_DOS_HEADER m_pDosHdr = NULL;
//PIMAGE_FILE_HEADER m_pFileHdr = NULL;
extern PIMAGE_OPTIONAL_HEADER pOptHdr;
extern PBYTE pAddr;
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
void ImpAddrTbl::AnalyszImportAddrTbl()
{
    qDebug() << "In ShowImportedDllsName";

    int iNumOfImgImplDesc = 0;
    DWORD dwRVAOfImplTblDesc = (DWORD)m_pImplDesc;
    qDebug() << dwRVAOfImplTblDesc;
    PIMAGE_IMPORT_DESCRIPTOR pTmpImplDesc = (PIMAGE_IMPORT_DESCRIPTOR)((PBYTE)pAddr + _RVAToOffset(pAddr, (DWORD)dwRVAOfImplTblDesc));
    QString strNamesInAllLoadedDll;
    //qDebug() << QString::asprintf("第一个IMAGE_IMPORT_DESCRIPTOR地址: 0x%08X", _RVAToOffset(pAddr, (DWORD)pTmpImplDesc));

    while (pTmpImplDesc->Name || pTmpImplDesc->OriginalFirstThunk || pTmpImplDesc->FirstThunk || pTmpImplDesc->TimeDateStamp ||
           pTmpImplDesc->ForwarderChain) {
        const char *pName = (char *)((PBYTE)pAddr + _RVAToOffset(pAddr, pTmpImplDesc->Name));
        m_VecDllName.push_back(QString(pName));
        pTmpImplDesc += 1;
    }

}


