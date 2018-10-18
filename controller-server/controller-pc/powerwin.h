#ifndef POWERWIN_H
#define POWERWIN_H

#include <QMainWindow>

namespace Ui {
class Powerwin;
}

class Powerwin : public QMainWindow
{
    Q_OBJECT

public:
    explicit Powerwin(QWidget *parent = 0);
    ~Powerwin();

private slots:
    void on_textBrowser_power_destroyed();

    void on_textBrowser_power_destroyed(QObject *arg1);

    void on_actionSave_As_triggered();

private:
    Ui::Powerwin *ui;
    QString file_path_py;
    QString file_path_log;

};

#endif // POWERWIN_H
