#ifndef __MESSAGE_WIDGET_H__
#define __MESSAGE_WIDGET_H__

#include <QWidget>
#include <QSet>

class QLabel;
class QPropertyAnimation;

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget();
    virtual ~MessageWidget();

    void Init();

    void ShowMessage(const QString &strMessage);


protected:

    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

private:
    void GradualHide();


private slots:
    void TriggerHide();


private:

    QLabel*                 m_label;
    QPropertyAnimation *    m_animation;
};

#endif // __MESSAGE_WIDGET_H__
