#include "dialog.h"
#include "ui_dialog.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QFont>
#include <QTableWidgetItem>
#include <QMainWindow>

PIMAGE_DOS_HEADER pDosHdr = NULL;
PIMAGE_FILE_HEADER pFileHdr = NULL;
PIMAGE_OPTIONAL_HEADER pOptHdr = NULL;
PIMAGE_NT_HEADERS pNtHeaders = NULL;
PIMAGE_SECTION_HEADER pSecHdr = NULL;
PBYTE pAddr = NULL;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog), m_strFilePathName(""), m_pMenuBar(NULL), m_pMenuEdit(NULL),
    m_pMenuFile(NULL), m_pFile_Exit(NULL), m_pFile_Open(NULL)
{
    ui->setupUi(this);

    InitConnections();
    InitTblWidget();
    allocateMemFromHeaps();
    FontMgr();

    setWindowIcon(QIcon(":/ico/images/Show.bmp"));
    this->setFixedSize(510, 550);

}


// 未完成
void Dialog::allocateMemFromHeaps()
{
    m_pMenuBar = new QMenuBar(this);
    m_pMenuBar->setGeometry(QRect(0, 0, 510, 21));

    m_pMenuFile = m_pMenuBar->addMenu("&文件");
    m_pFile_Open = m_pMenuFile->addAction("&打开");
    m_pFile_Exit = m_pMenuFile->addAction("&退出");

    m_pMenuEdit = m_pMenuBar->addMenu("&编辑");


}


Dialog::~Dialog()
{
    if (NULL != pAddr) {
        UnmapViewOfFile(pAddr);
        pAddr = NULL;
    }

    delete ui;
}

void Dialog::InitConnections()
{
    //connect(ui->btnBrowse, &QPushButton::clicked, this, &Dialog::on_btnBrowse_clicked);
    //connect(ui->btnExit, &QPushButton::clicked, this, &Dialog::on_btnExit_clicked);

}


void Dialog::on_btnBrowse_clicked()
{
    QString curPath = QDir::currentPath();
    QString dlgTitle = "选择一个文件";
    QString filter = "可执行文件(*.exe);;动态库文件(*.dll);;所有文件(*.*)";
    QString aFileName = QFileDialog::getOpenFileName(this, dlgTitle, curPath, filter);

    if (!aFileName.isEmpty()) {
        ui->editFileNames->setText(aFileName);
        this->m_strFilePathName = aFileName; // 保存读取的文件名
    }
}

void Dialog::on_btnExit_clicked()
{
    emit Dialog::close();
}

bool Dialog::FileMappingToMem()
{
    HANDLE hFile = NULL;
    HANDLE hFileMapping = NULL;

    const char *pFilePath = NULL;
    if (m_strFilePathName.isEmpty()) {
        QMessageBox::critical(this, "错误", "请先选择一个文件", QMessageBox::Ok);
        return(false);
    }
    pFilePath = m_strFilePathName.toUtf8().data();

    hFile = CreateFileA(pFilePath, FILE_READ_ACCESS, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile) {
        qDebug() << "CreateFileA filed: " << GetLastError();
        return(false);
    }
    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (NULL == hFileMapping) {
        CloseHandle(hFile);
        qDebug() << "CreateFileMapping filed: " << GetLastError();
        return(false);
    }
    pAddr = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if (NULL == pAddr) {
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        qDebug() << "MapViewOfFile failed: " << GetLastError() << endl;
        return(false);
    }
    CloseHandle(hFile);
    CloseHandle(hFileMapping);

    return(true);
}

bool Dialog::isFileisPortableExecute()
{
    if (NULL == pAddr) {
        if (!FileMappingToMem())
            return(false);
    }
    pDosHdr = (PIMAGE_DOS_HEADER)(pAddr);
    pNtHeaders = (PIMAGE_NT_HEADERS)(pAddr + pDosHdr->e_lfanew);
    pFileHdr = (PIMAGE_FILE_HEADER)(&pNtHeaders->FileHeader);
    pOptHdr = (PIMAGE_OPTIONAL_HEADER)(&pNtHeaders->OptionalHeader);
    pSecHdr = (PIMAGE_SECTION_HEADER)(pAddr + pDosHdr->e_lfanew + sizeof(IMAGE_NT_HEADERS));
    if (IMAGE_DOS_SIGNATURE == pDosHdr->e_magic && IMAGE_NT_SIGNATURE == pNtHeaders->Signature)
        return(true);
    else
        return(false);
}

QString Dialog::GetPlatFormType()
{
    char szPltType[11] = {0};
    if (!pFileHdr) {
        qDebug() << "GetPlatFormType failed!";
        return("");
    }

    return(QString::asprintf("0x%04X", pFileHdr->Machine));
}

QString Dialog::GetNumberOfSections()
{
    char szSecNums[11] = {0};
    if (!pFileHdr) {
        qDebug() << "GetNumberOfSections failed!";
        return("");
    }
    m_iNumOfSections = pFileHdr->NumberOfSections;

    return(QString::asprintf("%d", pFileHdr->NumberOfSections));
}

QString Dialog::GetBaseImageAddr()
{
    if (!pOptHdr) {
        qDebug() << "GetBaseImageAddr failed!";
        return("");
    }

    return(QString::asprintf("0x%08X", pOptHdr->ImageBase));
}

QString Dialog::GetFileMark()
{
    if (!pOptHdr) {
        qDebug() << "GetFileMark failed!";
        return("");
    }

    return(QString::asprintf("0x%04X", pOptHdr->Magic));
}

void Dialog::GetSectionInfo()
{
    QString strSectionName;
    DWORD dwSizeOfSection;
    DWORD dwRVA;
    DWORD dwRawFileSize;
    DWORD dwRawFileOffset;
    DWORD dwSecCharacteristics;
    PIMAGE_SECTION_HEADER pTmp = pSecHdr;
    if (!pSecHdr && m_iNumOfSections <= 0) {
        qDebug() << "GetSectionInfo failed!";
        return;
    }

    for (int i = 0; i < m_iNumOfSections; ++i) {
        strSectionName = QString::asprintf("%s", pTmp->Name);
        dwRVA = pTmp->VirtualAddress;
        dwSizeOfSection = pTmp->Misc.VirtualSize;
        dwRawFileSize = pTmp->SizeOfRawData;
        dwRawFileOffset = pTmp->PointerToRawData;
        dwSecCharacteristics = pTmp->Characteristics;
        CreateRowItems(i, strSectionName, dwSizeOfSection, dwRVA, dwRawFileSize, dwRawFileOffset, dwSecCharacteristics);
        pTmp += 1;
    }
}

void Dialog::CreateRowItems(int iRowNum, QString &strName, DWORD dwSecSize, DWORD dwVirAddr, DWORD dwRawSize, DWORD dwRawOffset, DWORD dwAttrs)
{
    QTableWidgetItem *item = NULL;
    QString strSecSize = QString::asprintf("0x%X", dwSecSize);
    QString strVirAddr = QString::asprintf("0x%X", dwVirAddr);
    QString strRawSize = QString::asprintf("0x%X", dwRawSize);
    QString strRawOfst = QString::asprintf("0x%X", dwRawOffset);
    QString strAttrs = QString::asprintf("0x%X", dwAttrs);
    bool fNameOk = false;
    for (auto &i : strName) {
        if (i == '.')
            fNameOk = true;
    }
    if (!fNameOk)
        strName = "Nul";

    ui->tblWidget->insertRow(iRowNum);
    item = new QTableWidgetItem(strName);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tblWidget->setItem(iRowNum, Dialog::colSecName, item);

    item = new QTableWidgetItem(strSecSize);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tblWidget->setItem(iRowNum, Dialog::colSecSize, item);

    item = new QTableWidgetItem(strVirAddr);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tblWidget->setItem(iRowNum, Dialog::colVirAddrSize, item);

    item = new QTableWidgetItem(strRawSize);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tblWidget->setItem(iRowNum, Dialog::colRawFileSize, item);

    item = new QTableWidgetItem(strRawOfst);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tblWidget->setItem(iRowNum, Dialog::colRawFileOfst, item);

    item = new QTableWidgetItem(strAttrs);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tblWidget->setItem(iRowNum, Dialog::colSecAttr, item);
}

void Dialog::FontMgr()
{
    QFont font;

    font = ui->editPlatform->font();
    font.setBold(true);
    font.setFamily("Consolas");
    font.setPointSize(12);
    ui->editAdvLoadAddr->setFont(font);
    ui->editFileMark->setFont(font);
    ui->editFileNames->setFont(font);
    ui->editPlatform->setFont(font);
    ui->editSectionNum->setFont(font);
}

void Dialog::on_btnQuery_clicked()
{
    if (!isFileisPortableExecute()) {
        qDebug() << "on_btnQuery_clicked failed" << endl;
        return;
    }
    // 必须要让isFileisPortableExecute先执行，不然全局变量未被赋值
    m_pIAT = new ImpAddrTbl;

    ui->editPlatform->setText(GetPlatFormType());
    ui->editFileMark->setText(GetFileMark());
    ui->editSectionNum->setText(GetNumberOfSections());
    ui->editAdvLoadAddr->setText(GetBaseImageAddr());
    GetSectionInfo();
    m_pIAT->ShowImportedDllsName();
}

void Dialog::InitTblWidget()
{
    QStringList headerText;
    QTableWidgetItem *tblItem = NULL;
    headerText << "节区名称" << "节区大小" << "虚拟地址" << "Raw尺寸" << "Raw偏移" << "节区属性";

    ui->tblWidget->setColumnCount(headerText.count());
    for (int i = 0; i < ui->tblWidget->columnCount(); ++i) {
        tblItem = new QTableWidgetItem(headerText.at(i));
        QFont font = tblItem->font();
        font.setBold(true);
        font.setPointSize(12);
        tblItem->setTextColor(Qt::blue);
        tblItem->setFont(font);
        ui->tblWidget->setHorizontalHeaderItem(i, tblItem);
    }
}








