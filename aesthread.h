#ifndef AESTHREAD_H
#define AESTHREAD_H

#include <QThread>
#include <QFile>
class AesThread : public QThread
{
    Q_OBJECT
public:
    AesThread(QFile *srfp, QFile *dstFp, QString pwd, bool bEncrypt=true);
    void run();

signals:
    void sigFinished(bool isEncrypt);
    void sigSizeRead(int size);
private:
    QFile *srFile;
    QFile *dstFile;
    bool isToEncrypt;
    QString strKey;
};

#endif // AESTHREAD_H
