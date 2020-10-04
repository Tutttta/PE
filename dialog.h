#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <windows.h>
#include <tchar.h>

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

    DWORD RVAToOffset(DWORD dwRVA); // 暂存
private:
    Ui::Dialog *ui;

    PBYTE m_pAddr;
    QString m_strFilePathName;
    PIMAGE_DOS_HEADER m_pDosHdr;
    PIMAGE_FILE_HEADER m_pFileHdr;
    PIMAGE_OPTIONAL_HEADER m_pOptHdr;
    PIMAGE_NT_HEADERS m_pNtHeaders;
    PIMAGE_SECTION_HEADER m_pSecHdr;

    int m_iNumOfSections;

    enum ColNum {
        colSecName = 0,
        colSecSize,
        colVirAddrSize,
        colRawFileSize,
        colRawFileOfst,
        colSecAttr
    };
};

#endif // DIALOG_H
