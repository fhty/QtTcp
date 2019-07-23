#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QFileInfo>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    tcpServer=new QTcpServer(this);
    tcpSocket=new QTcpSocket(this);
    setWindowTitle("server:8888");
    tcpServer->listen(QHostAddress::Any,8888);
    ui->ButtonFile->setEnabled(0);
    ui->ButtonSend->setEnabled(0);

    connect(tcpServer,&QTcpServer::newConnection,this,[=](){
        tcpSocket=tcpServer->nextPendingConnection();
        QString ip=tcpSocket->peerAddress().toString();
        quint16 port=tcpSocket->peerPort();
        ui->textEdit->setText(QString("%1,%2:connected!").arg(ip).arg(port));
        ui->ButtonFile->setEnabled(1);
        ui->ButtonSend->setEnabled(1);
    });
    connect(tcpSocket,&QTcpSocket::readyRead,this,[=](){
        QByteArray buf=tcpSocket->readAll();
        if(buf=="all done"){
            file.close();
            ui->textEdit->append(QString("client receive done!"));
            tcpSocket->disconnectFromHost();
            tcpSocket->close();
        }
    });
    connect(&timer,&QTimer::timeout,this,[=](){
        timer.stop();
        sendFile();
    });

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_ButtonFile_clicked()
{
    QString path=QFileDialog::getOpenFileName(this,"select:","../",".()");
    if(path.isEmpty()) return;

    QFileInfo info(path);
    fileName=info.fileName();
    fileSize=info.size();
    sendSize=0;

    file.setFileName(path);
    bool isok= file.open(QIODevice::ReadOnly);
    if(isok){
        ui->textEdit->append(path);
        ui->ButtonSend->setEnabled(1);
        ui->ButtonFile->setEnabled(0);
    }
}

void Widget::on_ButtonSend_clicked()
{
    QString head=QString("%1#%2").arg(fileName).arg(fileSize);
    qint64 len=tcpSocket->write(head.toUtf8());
    if(len>0){
        ui->textEdit->append(QString("send start!"));
        timer.start(100);
    }else {
        ui->textEdit->append(QString("send failed!"));
        ui->ButtonSend->setEnabled(1);
        ui->ButtonFile->setEnabled(1);
    }

}

void Widget::sendFile()
{
    qint64 len=0;
    do{
        char buff[2048]={0};
        len=file.read(buff,sizeof(buff));
        len=tcpSocket->write(buff,len);
        sendSize+=len;
    }while(len>0);

    if(sendSize==fileSize){
        file.close();
        ui->textEdit->append(QString("send success!"));
        tcpSocket->disconnectFromHost();
        tcpSocket->close();
    }else {
        ui->textEdit->append(QString("send failed,please reselect!"));
    }
}


