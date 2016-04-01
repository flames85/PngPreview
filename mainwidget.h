#ifndef __MAIN_WIDGET_H__
#define __MAIN_WIDGET_H__

#include <QWidget>
#include <QSet>

class QPoint;
class QPixmap;
class QDir;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    virtual ~MainWidget();

    bool Open(const QString &strPic);

protected:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void wheelEvent(QWheelEvent *event);
//    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

private:
    void Init();
    void setupContextMenu();
    void SetMask(bool bNeedFixPos);
    bool OpenPic(const QString &strPic, const QDir *dir, bool bNeedFixPos);
    void Transfer(bool direction);
    void GetPictures(const QDir &dir);

private slots:
    void TriggerOpenDialog();
    void TriggerHelp();
    void TrigerStayOnTop();
    void TrigerKeepScale();

private:
    // 原始位置
    QPoint*         m_originPos;
    // 原始图片信息，缩放后会丢失信息，所以每次都用原图片信息缩放
    QPixmap*        m_originPixmap;
    // 变换后的图片信息
    QPixmap         m_transferdPixmap;
    // 缩放值
    double          m_scale;
    bool            m_bKeepScale;
    // 旋转角度值
    int             m_degrees;
    // dir
    QString         m_strCurrentPic;
    QSet<QString>   m_pictures;
};

#endif // __MAIN_WIDGET_H__
