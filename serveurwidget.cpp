#include "serveurwidget.h"
#include "ui_serveurwidget.h"

serveurWidget::serveurWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::serveurWidget)
    ,socketEcouteServeur(new QTcpServer)
{
    ui->setupUi(this);
    if( !socketEcouteServeur->listen(QHostAddress::Any,ui->spinBoxnbPort->value())){
        ui->listWidget->addItem("serveur sourd");
    }else{
        ui->listWidget->addItem("serveur en écoute");
        connect(socketEcouteServeur,&QTcpServer::newConnection,this,&serveurWidget::onQtcpServer_newConnection);

    }

}

serveurWidget::~serveurWidget()
{
    delete ui;
    delete socketEcouteServeur;
}


void serveurWidget::on_pushButton_serveur_clicked()
{

}

void serveurWidget::onQtcpServer_newConnection()
{
    QTcpSocket *client;
    client=socketEcouteServeur->nextPendingConnection();
    connect(client,&QTcpSocket::readyRead,this,&serveurWidget::onQTcpSocket_readyRead);
    socketDialogueClient.append(client);
    QProcess *leProcess;
    leProcess = new QProcess;
    connect(leProcess,&QProcess::readyReadStandardOutput,this,&serveurWidget::onQProcess_readyReadStandardOutput);
    connect(client,&QTcpSocket::disconnected,this,&serveurWidget::onQtcpSocket_disconneted);
    process.append(leProcess);
    ui->listWidget->addItem("connection d'un client");

}

void serveurWidget::onQTcpSocket_readyRead()
{
    ui->listWidget->addItem("reception");

    QTcpSocket*client=qobject_cast<QTcpSocket*>(sender());
    int indexProcess=socketDialogueClient.indexOf(client);


    QChar commande;

    if(socketDialogueClient.at(indexProcess)->bytesAvailable())
    {
        QByteArray tmp=socketDialogueClient.at(indexProcess)->readAll();

        commande = tmp.at(0);

        QString messageCommande = "commande recue :"+ tmp +"";
        ui->listWidget->addItem(messageCommande);

        QString messageReponce = "Réponce envoyée : ";
        QString reponse;

        QHostAddress adrresse=socketDialogueClient.at(indexProcess)->peerAddress();

        switch (commande.toLatin1()) {
        case 'u':
            reponse=getenv("USER");
            ui->listWidget->addItem("client de l'adresse : "+adrresse.toString());
            socketDialogueClient.at(indexProcess)->write(reponse.toUtf8());
            ui->listWidget->addItem(messageReponce += reponse);

            break;
        case 'c':
            reponse=QSysInfo::machineHostName();
            ui->listWidget->addItem("client de l'adresse : "+adrresse.toString());
            socketDialogueClient.at(indexProcess)->write(reponse.toUtf8());
            ui->listWidget->addItem(messageReponce += reponse);
            break;
        case 'o':
            reponse=QSysInfo::kernelType();
            ui->listWidget->addItem("client de l'adresse : "+adrresse.toString());
            socketDialogueClient.at(indexProcess)->write(reponse.toUtf8());
            ui->listWidget->addItem(messageReponce += reponse);


            break;
        case 'a':
            reponse=QSysInfo::currentCpuArchitecture();
            ui->listWidget->addItem("client de l'adresse : "+adrresse.toString());
            socketDialogueClient.at(indexProcess)->write(reponse.toUtf8());
            ui->listWidget->addItem(messageReponce += reponse);
            break;
        default:
            break;
        }

    }
}

void serveurWidget::onQtcpSocket_disconneted()
{

    QTcpSocket*client=qobject_cast<QTcpSocket*>(sender());
    int indexProcess=socketDialogueClient.indexOf(client);
    QString adrresse = socketDialogueClient.at(indexProcess)->peerAddress().toString();
     ui->listWidget->addItem("déconnexion client : "+adrresse);
    socketDialogueClient.removeAt(indexProcess);

}

void serveurWidget::onQProcess_readyReadStandardOutput()
{
    QTcpSocket*client=qobject_cast<QTcpSocket*>(sender());
    int indexProcess=socketDialogueClient.indexOf(client);

    QString reponse= process.at(indexProcess)->readAllStandardOutput();
    if(!reponse.isEmpty()){
        QString message="Réponse envoyée à"  +  socketDialogueClient.at(indexProcess)->peerAddress().toString()+":"+reponse;
        ui->listWidget->addItem(message);
        socketDialogueClient.at(indexProcess)->write(reponse.toLatin1());
    }
}
