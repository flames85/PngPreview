#include <QtGui>
#include <qDebug>
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include "messagewidget.h"

MessageWidget::MessageWidget() :
    QWidget(NULL, Qt::FramelessWindowHint),
    m_animation(new QPropertyAnimation(this, "windowOpacity"))
{
    //界面动画，改变透明度的方式消失1 - 0渐变
    m_animation->setDuration(5000);
    m_animation->setStartValue(1);
    m_animation->setEndValue(0);
    connect(m_animation, SIGNAL(finished()), this, SLOT(TriggerHide()));
}

MessageWidget::~MessageWidget()
{

}


void MessageWidget::Init()
{
//    this->setGeometry(QRect(0, 0, 480, 15));
    // 置顶
    Qt::WindowFlags flags = this->windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    flags |= Qt::Tool;
    this->setWindowFlags(flags);

    // label
    m_label = new QLabel(this);
//    m_label->setWordWrap(true);
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // layout

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_label);
    this->setLayout(layout);
}


void MessageWidget::ShowMessage(const QString &strMessage)
{
    // 显示
    show();

    // 修改显示字符
    m_label->setText(strMessage);
    m_label->adjustSize();
    m_label->show();

    // 渐隐
    GradualHide();
}

void MessageWidget::GradualHide()
{
   if(QAbstractAnimation::Running != m_animation->state())
   {
       m_animation->start();
   }
}

void MessageWidget::TriggerHide()
{
    this->hide();
}

void MessageWidget::enterEvent(QEvent *)
{
    if(QAbstractAnimation::Running == m_animation->state())
    {
        m_animation->stop();
        m_animation->start();
        m_animation->stop();
    }

    this->show();
}

void MessageWidget::leaveEvent(QEvent *)
{
    GradualHide();
}


