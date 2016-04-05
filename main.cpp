#include <QtGui/QApplication>
#include <QIcon>

#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon("yoda128.ico"));

    MainWidget *widget = new MainWidget;
    widget->Init();
    widget->show();
    widget->setAcceptDrops(true);

    if(argc >= 2)
    {
        widget->OpenPic(QString(argv[1]));
    }
    else
    {
        widget->OpenPic(":/images/yoda.png", NULL);
    }

    return app.exec();
}
