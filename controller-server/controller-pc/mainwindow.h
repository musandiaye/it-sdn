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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QElapsedTimer>

#include "sdn-serial-packet.h"
#include "serialconnector.h"
#include "controller-core.h"
#include "sdn-addr.h"
#include <powerwin.h>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void processByte(unsigned char data);
    void frameDecode(const char *framebuffer, uint8_t frame_length);
    //void sendPacket(sdn_serial_packet_t *packet);
    void sendPacket(unsigned char* packet, unsigned short packet_len, sdnaddr_t to);

private slots:

    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void decodePacket(QByteArray packet);
    void on_getNodeButton_clicked();
    void processNodeIdMessage(sdnaddr_t *nodeId);

    void on_radioButton_serial_clicked(bool checked);

    void on_ipAddress_cursorPositionChanged(int arg1, int arg2);

    void on_actionSave_Statistics_As_triggered();

    void on_actionAbout_triggered();

    void on_pushButton_update_clicked();

    void on_pushButton_load_clicked();

    void on_clrbrowser_clicked();
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_sample_rate_editingFinished();

    void on_traffic_clicked();

public slots:
    void myclock();
signals:

    void new_packet_available();

private:

    Ui::MainWindow *ui;

    SerialConnector serialConnector;
    QElapsedTimer connectionTimer;
    QTimer *getIdTimer;

    sdnaddr_t controller_addr;// = (sdnaddr_t) {{ 1, 0 }};

    bool initDone=false;
    QString file_path;
    QString file_path1;
    QString file_path2;
    QString file_path3;
    QString file_path4;
    QString file_path5;
    QString file_path6;
    QTimer *timer;//declare an object for the qtimer class
    QTimer *timecount;//uses actual time to count for refresh rate
    Powerwin *Power;//creates an instance of Powerwin

};

#endif // MAINWINDOW_H
