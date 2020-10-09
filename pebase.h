#ifndef PEBASE_H
#define PEBASE_H
#include <windows.h>
#include <QObject>

class PEBase : public QObject
{
    Q_OBJECT
public:
    explicit PEBase(QObject *parent = nullptr);

signals:

public slots:

protected:
    DWORD _RVAToOffset(PBYTE pAddr, DWORD dwRVA);
};

#endif // PEBASE_H
