#ifndef FILEENCRYPT_H
#define FILEENCRYPT_H

#include <QMainWindow>
#include "aesthread.h"
class QFile;
class QFileInfo;
namespace Ui {
class FileEncrypt;
}

class FileEncrypt : public QMainWindow
{
    Q_OBJECT

public:
    explicit FileEncrypt(QWidget *parent = 0);
    ~FileEncrypt();

private slots:
    void on_hashCheckBtn_clicked();

    void on_enBtn_clicked();

    void on_deBtn_clicked();

    void on_openBtn_clicked();

    void responsFinished(bool isEncrypt);

    void drawProceess(int value);

private:
    Ui::FileEncrypt *ui;
    QFile *srFile;
    QFile *dstFile;
    QFileInfo *info;
    AesThread *pro;
};

#endif // FILEENCRYPT_H
