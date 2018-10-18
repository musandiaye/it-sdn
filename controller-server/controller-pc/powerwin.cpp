#include "powerwin.h"
#include "ui_powerwin.h"

Powerwin::Powerwin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Powerwin)
{
    ui->setupUi(this);
}

Powerwin::~Powerwin()
{
    delete ui;
}
