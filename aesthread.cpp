#include "aesthread.h"
#include "AES.h"
#include <QDebug>

#define SIZE 16*30

AesThread::AesThread(QFile *srfp, QFile *dstFp, QString pwd, bool bEncrypt)
{
    srFile  = srfp;
    dstFile = dstFp;
    strKey  = pwd;
    isToEncrypt = bEncrypt;
}

void AesThread::run()
{
    TAesClass aes;
    QByteArray byteKey = strKey.toLatin1();
    aes.InitializePrivateKey(16, (UCHAR*)byteKey.data());
    char tmpChar[1024];
    char strChar[1024];
    int len = 0;
    qint32 totalRead = 0;
    while(!srFile->atEnd())
    {
        memset(tmpChar, 0, sizeof(tmpChar));
        memset(strChar, 0, sizeof(strChar));
        int size = srFile->read(strChar, SIZE);
        totalRead += size;
        emit sigSizeRead(totalRead);
        if(isToEncrypt)
            len = (int)aes.OnAesEncrypt((LPVOID)strChar, (DWORD)size, (LPVOID)tmpChar);
        else
            len = (int)aes.OnAesUncrypt((LPVOID)strChar, (DWORD)size, (LPVOID)tmpChar);
        dstFile->write(tmpChar, len);
        usleep(200);
    }
    emit sigFinished(isToEncrypt);
}

