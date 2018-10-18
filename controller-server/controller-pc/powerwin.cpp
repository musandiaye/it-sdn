#include "powerwin.h"
#include "ui_powerwin.h"
#include <QDateTime>
#include <QFileDialog>
#include <QFile>
#include <QProcess>
#include <QTextStream>
#include <QMessageBox>
Powerwin::Powerwin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Powerwin)
{
    ui->setupUi(this);
    QString Pythonfilter ="Python Files (*.py)";
    QString filter ="Text Files (*.txt)";
    file_path_py =QFileDialog::getOpenFileName(this,"Load the energest script", QDir::homePath(),Pythonfilter);//This obtains the file path
    file_path_log =QFileDialog::getOpenFileName(this,"Choose a log file", QDir::homePath(),filter);//This obtains the file path>
    QProcess *process = new QProcess;
    process->start( "python", QStringList()<< file_path_py << file_path_log);
    process->waitForFinished();
    QString data=process->readAllStandardOutput();
    ui->textBrowser_power->append(data);
}

Powerwin::~Powerwin()
{
    delete ui;
}




void Powerwin::on_actionSave_As_triggered()
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
    QString text = ui->textBrowser_power->toPlainText();//copies the text edit content in the ui to text variable
    out<< text;//then we pass the text to the output
    file.flush();//we flush the file
    file.close();//Once done the file is closed
}
