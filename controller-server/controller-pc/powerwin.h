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

private:
    Ui::Powerwin *ui;
};

#endif // POWERWIN_H
