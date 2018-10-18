/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSave_Statistics_As;
    QAction *actionAbout;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QRadioButton *radioButton_ip;
    QRadioButton *radioButton_serial;
    QLineEdit *ipAddress;
    QLabel *label;
    QSpinBox *portNumber;
    QSpacerItem *horizontalSpacer;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QHBoxLayout *horizontalLayout_3;
    QLabel *statusLabel_2;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_2;
    QLabel *statusLabel;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_4;
    QLineEdit *time_label;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_3;
    QPushButton *getNodeButton;
    QPushButton *pushButton_load;
    QPushButton *pushButton_update;
    QPushButton *pushButton;
    QPushButton *clrbrowser;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QSpinBox *sample_rate;
    QTextBrowser *terminalTextBox;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *verticalLayout_5;
    QPushButton *pushButton_2;
    QPushButton *traffic;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(965, 502);
        actionSave_Statistics_As = new QAction(MainWindow);
        actionSave_Statistics_As->setObjectName(QStringLiteral("actionSave_Statistics_As"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        radioButton_ip = new QRadioButton(tab);
        radioButton_ip->setObjectName(QStringLiteral("radioButton_ip"));
        radioButton_ip->setChecked(true);

        horizontalLayout->addWidget(radioButton_ip);

        radioButton_serial = new QRadioButton(tab);
        radioButton_serial->setObjectName(QStringLiteral("radioButton_serial"));

        horizontalLayout->addWidget(radioButton_serial);

        ipAddress = new QLineEdit(tab);
        ipAddress->setObjectName(QStringLiteral("ipAddress"));

        horizontalLayout->addWidget(ipAddress);

        label = new QLabel(tab);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        portNumber = new QSpinBox(tab);
        portNumber->setObjectName(QStringLiteral("portNumber"));
        portNumber->setMaximum(65000);
        portNumber->setValue(60001);

        horizontalLayout->addWidget(portNumber);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        connectButton = new QPushButton(tab);
        connectButton->setObjectName(QStringLiteral("connectButton"));

        horizontalLayout->addWidget(connectButton);

        disconnectButton = new QPushButton(tab);
        disconnectButton->setObjectName(QStringLiteral("disconnectButton"));
        disconnectButton->setEnabled(false);

        horizontalLayout->addWidget(disconnectButton);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        statusLabel_2 = new QLabel(tab);
        statusLabel_2->setObjectName(QStringLiteral("statusLabel_2"));

        horizontalLayout_3->addWidget(statusLabel_2);


        verticalLayout_2->addLayout(horizontalLayout_3);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_4 = new QVBoxLayout(tab_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_2 = new QLabel(tab_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_4->addWidget(label_2);

        statusLabel = new QLabel(tab_2);
        statusLabel->setObjectName(QStringLiteral("statusLabel"));

        horizontalLayout_4->addWidget(statusLabel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        label_4 = new QLabel(tab_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_4->addWidget(label_4);

        time_label = new QLineEdit(tab_2);
        time_label->setObjectName(QStringLiteral("time_label"));

        horizontalLayout_4->addWidget(time_label);


        verticalLayout_4->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        getNodeButton = new QPushButton(tab_2);
        getNodeButton->setObjectName(QStringLiteral("getNodeButton"));

        verticalLayout_3->addWidget(getNodeButton);

        pushButton_load = new QPushButton(tab_2);
        pushButton_load->setObjectName(QStringLiteral("pushButton_load"));

        verticalLayout_3->addWidget(pushButton_load);

        pushButton_update = new QPushButton(tab_2);
        pushButton_update->setObjectName(QStringLiteral("pushButton_update"));

        verticalLayout_3->addWidget(pushButton_update);

        pushButton = new QPushButton(tab_2);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout_3->addWidget(pushButton);

        clrbrowser = new QPushButton(tab_2);
        clrbrowser->setObjectName(QStringLiteral("clrbrowser"));

        verticalLayout_3->addWidget(clrbrowser);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(tab_2);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_2->addWidget(label_3);

        sample_rate = new QSpinBox(tab_2);
        sample_rate->setObjectName(QStringLiteral("sample_rate"));
        sample_rate->setMaximum(3600000);
        sample_rate->setSingleStep(5000);
        sample_rate->setValue(30000);

        horizontalLayout_2->addWidget(sample_rate);


        verticalLayout_3->addLayout(horizontalLayout_2);


        horizontalLayout_5->addLayout(verticalLayout_3);

        terminalTextBox = new QTextBrowser(tab_2);
        terminalTextBox->setObjectName(QStringLiteral("terminalTextBox"));
        terminalTextBox->setReadOnly(true);

        horizontalLayout_5->addWidget(terminalTextBox);


        verticalLayout_4->addLayout(horizontalLayout_5);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        verticalLayout_6 = new QVBoxLayout(tab_3);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        pushButton_2 = new QPushButton(tab_3);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        verticalLayout_5->addWidget(pushButton_2);

        traffic = new QPushButton(tab_3);
        traffic->setObjectName(QStringLiteral("traffic"));

        verticalLayout_5->addWidget(traffic);


        horizontalLayout_6->addLayout(verticalLayout_5);


        verticalLayout_6->addLayout(horizontalLayout_6);

        tabWidget->addTab(tab_3, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 965, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        QWidget::setTabOrder(radioButton_ip, radioButton_serial);
        QWidget::setTabOrder(radioButton_serial, ipAddress);
        QWidget::setTabOrder(ipAddress, portNumber);
        QWidget::setTabOrder(portNumber, connectButton);
        QWidget::setTabOrder(connectButton, disconnectButton);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionSave_Statistics_As);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Controller", Q_NULLPTR));
        actionSave_Statistics_As->setText(QApplication::translate("MainWindow", "Save Statistics As", Q_NULLPTR));
        actionAbout->setText(QApplication::translate("MainWindow", "About", Q_NULLPTR));
        radioButton_ip->setText(QApplication::translate("MainWindow", "IP", Q_NULLPTR));
        radioButton_serial->setText(QApplication::translate("MainWindow", "Serial", Q_NULLPTR));
        ipAddress->setText(QApplication::translate("MainWindow", "127.0.0.1", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "IP PORT", Q_NULLPTR));
        connectButton->setText(QApplication::translate("MainWindow", "Connect", Q_NULLPTR));
        disconnectButton->setText(QApplication::translate("MainWindow", "Disconnect", Q_NULLPTR));
        statusLabel_2->setText(QApplication::translate("MainWindow", "Disconnected", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Connect", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "Link Status-", Q_NULLPTR));
        statusLabel->setText(QApplication::translate("MainWindow", "Disconnected", Q_NULLPTR));
        label_4->setText(QApplication::translate("MainWindow", "Time:", Q_NULLPTR));
        getNodeButton->setText(QApplication::translate("MainWindow", "Request Node Id", Q_NULLPTR));
        pushButton_load->setText(QApplication::translate("MainWindow", "Load Performance Metrics", Q_NULLPTR));
        pushButton_update->setText(QApplication::translate("MainWindow", " Update Performance Metrics", Q_NULLPTR));
        pushButton->setText(QApplication::translate("MainWindow", "View Energest Metrics", Q_NULLPTR));
        clrbrowser->setText(QApplication::translate("MainWindow", "Clear", Q_NULLPTR));
        label_3->setText(QApplication::translate("MainWindow", "Refresh Rate (ms)", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Management", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("MainWindow", "Energy Metrics", Q_NULLPTR));
        traffic->setText(QApplication::translate("MainWindow", "Traffic Metrics", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MainWindow", "Graphs", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", Q_NULLPTR));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
