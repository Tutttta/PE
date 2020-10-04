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



Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog), m_strFilePathName(""), m_pDosHdr(NULL),
    m_pNtHeaders(NULL), m_pFileHdr(NULL), m_pOptHdr(NULL), m_pAddr(NULL)
{


    ui->setupUi(this);
    InitConnections();
    InitTblWidget();
    FontMgr();

    setWindowIcon(QIcon(":/ico/images/Show.bmp"));
    this->setFixedSize(510, 550);

}

Dialog::~Dialog()
{
    if (NULL != m_pAddr) {
        UnmapViewOfFile(m_pAddr);
        m_pAddr = NULL;
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
    m_pAddr = (PBYTE)MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
    if (NULL == m_pAddr) {
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
    if (NULL == m_pAddr) {
        if (!FileMappingToMem())
            return(false);
    }
    m_pDosHdr = (PIMAGE_DOS_HEADER)(m_pAddr);
    m_pNtHeaders = (PIMAGE_NT_HEADERS)(m_pAddr + m_pDosHdr->e_lfanew);
    m_pFileHdr = (PIMAGE_FILE_HEADER)(&m_pNtHeaders->FileHeader);
    m_pOptHdr = (PIMAGE_OPTIONAL_HEADER)(&m_pNtHeaders->OptionalHeader);
    m_pSecHdr = (PIMAGE_SECTION_HEADER)(m_pAddr + m_pDosHdr->e_lfanew + sizeof(IMAGE_NT_HEADERS));
    if (IMAGE_DOS_SIGNATURE == m_pDosHdr->e_magic && IMAGE_NT_SIGNATURE == m_pNtHeaders->Signature)
        return(true);
    else
        return(false);
}

QString Dialog::GetPlatFormType()
{
    char szPltType[11] = {0};
    if (!m_pFileHdr) {
        qDebug() << "GetPlatFormType failed!";
        return("");
    }

    return(QString::asprintf("0x%04X", m_pFileHdr->Machine));
}

QString Dialog::GetNumberOfSections()
{
    char szSecNums[11] = {0};
    if (!m_pFileHdr) {
        qDebug() << "GetNumberOfSections failed!";
        return("");
    }
    m_iNumOfSections = m_pFileHdr->NumberOfSections;

    return(QString::asprintf("%d", m_pFileHdr->NumberOfSections));
}

QString Dialog::GetBaseImageAddr()
{
    if (!m_pOptHdr) {
        qDebug() << "GetBaseImageAddr failed!";
        return("");
    }

    return(QString::asprintf("0x%08X", m_pOptHdr->ImageBase));
}

QString Dialog::GetFileMark()
{
    if (!m_pOptHdr) {
        qDebug() << "GetFileMark failed!";
        return("");
    }

    return(QString::asprintf("0x%04X", m_pOptHdr->Magic));
}

void Dialog::GetSectionInfo()
{
    QString strSectionName;
    DWORD dwSizeOfSection;
    DWORD dwRVA;
    DWORD dwRawFileSize;
    DWORD dwRawFileOffset;
    DWORD dwSecCharacteristics;
    PIMAGE_SECTION_HEADER pTmp = m_pSecHdr;
    if (!m_pSecHdr && m_iNumOfSections <= 0) {
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
    ui->editPlatform->setText(GetPlatFormType());
    ui->editFileMark->setText(GetFileMark());
    ui->editSectionNum->setText(GetNumberOfSections());
    ui->editAdvLoadAddr->setText(GetBaseImageAddr());
    GetSectionInfo();
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








