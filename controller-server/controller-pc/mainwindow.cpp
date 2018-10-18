/*
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain this list of conditions
 *    and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE SOFTWARE PROVIDER OR DEVELOPERS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sdn-addr.h"
#include "sdn-send-packet.h"
#include "sdn-serial-send.h"
#include <QProcess>
#include <QDebug>
#include <QElapsedTimer>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
//#include "ui_powerwin.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   // ui->tabWidget->addTab();
    /*This function shows the time*/
    timer = new QTimer(this);
    connect (timer, SIGNAL(timeout()),SLOT(myclock()));
    timer->start(1000);//time here is in miliseconds so in this case the output will be printed every second
    ui->ipAddress->setText("127.0.0.1");
    connectionTimer.start();
    getIdTimer = new QTimer(this);
    connect(getIdTimer, SIGNAL(timeout()), this, SLOT(on_getNodeButton_clicked()));

    connect(&serialConnector, &SerialConnector::state_changed_stringfy, [this](QString text){
        connectionTimer.restart();
        ui->statusLabel->setText(text);
        ui->statusLabel_2->setText(text);
        ui->connectButton->setEnabled(!serialConnector.ready());
        ui->disconnectButton->setEnabled(serialConnector.ready());
    });

    connect(&serialConnector, &SerialConnector::printfString, [this](QString text) {
        ui->terminalTextBox->append(QString("%1(ms): %2").arg(connectionTimer.elapsed()).arg(text));
    });

    connect(&serialConnector, &SerialConnector::nodeIdReceived, this, &MainWindow::processNodeIdMessage);
    connect(&serialConnector, &SerialConnector::defaultPacket, this, &MainWindow::decodePacket);

//    connect(&serialConnector, &SerialConnector::ackReceived, [this](){
//         ui->terminalTextBox->append(QString("Received Serial ACK")); //TODO something
//    });

//    connect(&serialConnector, &SerialConnector::nackReceived, [this](){
//        ui->terminalTextBox->append(QString("Received Serial NACK")); //TODO something
//    });

    connect(&serialConnector, &SerialConnector::ackReceived, this, &sdn_send_done);

    connect(&serialConnector, &SerialConnector::nackReceived, this, &sdn_send_done);
   /*The following enables sample rate to count in real time*/
    timecount = new QTimer(this);
    connect (timecount, SIGNAL(timeout()),SLOT(on_pushButton_update_clicked()));
    timecount->start(ui->sample_rate->value());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::decodePacket(QByteArray packet)
{
//    qDebug() << "new SDN PACKET received";
    sdn_serial_packet_t *packet_received = (sdn_serial_packet_t*)packet.data();
    controller_receive((uint8_t *)packet_received->payload, (uint16_t)packet_received->header.payload_len);
}

void MainWindow::on_connectButton_clicked()
{
    if(ui->radioButton_ip->isChecked()) {
        serialConnector.initConnection(ui->ipAddress->text(), (quint16)ui->portNumber->value());
    } else {
        serialConnector.initConnection(ui->ipAddress->text());
    }
    getIdTimer->start(1000);
}

void MainWindow::on_disconnectButton_clicked()
{
    serialConnector.close();
}

//void MainWindow::on_clsButton_clicked()
//{
//    ui->terminalTextBox->clear();
//}

void MainWindow::on_getNodeButton_clicked()
{
  serialConnector.requestNodeId();

}

void MainWindow::processNodeIdMessage(sdnaddr_t *nodeId)
{
    if(!initDone) {
        memcpy(controller_addr.u8, nodeId, SDNADDR_SIZE);
        controller_init(&controller_addr);
        initDone = true;
        ui->terminalTextBox->setText(QString("Executed controller_init()"));
    }
    getIdTimer->stop();
    ui->terminalTextBox->append(QString("%1(ms): NODE ID REQUEST ANSWER: %2").
                                arg(connectionTimer.elapsed()).arg(*((int *)nodeId)));
}

void MainWindow::sendPacket(unsigned char* packet, unsigned short packet_len, sdnaddr_t to) {
    sdn_serial_packet_t serial_packet;
    serial_packet.header.msg_type = SDN_SERIAL_MSG_TYPE_RADIO;
    serial_packet.header.payload_len = packet_len;
    sdnaddr_copy(&serial_packet.header.node_addr, &to);
    memcpy(serial_packet.payload, packet, packet_len);
    //ui->textEdit->setText(serial_packet);
//     unsigned int i;
//     for (i=0; i < serial_packet.header.payload_len; i++) {

//         printf("%02X ", serial_packet.payload[i]);
//     }
//     printf("\n");
    serialConnector.sendGenericPacket(&serial_packet);
}

void MainWindow::on_radioButton_serial_clicked(bool checked)
{
    if (checked) {
        ui->ipAddress->setText("/dev/ttyUSB0");
    }

}

void MainWindow::on_actionSave_Statistics_As_triggered()
{
    QString filter ="All files (*.*);; Text Files (*.txt);; cpp Files (*.cpp)";
    QString file_newpath =QFileDialog::getSaveFileName(this,"Choose a File", QDir::homePath(),filter);//This obtains the file path
    QFile file(file_newpath);// we declare an instance of QFile and pass the new file path to it
    QString file_path = file_newpath; //here we update the file_path with the new file path
    if (!file.open(QFile::WriteOnly | QFile::Text)) { /*If the file is not open do this*/

        QMessageBox::warning(this,"..","File is not open");
        return; // returns void and exits the program
    }
    QTextStream out(&file);//gives reference to the address location
    QString text = ui->terminalTextBox->toPlainText();//copies the text edit content in the ui to text variable
    out<< text;//then we pass the text to the output
    file.flush();//we flush the file
    file.close();//Once done the file is closed
}

void MainWindow::on_actionAbout_triggered()
{
    QString about_text;
     //about_text="Author: Musa Ndiaye\n";
     about_text+="Version 0.11\n"; //The += allows appending to the about_text string variable
     about_text+="Copyright 2017" ;
    QMessageBox::about(this,"Management Service UI",about_text);
}



void MainWindow::on_pushButton_update_clicked()
{
    QTime time = QTime::currentTime();
    QString time_text = time.toString("hh : mm : ss ");
    ui->terminalTextBox->append("Time Stamp: " + time_text + "\n");
    QProcess *process = new QProcess;
    process->start( "python", QStringList()<< file_path1 << file_path2);
    process->waitForFinished();
    QString text=process->readAllStandardOutput();
    ui->terminalTextBox->append(text);
    timecount->start(ui->sample_rate->value());
}

void MainWindow::on_pushButton_load_clicked()
{
    QString Pythonfilter ="Python Files (*.py)";
    QString filter ="Text Files (*.txt)";
    file_path1 =QFileDialog::getOpenFileName(this,"Choose a statistics script", QDir::homePath(),Pythonfilter);//This obtains the file path
    file_path2 =QFileDialog::getOpenFileName(this,"Choose a log file", QDir::homePath(),filter);//This obtains the file path>
    QProcess *process = new QProcess;
    process->start( "python", QStringList()<< file_path1 << file_path2);
    process->waitForFinished();
    QString text=process->readAllStandardOutput();
    ui->terminalTextBox->append(text);
}

void MainWindow::on_clrbrowser_clicked()
{
     ui->terminalTextBox->clear();
}

void MainWindow::myclock()
{
   //qDebug()<< "update..";
    QTime time = QTime::currentTime();
    QString time_text = time.toString("hh : mm : ss "); //converts the time to string in the format shown
    /*The following code creates the illusion of a flashing timer*/
//    if ((time.second() % 2) == 0){
//        time_text[3] =' ';
//       time_text[8] =' ';
//   }
ui->time_label->setText(time_text);
}

//void MainWindow::on_pushButton_clicked()
//{
//    Power = new Powerwin(this); //pointer= new class()
//    Power->show();
//}

void MainWindow::on_pushButton_2_clicked()
{
    QString Pythonfilter ="Python Files (*.py)";
    QString filter ="Text Files (*.txt)";
    file_path3 =QFileDialog::getOpenFileName(this,"Choose a power script", QDir::homePath(),Pythonfilter);//This obtains the file path
    file_path4 =QFileDialog::getOpenFileName(this,"Choose energest file", QDir::homePath(),filter);//This obtains the file path>
    QProcess *process = new QProcess;
    process->start( "python", QStringList()<<file_path3<<file_path4);
    process->waitForStarted();
}




void MainWindow::on_traffic_clicked()
{
    QString Pythonfilter ="Python Files (*.py)";
    QString filter ="Text Files (*.txt)";
    file_path5 =QFileDialog::getOpenFileName(this,"Choose a traffic script", QDir::homePath(),Pythonfilter);//This obtains the file path
    file_path6 =QFileDialog::getOpenFileName(this,"Choose traffic statistics file", QDir::homePath(),filter);//This obtains the file path>
    QProcess *process = new QProcess;
    process->start( "python", QStringList()<<file_path5<<file_path6);
    process->waitForStarted();
}
