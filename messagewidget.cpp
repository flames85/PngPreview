#include <QtGui>
#include <qDebug>
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include "messagewidget.h"

MessageWidget::MessageWidget() :
    QWidget(NULL, Qt::FramelessWindowHint)
{

}

MessageWidget::~MessageWidget()
{

}


void MessageWidget::Init()
{
    this->setGeometry(QRect(0, 0, 500, 15));
    // 置顶
    Qt::WindowFlags flags = this->windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    flags |= Qt::Tool;
    this->setWindowFlags(flags);

    // label
    m_label = new QLabel(this);
//    m_label->setWordWrap(true);
    m_label->setAlignment(Qt::AlignVCenter);

}


void MessageWidget::ShowMessage(const QString &strMessage)
{
    // 显示
    show();

    // 修改显示字符
    m_label->setText(strMessage);
    m_label->adjustSize();
    m_label->show();

    Hide();
//    // 一定时间后执行隐藏
//    QTimer::singleShot(nDurationMsec, this, SLOT(Hide()));
}

void MessageWidget::Hide()
{
   //界面动画，改变透明度的方式消失1 - 0渐变
   QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
   animation->setDuration(5000);
   animation->setStartValue(1);
   animation->setEndValue(0);
   animation->start();
   connect(animation, SIGNAL(finished()), this, SLOT(hide()));
}


