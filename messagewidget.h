#ifndef __MESSAGE_WIDGET_H__
#define __MESSAGE_WIDGET_H__

#include <QWidget>
#include <QSet>

class QLabel;

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget();
    virtual ~MessageWidget();

    void Init();

    void ShowMessage(const QString &strMessage);


protected:


private:

    void Hide();

private slots:


private:

    QLabel*     m_label;
};

#endif // __MESSAGE_WIDGET_H__
