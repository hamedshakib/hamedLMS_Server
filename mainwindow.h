#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"

#include "qdebug.h"
#include "qsql.h"
#include "qsqldatabase.h"
#include "qsqlquery.h"
#include "qsqlerror.h"
#include "qsqlquerymodel.h"
#include <string>
#include "qtcpserver.h"
#include "qtcpsocket.h"
#include "qtimer.h"
#include "qfile.h"
#include "qfileinfo.h"
//#include "qudpsocket.h"


class mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    mainwindow(QWidget *parent = Q_NULLPTR);

private:
    Ui::mainwindowClass ui;


public:
    QSqlDatabase db;
    QString UsernameToName(QString username);
    void clear_addEdit();
    int now_username;

public slots:
    void initial_start();
    void create_database_table();
    void edit_user_database(QString former_username, QString NewUsername, QString Name, QString Password, int situation);
    void add_user_database(QString Username, QString Name, QString Password, int situation);
    void remove_user_database(QString Username);
    void show_database();
    void on_Edit_Button_clicked();
    void on_Remove_Button_clicked();
    void on_addEdit_Button_clicked();
    void listenToport1();
public:
    QTcpServer* server1;
    QTcpSocket* socketserver1;


    QString chats;
    QString textEdit1;
    QString classname;

    QMap<int, QString> map_socketUsername;
    QMap<int, int> map_socketAddence;
    QList<int> Addencesocket;

public slots:
    void newconnection1();
    void ReadData(int number1);
    void sendAttendence();
    int checkattendence(QString username);
   // void DatasendTonewClient(int number1);
    void changeAttenceSituation(int number1, char s);
    void editnameoflesson();

    void inistalRead();
    void disconnectSocket1(int connectionNumber2);
    void recivefile(QByteArray reciveed);
    void recivefilefromClient(QByteArray reciveed = QByteArray(QString("").toUtf8()));
    void sendpdftoClient(int pdfnumber, int page, QString username = "");
    void finishedrecive(int pdfnumber);

public:
    QTcpSocket *sockets[100];
    QTcpSocket* temp = new QTcpSocket(this);
    QTcpSocket* resivesocketFile = new QTcpSocket(this);
    int numberSockets;
    QByteArray allfile;
    int finishsocket;

    struct pdf_ha
    {
        int numberpage;
        QString name;
        QByteArray pages[999];
    }pdf[2];
    
};
