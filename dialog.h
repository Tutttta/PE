#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <windows.h>
#include <tchar.h>
#include <QMenuBar>
#include <QMenu>
#include "impaddrtbl.h"

extern PBYTE pAddr;
extern PIMAGE_DOS_HEADER pDosHdr;
extern PIMAGE_FILE_HEADER pFileHdr;
extern PIMAGE_OPTIONAL_HEADER pOptHdr;
extern PIMAGE_NT_HEADERS pNtHeaders;
extern PIMAGE_SECTION_HEADER pSecHdr;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
private:
    void InitConnections();
private slots:
    void on_btnBrowse_clicked();

    void on_btnExit_clicked();

    void on_btnQuery_clicked();

private:
    void InitTblWidget();

    bool FileMappingToMem();
    bool isFileisPortableExecute();

    QString GetPlatFormType();
    QString GetNumberOfSections();
    QString GetBaseImageAddr();
    QString GetFileMark();

    void GetSectionInfo();

    void CreateRowItems(int iRowNum, QString &str, DWORD dwSecSize, DWORD dwVirAddr, DWORD dwRawSize, DWORD dwRawOffset, DWORD dwAttrs);


    void FontMgr();
    void allocateMemFromHeaps();

    DWORD RVAToOffset(DWORD dwRVA);

    int m_iNumOfSections;

//    struct _BASEPEINFO {
//        PIMAGE_DOS_HEADER pDosHdr;
//        PIMAGE_FILE_HEADER pFileHdr;
//        PIMAGE_OPTIONAL_HEADER pOptHdr;
//        PIMAGE_NT_HEADERS pNtHeaders;
//        PIMAGE_SECTION_HEADER pSecHdr;
//        int iNumOfSections;
//    } BasePeInfo;

    Ui::Dialog *ui;
    QString m_strFilePathName;
    enum ColNum {
        colSecName = 0,
        colSecSize,
        colVirAddrSize,
        colRawFileSize,
        colRawFileOfst,
        colSecAttr
    };

    ImpAddrTbl *m_pIAT;
public:
    QMenuBar *m_pMenuBar;
    QMenu *m_pMenuFile;
    QAction *m_pFile_Open;
    QAction *m_pFile_Exit;
    QMenu *m_pMenuEdit;
};

#endif // DIALOG_H
