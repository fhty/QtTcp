#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QTimer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void sendFile();
private slots:
    void on_ButtonFile_clicked();

    void on_ButtonSend_clicked();

private:
    Ui::Widget *ui;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QFile file;
    QString fileName;
    qint64 fileSize;
    qint64 sendSize;
    QTimer timer;
};

#endif // WIDGET_H
