#include "mainwindow.h"

int MembersNumber = 0;

mainwindow::mainwindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    initial_start();
}

void mainwindow::initial_start() {
    create_database_table();
    show_database();
    ui.addEdit_Button->setText("Add");
    listenToport1();
    connect(ui.lineEdit, &QLineEdit::textEdited, this, &mainwindow::editnameoflesson);

}

void mainwindow::create_database_table() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("lms1server.db");
    qDebug() << "databse_open:" << db.open();
    QSqlQuery* Create_query = new QSqlQuery(db);
    QString pre_Create("CREATE TABLE IF NOT EXISTS USERS("
        "[UserName] integer primary key not null,"
        "[Name] nvarchar not null,"
        "[Password] nvarchar not null,"
        "[Situation] tinyint not null);");
    Create_query->prepare(pre_Create);
    if (!Create_query->exec())
        qDebug() << "create table error: " << Create_query->lastError();
}

void mainwindow::edit_user_database(QString former_username, QString NewUsername, QString Name, QString Password, int situation) {
    QSqlQuery* edit_query = new QSqlQuery(db);
    QString pre_edit = QString("UPDATE ") + " USERS " + "SET " + "[UserName]=" + QString("'%1',").arg(NewUsername) + "[Name]=" + QString("'%1',").arg(Name) + "[Password]=" + QString("'%1',").arg(Password) + "[Situation]=" + QString("%1 ").arg(QString::number(situation)) + "Where " + "[UserName]=" + QString("'%1'").arg(former_username);
    edit_query->prepare(pre_edit);
    if (!edit_query->exec())
        qDebug() << "edit user sqllite error: " << edit_query->lastError();
}

void mainwindow::add_user_database(QString Username, QString Name, QString Password, int situation) {
    QSqlQuery* add_query = new QSqlQuery(db);
    QString pre_add = QString("insert into") + " USERS" + "([UserName],[Name],[Password],[Situation])" + "values(" + QString("'%1',").arg(Username) + QString("'%1',").arg(Name) + QString("'%1',").arg(Password) + QString("%1").arg(QString::number(situation)) + ")";
    add_query->prepare(pre_add);
    if (!add_query->exec())
        qDebug() << "add user to table sqllite error: " << add_query->lastError();
    else
        MembersNumber++;
}

void mainwindow::remove_user_database(QString Username) {
    QSqlQuery* remove_query = new QSqlQuery(db);
    QString pre_remove = QString("DELETE ") + "From " + "USERS " + "Where " + "[UserName]=" + QString("'%1'").arg(Username);
    remove_query->prepare(pre_remove);
    if (!remove_query->exec())
        qDebug() << "remove user sqllite error: " << remove_query->lastError();
    else
        MembersNumber--;
}

void mainwindow::show_database() {
    QSqlQuery* show_query = new QSqlQuery(db);
    QString pre_show = QString("Select * From ") + "USERS";
    show_query->prepare(pre_show);
    if (!show_query->exec())
        qDebug() << "show users sqllite error: " << show_query->lastError();

    QSqlQueryModel* model1 = new QSqlQueryModel(this);
    model1->setQuery(*show_query);
    ui.tableView->setModel(model1);

    /*+*/ 
    for (int i = 0; i < ui.tableView->model()->rowCount(); i++) {
        if (sockets[i] == nullptr)
            sockets[i] = new QTcpSocket(this);

        map_socketUsername.insert(i, ui.tableView->model()->data(ui.tableView->model()->index(i, 0)).toString());
    }
}

void mainwindow::on_Edit_Button_clicked() {
    QItemSelectionModel* select = ui.tableView->selectionModel();
    int value = ui.tableView->model()->data(ui.tableView->model()->index(select->currentIndex().row(), 0)).toInt();
    QSqlQuery* sqledit = new QSqlQuery(db);
    QString pre_edit = QString("select * From USERS Where [UserName]=") + QString("'%1'").arg(QString::number(value));
    now_username = value;
    sqledit->prepare(pre_edit);
    sqledit->exec();
    while (sqledit->next())
    {
        ui.Set_username->setText(sqledit->value(0).toString());
        ui.Set_name->setText(sqledit->value(1).toString());
        ui.Set_password->setText(sqledit->value(2).toString());
        switch (sqledit->value(3).toInt())
        {
        case 0: ui.radioButton->setChecked(true); break;
        case 1: ui.radioButton_2->setChecked(true); break;
        case 2: ui.radioButton_3->setChecked(true); break;
        }
    }
    if (ui.addEdit_Button->text() == "Edit") {
        clear_addEdit();
    }
    else if (ui.addEdit_Button->text() == "Add")
        ui.addEdit_Button->setText("Edit");
}

void mainwindow::on_Remove_Button_clicked() {
    QItemSelectionModel* select = ui.tableView->selectionModel();
    int value = ui.tableView->model()->data(ui.tableView->model()->index(select->currentIndex().row(), 0)).toInt();
    remove_user_database(QString::number(value));
    show_database();
}

void mainwindow::on_addEdit_Button_clicked() {
    if (ui.addEdit_Button->text() == "Edit") {
        int value1;
        if (ui.radioButton->isChecked())
            value1 = 0;
        else if (ui.radioButton_2->isChecked())
            value1 = 1;
        else if (ui.radioButton_3->isChecked())
            value1 = 2;
        edit_user_database(QString::number(now_username), ui.Set_username->text(), ui.Set_name->text(), ui.Set_password->text(), value1);
        show_database();
        clear_addEdit();
        ui.addEdit_Button->setText("Add");

    }

    else if (ui.addEdit_Button->text() == "Add") {
        int value1;
        if (ui.radioButton->isChecked())
            value1 = 0;
        else if (ui.radioButton_2->isChecked())
            value1 = 1;
        else if (ui.radioButton_3->isChecked())
            value1 = 2;
        add_user_database(ui.Set_username->text(), ui.Set_name->text(), ui.Set_password->text(), value1);
        clear_addEdit();
        show_database();
    }


}

void mainwindow::clear_addEdit() {
    ui.addEdit_Button->setText("Add");
    ui.Set_username->setText("");
    ui.Set_name->setText("");
    ui.Set_password->setText("");
    ui.radioButton->setChecked(false);
    ui.radioButton_2->setChecked(false);
    ui.radioButton_3->setChecked(false);
}


void mainwindow::listenToport1() {
    server1 = new QTcpServer(this);
    server1->listen(QHostAddress::Any, 23245);
    connect(server1, SIGNAL(newConnection()), this, SLOT(newconnection1()));


}

void mainwindow::newconnection1() {
    temp = server1->nextPendingConnection();

    connect(temp, &QTcpSocket::readyRead, this, &mainwindow::inistalRead);
}

void mainwindow::inistalRead() {
    QString Data;
    QByteArray data(temp->readAll());
    if (data.at(0) == '0')
    {
        if (data.at(1) == ' ' && data.at(2) == ' ')
        {
            Data.insert(0, data);
        }
    }
    //
    else if (data.at(0) == '~')
    {
        if (data.at(1) == ' ' && data.at(2) == ' ')
            resivesocketFile = temp;
        recivefile(data);
    }
    else {
        Data.insert(0, " ");
    }
        int usernamelen = Data.lastIndexOf(QString("  passi=")) - (Data.lastIndexOf(QString(" username =")) + 12);
        int Passwordlen = Data.length() - (Data.lastIndexOf(QString("  passi=")) + 8);

        QString username = Data.mid(Data.lastIndexOf(QString(" username =")) + 11, usernamelen);
        QString Password = Data.mid(Data.lastIndexOf(QString("  passi=")) + 8, Passwordlen);

        
        for (int i = 0; i < ui.tableView->model()->rowCount(); i++) {
            if ((ui.tableView->model()->data(ui.tableView->model()->index(i, 0)).toString() == username))
                if (ui.tableView->model()->data(ui.tableView->model()->index(i, 2)).toString() == Password)
                {
                    /*for azi*/        QString payaam = "##$$##" + QString("you Entered") + "##@@##";
                    /*for azi*/        QByteArray bit12(payaam.toUtf8());
                    /*for azi*/        temp->write(bit12);
                    temp->waitForBytesWritten(50000);

                    if (ui.tableView->model()->data(ui.tableView->model()->index(i, 3)).toString() == "1")
                    {
                        /*for azi*/             QByteArray bit13 = QString("##$$##" + QString("+  all Professor feature") + "##@@##").toUtf8();
                        /*for azi*/             temp->write(bit13);
                    }
                    //badan send list of attendees
                    int socket_ghabli = checkattendence(username);
                    if (socket_ghabli != -1)
                    {
                        //tekrari
                        temp->write("##$$##2  ##@@##");
                        temp->waitForBytesWritten(100);
                        //sockets1.at(socketlist1.at(id_ghabli))->deleteLater();

                        for (int i = 0; i < numberSockets; i++)
                        {
                            sockets[socket_ghabli] = temp;
                        }
                    }
                    else
                    {
                        //new connected
                        int number1=-1;
                        for (int j = 0; j < ui.tableView->model()->rowCount(); j++) {
                            if (map_socketUsername[j] == username) {
                                sockets[j] = temp;
                                number1 = j;
                                break;
                            }
                        }
                        //problem
                        connect(sockets[number1], &QTcpSocket::readyRead, this, [&, number1]()->void {ReadData(number1); });
                        connect(sockets[number1], &QTcpSocket::disconnected, this, [&, number1]()->void {disconnectSocket1(number1); });
                        
                            QTimer::singleShot(100, [&]() {sendAttendence(); });
                        numberSockets++;
                        Addencesocket << number1;
                    }
                    




                temp->write(QByteArray(chats.toUtf8()));
                temp->write(QByteArray(textEdit1.toUtf8()));
                temp->write(QByteArray(classname.toUtf8()));
                }
        }


     
    

        

    disconnect(temp, &QTcpSocket::readyRead, this, &mainwindow::inistalRead);
    temp = nullptr;
}

void mainwindow::ReadData(int number1) {




    QString Data("");
    QByteArray data(sockets[number1]->readAll());
    qDebug() << "data:" << data;



    if (data.at(0) == '1' || data.at(0) == 'c')
    {
        if (data.at(1) == ' ' && data.at(2) == ' ')
        {
            Data.insert(0, data);
        }
    }
    else {
        Data.insert(0, " ");
    }

   
    if (Data.at(0) == '1')
    {
        //chat ha
        QString Textmessage = Data.section("  ", 2);
        qDebug() << Data;
        QString userwithspass = Data.section("  ", 1).remove(Data.section("  ", 2));
        QString username = userwithspass.remove("  ");
        username.remove("username**/=");


        qDebug() << UsernameToName(username) + ": " + Textmessage;
        QString payam = QString("1  ") + UsernameToName(username) + ": " + Textmessage;

        /*for azi*/    payam = "##$$##" + payam + "##@@##";
        chats.append(payam);


        for (int i = 0; i < Addencesocket.count(); i++) {
            sockets[Addencesocket.at(i)]->write(QByteArray(payam.toUtf8()));
        }

    }
    qDebug() << "bedoon 3333";
    if (Data.at(0) == 'c') {


        /*for azi*/     QString paaayam = "##$$##" + Data + "##@@##";
        textEdit1 = paaayam;

        for (int i = 0; i < Addencesocket.count(); i++)
            if (sockets[Addencesocket.at(i)] != sender()) {
                /*for azi*/     sockets[Addencesocket.at(i)]->write(QByteArray(paaayam.toUtf8()));

                //                sockets1.at(socketlist1.at(i))->write(QByteArray(Data.toUtf8()));
            }
    }


    if (data.at(0) == '4')
    {
        qDebug() << "+33333";
        if (data.at(1) == ' ' && data.at(2) == ' ' && data.at(3) == '*' && data.at(4) == '*' && data.at(5) == '*')
        {
            qDebug() << "seda";
            QString azi(data);
            azi.remove(0, 5);

            for (int i = 0; i < azi.length(); i++)
            {
                if (azi[i] == '0' || azi[i] == '1' || azi[i] == 'c')
                    if (azi[i + 1] == ' ' && azi[i + 2] == ' ')
                        qDebug() << "eshkal";
            }


            for (int i = 0; i < Addencesocket.count(); i++)
                if (sockets[Addencesocket.at(i)] != sender()) {


                    /*for azi*/  //   QString payyyam = "##$$##" + data + "##@@##";
                    /*for azi*/  //   sockets1.at(socketlist1.at(i))->write(QByteArray(payyyam.toUtf8()));
                    sockets[Addencesocket.at(i)]->write(data);
                }


        }
    }


    if (data.at(0) == 'S') {
        //situtaion
        if (data.at(1) == ' ' && data.at(2) == ' ' && data.at(3) == 's' && data.at(4) == 'i' && data.at(5) == 't' && data.at(6) == '=')

            changeAttenceSituation(number1, data.at(7));

    }






}


int mainwindow::checkattendence(QString username) {
    int lastusername = -1;
    for(int i=0;i<Addencesocket.count();i++)
    if (map_socketUsername[Addencesocket[i]] == username)
    {
        lastusername = i;
    }
    return lastusername;
}

void mainwindow::disconnectSocket1(int connectionNumber2) {
    for (int i = 0; i < Addencesocket.count(); i++)
    {
        if (Addencesocket.at(i) == connectionNumber2)
        {
            Addencesocket.removeAt(i);
            
        }
    }
    sockets[connectionNumber2] = nullptr;
    sendAttendence();

}

void mainwindow::sendAttendence()
{
    QString payam2("");

    foreach(int numbersocket1, Addencesocket) {
        payam2 = payam2 + map_socketUsername[numbersocket1] + "==" + UsernameToName(map_socketUsername[numbersocket1]) + "///";
    }
    payam2.remove(payam2.length() - 3, payam2.length());
    payam2 = "L  " + payam2;
    /*for azi*/     payam2 = "##$$##" + payam2 + "##@@##";
    for (int i = 0; i < Addencesocket.count(); i++) {

        //koliAttendence = payam2;
        sockets[Addencesocket[i]]->write(QByteArray(payam2.toUtf8()));
        //sockets1.at(socketlist1.at(i))->waitForBytesWritten(100);
    }
}

QString mainwindow::UsernameToName(QString username) {
    QSqlQuery* change_query = new QSqlQuery(mainwindow::db);
    QString pre_query = QString("select [Name] From USERS Where[UserName] = ") + username;
    change_query->prepare(pre_query);
    change_query->exec();
    change_query->next();
    QString temp = change_query->value(0).toString();
    return temp;

}

void mainwindow::changeAttenceSituation(int number1, char s) {
    QString payam2("");

    // foreach(QString ttt, Attendence) {
    payam2 = map_socketUsername[number1] + "..." + s;
    // }
    payam2 = "S  " + payam2;
    payam2 = "##$$##" + payam2 + "##@@##";
    for (int i = 0; i < Addencesocket.count(); i++) {
        sockets[Addencesocket.at(i)]->write(QByteArray(payam2.toUtf8()));
    }
}

void mainwindow::editnameoflesson() {
    QString payam = ui.lineEdit->text();
    payam = "N  " + payam;
    payam = "##$$##" + payam + "##@@##";
    classname = payam;
    for (int i = 0; i < Addencesocket.count(); i++) {
        sockets[Addencesocket.at(i)]->write(QByteArray(payam.toUtf8()));
    }
}

void mainwindow::recivefile(QByteArray reciveed) {

    connect(resivesocketFile, &QTcpSocket::readyRead, this, [&]()->void {recivefilefromClient(); });
    connect(resivesocketFile, &QTcpSocket::disconnected, this, [&]()->void {finishedrecive(0); });
    //temp = nullptr;
    recivefilefromClient(reciveed);
    finishsocket = 0;
}

void mainwindow::recivefilefromClient(QByteArray reciveed) {
    QByteArray reci;
    bool flagend;
    if (reciveed != "") {
        qDebug() << "no";
        //qDebug() << reciveed;
        reciveed.remove(0, 4);
        int j;
        for (int i = 0; reciveed.at(i) != '*'; i++)
        {
            j = i+1;
        }
        qDebug() << "j=" << j;
        int numberpage = reciveed.mid(0, j).toInt();
        pdf[0].numberpage = numberpage;
        qDebug() << "numberpage=" << numberpage;
        reciveed.remove(0, j + 1);
        for (int i = 0; reciveed.at(i) != '^'; i++)
        {
            j = i + 1;
        }
        //qDebug() << "recive=" << reciveed << endl << endl << endl;
        QString name= reciveed.mid(0, j);
        pdf[0].name = name;
       //qDebug() << "name=" << name;
        reciveed.remove(0, j + 1);
        //qDebug() << "recive=" << reciveed << endl << endl << endl;
        allfile.append(reciveed);
    }
    else if(reciveed == "" && resivesocketFile->bytesAvailable()>0){
        reci=resivesocketFile->readAll();
        //qDebug() << reci;
        allfile.append(reci);
        finishsocket = 0;
       
    }
    else if (reciveed == "" && resivesocketFile->bytesAvailable() == 0)
    {
        if (finishsocket < 10) {

            finishsocket++;
            goto G;
            
        }
        else
        {
            /*
            qDebug() << "tamam shod";
            qDebug() << allfile;
            for (int i = 0;i < pdf[0].numberpage; i++) {
                qint64 moghe = allfile.indexOf("&&&&");
                //qDebug() <<"mogh="<<moghe;
                pdf[0].pages[i] = allfile.mid(0, moghe);
                allfile.remove(0, moghe + 4);
            }
            qDebug() << "fin";
            //disconnect(resivesocketFile, &QTcpSocket::readyRead, this, &mainwindow::recivefilefromClient);
            //resivesocketFile = nullptr;
            sendpdftoClient(0, 0);
            return;
            */
        }
    }
    if (false)
    {
    G:
        _sleep(100);
        recivefilefromClient();
    }
}

void mainwindow::finishedrecive(int pdfnumber)
{
    qDebug() << "tamam shod";
    //qDebug() << allfile;
    for (int i = 0;/*allfile.length()<100*/i < pdf[0].numberpage; i++) {
        qint64 moghe = allfile.indexOf("&&&&");
        //qDebug() <<"mogh="<<moghe;
        pdf[0].pages[i] = allfile.mid(0, moghe);
        allfile.remove(0, moghe + 4);
    }
    qDebug() << "fin";
    //disconnect(resivesocketFile, &QTcpSocket::readyRead, this, &mainwindow::recivefilefromClient);
    //resivesocketFile = nullptr;
    sendpdftoClient(0, 0);


}




void mainwindow::sendpdftoClient(int pdfnumber,int page,QString username)
{
    if(username ==""){
        qDebug() << "yoooooooooooooooo";
        for (int i = 0; i < Addencesocket.count(); i++) {
            /*QString payam = "~  page=" + QString("%1").number(page) + "," + QString(pdf[pdfnumber].pages[page]);
            qDebug() << "payam=" << pdf[pdfnumber].pages[page];
            payam = "##$$##" + payam + "##@@##";
            QByteArray pp = QString("##$$##").toUtf8()+QString("~  page=").toUtf8() + QString(QString("%1").number(page)).toUtf8() + QString(",").toUtf8() + pdf[pdfnumber].pages[page] + QString("##@@##").toUtf8();
            //sockets[Addencesocket.at(i)]->write(QByteArray(payam.toUtf8()));
            sockets[Addencesocket.at(i)]->write(pp,1024);
            //sockets[Addencesocket.at(i)]->flush();
            */



            QByteArray movaghati = pdf[pdfnumber].pages[page];
            QByteArray payam123= QString("##$$##").toUtf8() + QString("~  page=").toUtf8() + QString(QString("%1").number(page)).toUtf8() + QString(",").toUtf8() + movaghati.mid(0, 256) + QString("##@@##").toUtf8();
            sockets[Addencesocket.at(i)]->write(payam123);
            movaghati.remove(0, 256);
            while(!movaghati.isEmpty())
            {
                payam123= QString("##$$##").toUtf8() +QString("~  conti").toUtf8()+ movaghati.mid(0, 256)+ QString("##@@##").toUtf8();
                sockets[Addencesocket.at(i)]->write(payam123);
                movaghati.remove(0, 1024);
            }
            payam123 = QString("##$$##").toUtf8() + QString("~  conti").toUtf8() + "***" + QString("##@@##").toUtf8();
            sockets[Addencesocket.at(i)]->write(payam123);



        }
    }
    else
    {
        //sockets[Addencesocket.at(i)]->write(QByteArray(payam.toUtf8()));
        
    }
}
