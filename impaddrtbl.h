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

    void AnalyszImportAddrTbl();

signals:

public slots:

public:
    const QVector<QString> &GetDllNamesVec() const {
        return(m_VecDllName);
    }

private:
    PIMAGE_IMPORT_DESCRIPTOR m_pImplDesc;
    QVector<QString> m_VecDllName;

};

#endif // IMPADDRTBL_H
