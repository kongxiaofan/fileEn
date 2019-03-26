#include "fileencrypt.h"
#include "ui_fileencrypt.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

const QString DEFAULT_KEY = "1234567887654321";

FileEncrypt::FileEncrypt(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FileEncrypt)
{
    srFile  = NULL;
    dstFile = NULL;
    info    = NULL;
    pro     = NULL;
    ui->setupUi(this);
}

FileEncrypt::~FileEncrypt()
{
    if(srFile != NULL && srFile->isOpen())
        srFile->close();
    if(dstFile != NULL && dstFile->isOpen())
        dstFile->close();
    if(info)
        delete info;
    if(pro)
        delete info;
    delete ui;
}

/**
 * @brief FileEncrypt::on_hashCheckBtn_clicked
 * @note can not hash the big file
 * @bug  1. fix the file to seek 0
 */
void FileEncrypt::on_hashCheckBtn_clicked()
{
    QByteArray strByte = srFile->readAll();
    QByteArray byteMd5 = QCryptographicHash::hash(strByte, QCryptographicHash::Md5);
    QByteArray byteSha256 = QCryptographicHash::hash(strByte, QCryptographicHash::Sha256);
    QString str = "MD5: " + byteMd5.toHex() + "\n" +
                  "SHA256: " + byteSha256.toHex() + "\n";
    QMessageBox::about(this, "HASH值", str);
    srFile->seek(0);
}


void FileEncrypt::on_enBtn_clicked()
{
    QString baseName = info->baseName();
    if(info->suffix() == "enc")
    {
        int rb = QMessageBox::warning(this, "警告", "该文档已是加密文件！是否继续加密？", QMessageBox::Yes, QMessageBox::No);
        if(rb == QMessageBox::No)
            return;
    }

    QString strKey = ui->pwdLE->text();
    if(strKey.isEmpty())
        strKey = DEFAULT_KEY;
    if(strKey.isEmpty() || strKey.size() != 16)
    {
        QMessageBox::critical(this, "警告", "请输入16位密码！");
        return;
    }
    QString enFilePath =  baseName + "_" + info->suffix() +".enc";
    dstFile = new QFile(enFilePath);
    if(!dstFile->open(QFile::WriteOnly))
    {
        qDebug("open en file error");
        return;
    }
    pro = new AesThread(srFile, dstFile, strKey);
    connect(pro, SIGNAL(sigFinished(bool)), SLOT(responsFinished(bool)));
    connect(pro, SIGNAL(sigSizeRead(int)), SLOT(drawProceess(int)));
    pro->start();
}

void FileEncrypt::on_deBtn_clicked()
{
    if(info->suffix() != "enc")
    {
        int rb = QMessageBox::warning(this, "警告", "该文档不是加密文件！是否继续解密？", QMessageBox::Yes, QMessageBox::No);
        if(rb == QMessageBox::No)
            return;
    }
    QString strKey = ui->pwdLE->text();
    if(strKey.isEmpty())
        strKey = DEFAULT_KEY;
    if(strKey.isEmpty() || strKey.size() != 16)
    {
        QMessageBox::critical(this, "警告", "请输入16位密码！");
        return;
    }

    QString deFilePath="";
    QString baseName = info->baseName();
    if(info->suffix() == "enc")
    {
        if(baseName.contains("_"))
        {
            QStringList list = baseName.split("_");
            for(int i=0; i<list.size()-1; i++)
            {
                deFilePath += list[i];
            }
            deFilePath += "_de." + list[list.size() - 1];
        }
    }
    else
    {
        deFilePath = info->baseName() + "_de.dec";
    }

    dstFile = new QFile(deFilePath);
    if(!dstFile->open(QFile::WriteOnly))
    {
        qDebug("open en file error");
        return;
    }
    pro = new AesThread(srFile, dstFile, strKey, false);
    connect(pro, SIGNAL(sigFinished(bool)), SLOT(responsFinished(bool)));
    connect(pro, SIGNAL(sigSizeRead(int)), SLOT(drawProceess(int)));
    pro->start();
}

void FileEncrypt::on_openBtn_clicked()
{
    if(srFile != NULL)
    {
        srFile->close();
        srFile = NULL;
    }

    QString path = QFileDialog::getOpenFileName(NULL, "标题", ".", "*.txt;;*.enc;;*.jpg;;*.*");
    qDebug()<<"path: "<< path;
    if(path.isEmpty())
        return;
    info = new QFileInfo(path);
    srFile = new QFile(path);

    if(!srFile->open(QFile::ReadOnly))
        qDebug()<<"error open!";
    ui->progressBar->setMaximum(info->size());
    ui->fileLE->setText(path);
    ui->hashCheckBtn->setEnabled(true);
    ui->enBtn->setEnabled(true);
    ui->deBtn->setEnabled(true);
}

void FileEncrypt::responsFinished(bool isEncrypt)
{
    dstFile->close();
    delete pro;
    pro = NULL;
    QString str;
    if(isEncrypt)
        str = "加密";
    else
        str = "解密";
    QMessageBox::information(this, str, str+"完成！");
    ui->enBtn->setEnabled(false);
    ui->deBtn->setEnabled(false);
    ui->hashCheckBtn->setEnabled(false);
    ui->progressBar->setValue(0);
}

void FileEncrypt::drawProceess(int value)
{
    ui->progressBar->setValue(value);
}
