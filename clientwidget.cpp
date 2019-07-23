#include "clientwidget.h"
#include "ui_clientwidget.h"
#include <QByteArray>

ClientWidget::ClientWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientWidget)
{
    ui->setupUi(this);
    tcpSocket=new QTcpSocket(this);

    bool isHead=true;
    ui->progressBar_prog->setValue(0);
    connect(tcpSocket,&QTcpSocket::readyRead,this,[&](){
        QByteArray buf=tcpSocket->readAll();
        if(isHead){
            isHead=false;

            fileName=QString(buf).section("#",0,0);
            fileSize=QString(buf).section("#",1,1).toInt();
            recvSize=0;
            ui->textEdit->setText(QString("file name:%1").arg(fileName));
            ui->textEdit->append(QString("size:%1").arg(fileSize));
            ui->progressBar_prog->setMaximum(fileSize/1024);
            ui->progressBar_prog->setMinimum(0);

            file.setFileName(fileName);
            bool isOpened=file.open(QIODevice::WriteOnly);
            if(isOpened){
                ui->textEdit->append(QString("creat file!"));
            }else {
                ui->textEdit->append(QString("creat failed!"));
            }
        }else {
            qint64 len=file.write(buf);
            if(len>0){
                recvSize+=len;
                ui->progressBar_prog->setValue(recvSize/1024);
                ui->textEdit->append(QString("write data..."));
            }
            if(recvSize==fileSize){
                file.close();
                tcpSocket->write("all done");
                ui->textEdit->append(QString("receive success!"));
            }
        }
    });
}

ClientWidget::~ClientWidget()
{
    delete ui;
}

void ClientWidget::on_pushButton_connect_clicked()
{
    tcpSocket->connectToHost(ui->lineEdit_ip->text(),ui->lineEdit_port->text().toInt());
}
