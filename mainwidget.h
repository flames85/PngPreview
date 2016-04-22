#ifndef __MAIN_WIDGET_H__
#define __MAIN_WIDGET_H__

#include <QWidget>
#include <QSet>

class QPoint;
class QPixmap;
class QDir;
class MessageWidget;
class NetworkPicture;
class QUrl;
class QNetworkReply;
class QBuffer;

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);
    virtual ~MainWidget();

    void Init();

    // 打开网络图片
    void OpenPic(const QUrl &url);
    // 打开本地图片
    bool OpenPic(const QString &strPic);
    // 打开本地图片
    bool OpenPic(const QString &strPic, const QDir *dir);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void wheelEvent(QWheelEvent *event);
//    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *e);
//    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);

private:
    // 右键菜单
    void setupContextMenu();
    // 将图片设置为界面
    void SetMask(bool bNeedFixPos);
    // 图片缩放
    void Zoom(bool bDirection);
    // 图片旋转
    void Rotate(bool bDirection);
    // 浏览
    void Browse(bool bDirection);
    // 获取目录下所有图片文件名称
    void GetPictures(const QDir &dir);
    // 位置移动
    void Move(const QPoint &moveTo);

private slots:
    // 打开文件选择对话框
    void TriggerOpenDialog();
    // 打开帮助对话框
    void TriggerHelp();
    // 设置始终保持最前
    void TrigerAlwaysOnTop();
    // 垂直或水平翻转
    void TriggerFlip();

    // 显示网络图片
    void TriggerShowNetworkPicture(QNetworkReply *reply);

    // 显示动画
    void TriggerMovieUpdated(const QRect &);
//    void TriggerMovieFinished();

private:
    // 判断鼠标位置是否移动了
    bool            m_bMouseMove;
    // 原始位置
    QPoint         *m_originPos;

    //! 以下一组成员变量保存了图片的信息, 每次变化图片显示(包括新打开图片,旋转图片,缩放图片)都以原图信息做改变厚显示
    /***************************************/
    // 原始图片信息，缩放后会丢失信息，所以每次都用原图片信息缩放
    QPixmap*        m_originPixmap;
    // 变换后的图片信息
    QPixmap         m_transferdPixmap;
    // 缩放值
    double          m_scale;
    QAction*        m_act_keep_scale;
    // 旋转角度值
    int             m_degrees;
    // 是否水平翻转
    QAction*        m_act_flip_h;
    // 是否垂直翻转
    QAction*        m_act_flip_v;
    /***************************************/

    // dir
    QString         m_strCurrentPic;
    QSet<QString>   m_dirPic;

    // support format
    QList<QString>  m_supportFormatList;

    // message widget
    MessageWidget   *m_msgWidget;

    // 网络图片
    NetworkPicture  *m_networkPic;
    QBuffer         *m_networkPicBuffer;

    // 动画
    QMovie          *m_movie;
};

#endif // __MAIN_WIDGET_H__
