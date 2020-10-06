#ifndef IMPADDRTBL_H
#define IMPADDRTBL_H
#include <windows.h>
#include <QWidget>
#include <QDebug>
#include <QVector>
#include "pebase.h"

class ImpAddrTbl : public PEBase
{
    Q_OBJECT
public:
    explicit ImpAddrTbl();

    void ShowImportedDllsName();

signals:

public slots:


private:
    PIMAGE_IMPORT_DESCRIPTOR m_pImplDesc;
    QVector<QString> m_VecDllName;


};

#endif // IMPADDRTBL_H
