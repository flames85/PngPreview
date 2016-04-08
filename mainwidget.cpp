#include <assert.h>
#include <QtGui>
#include <qDebug>
#include <QPoint>
#include <QDebug>
#include <QLabel>
#include <QtGlobal>
#include "mainwidget.h"
#include "messagewidget.h"
#include "networkPicture.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint),
    m_bMouseMove(false),
    m_originPos(new QPoint),
    m_originPixmap(new QPixmap()),
    m_scale(1.0),
    m_act_keep_scale(NULL),
    m_degrees(0),
    m_act_flip_h(NULL),
    m_act_flip_v(NULL),
    m_msgWidget(new MessageWidget()),
    m_networkPic(new NetworkPicture(this))
{
    m_supportFormatList << "png" << "jpg" << "gif" << "bmp" << "tiff" << "ico" << "svg";

    connect(m_networkPic,
            SIGNAL( ShowNetworkPicture(const QByteArray &) ),
            this,
            SLOT(TriggerShowNetworkPicture(const QByteArray &)));
}

MainWidget::~MainWidget()
{
    if(m_originPos)
    {
        delete m_originPos;
    }
    if(m_originPixmap)
    {
        delete m_originPixmap;
    }
}

void MainWidget::OpenPic(const QUrl &url)
{
    m_networkPic->Get(url);
}

bool MainWidget::OpenPic(const QString &strPic)
{
    qDebug() << "open pic:" << strPic;
    QFileInfo picInfo(strPic);
    if(!picInfo.exists())
    {
        QString strMsg = QString("open picture %1 fail: file not exists!").arg(strPic);
        qDebug() << strMsg;
        m_msgWidget->ShowMessage(strMsg);
        return false;
    }

    return OpenPic(picInfo.absoluteFilePath(), &(picInfo.absoluteDir()));
}

bool MainWidget::OpenPic(const QString &strPic, const QDir *dir)
{
    if(!m_originPixmap->load(strPic))
    {
        bool bLoadSuccess = false;
        foreach(const QString &strFormat, m_supportFormatList)
        {
            if(strPic.endsWith(strFormat, Qt::CaseInsensitive ))
            {
                continue;
            }

            if(m_originPixmap->load(strPic, strFormat.toAscii().data()))
            {
                bLoadSuccess = true;
                break;
            }
        }

        if(!bLoadSuccess)
        {
            QString strMsg = QString("open picture %1 fail!").arg(strPic);
            qDebug() << strMsg;
            m_msgWidget->ShowMessage(strMsg);
            return false;
        }
    }

    // 只有打开成功，才更新当期文件
    m_strCurrentPic = strPic;
    // 只有打开成功，才更新目录
    if(dir)
    {
        GetPictures(*dir);
    }

    //! 因为打开了新文件: 初始化所有状态
    if(!m_act_keep_scale->isChecked()) // 只有非保持缩放的时候才初始化scale为1
    {
        m_scale = 1.0;
    }
    m_degrees = 0;    
    m_act_flip_h->setChecked(false);
    m_act_flip_v->setChecked(false);
    //! 初始化结束

    SetMask(true);

    return true;
}

void MainWidget::TriggerOpenDialog()
{
    qDebug() << "open png...";
    QFileDialog* dialog = new QFileDialog(this);//创建对话框
    dialog->resize(700,400);    //设置显示的大小
    dialog->setFilter( "Allfile(*.*);;pngfile(*.png);;jpgfile(*.jpg);;giffile(*.gif);;bmpfile(*.bmp)"); //设置文件过滤器
    dialog->setViewMode(QFileDialog::Detail);  //设置浏览模式，有 列表（list） 模式和 详细信息（detail）两种方式
    if ( dialog->exec() == QDialog::Accepted )   //如果成功的执行
    {
        qDebug() << "has select file:" << dialog->selectedFiles()[0] << "dir:" << dialog->directory().path();
        OpenPic(dialog->selectedFiles()[0], &(dialog->directory()));
    }

    dialog->close();
    dialog->deleteLater();
}

void MainWidget::TriggerHelp()
{
    QFile help(":/help/shortcutkey.txt");
    if(!help.open(QIODevice:: ReadOnly))
    {
        return;
    }

    QByteArray context = help.readAll();

    QMessageBox *msgBox = new QMessageBox(QMessageBox::Information,
                                          "Help",
                                          QString::fromUtf8(context),
                                          QMessageBox::Ok);

    msgBox->setIconPixmap(QPixmap(":/images/help.png").scaled(QSize(64, 64),
                                                              Qt::KeepAspectRatio,
                                                              Qt::SmoothTransformation));
    msgBox->show();
}

void MainWidget::TrigerAlwaysOnTop()
{
    //! 置顶
    Qt::WindowFlags flags = this->windowFlags();
    flags ^= Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags);
    this->show();
}

// 垂直或水平翻转
void MainWidget::TriggerFlip()
{
    SetMask(false);
}

void MainWidget::TriggerShowNetworkPicture(const QByteArray &picData)
{
    // 打开的是网络图片,所以本地图片删除
    m_strCurrentPic.clear();
    m_dirPic.clear();

    if(!m_originPixmap->loadFromData(picData))
    {
        QString strMsg = QString("open picture from %1 fail!").arg(m_networkPic->m_url.toString());
        qDebug() << strMsg;
        m_msgWidget->ShowMessage(strMsg);
    }

    //! 因为打开了新文件: 初始化所有状态
    if(!m_act_keep_scale->isChecked()) // 只有非保持缩放的时候才初始化scale为1
    {
        m_scale = 1.0;
    }
    m_degrees = 0;
    m_act_flip_h->setChecked(false);
    m_act_flip_v->setChecked(false);
    //! 初始化结束

    SetMask(true);
}

void MainWidget::Init()
{
    this->setObjectName(tr("PngPreview"));
    this->setWindowTitle(tr("PngPreview - by Flames"));
    this->setAcceptDrops(true);

    setupContextMenu();
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);

    m_msgWidget->Init();
}

void MainWidget::setupContextMenu()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    // open
    QAction *act_yoda = new QAction(QIcon(":/images/open.png"), tr("&Open(O)..."), this);
    // flip horizontal
    m_act_flip_h = new QAction(tr("Flip &Horizontal(H)"), this);
    m_act_flip_h->setCheckable(true);
    m_act_flip_h->setChecked(false);
    // flip vertical
    m_act_flip_v = new QAction(tr("Flip &Vertical(V)"), this);
    m_act_flip_v->setCheckable(true);
    m_act_flip_v->setChecked(false);
    // top
    QAction *act_top = new QAction( tr("&Always on Top(A)"), this);
    act_top->setCheckable(true);
    act_top->setChecked(false);
    // lock scale
    m_act_keep_scale = new QAction(tr("&Keep Scale(K)"), this);
    m_act_keep_scale->setCheckable(true);
    m_act_keep_scale->setChecked(true);
    // help
    QAction *act_help = new QAction(QIcon(":/images/help.png"), tr("Help"), this);
    // quit
    QAction *act_quit = new QAction(QIcon(":/images/quit.png"), tr("&Quit(Q)"), this);

    // adds
    addAction(act_yoda);
    addAction(m_act_keep_scale);
    addAction(act_top);
    addAction(m_act_flip_h);
    addAction(m_act_flip_v);
    addAction(act_help);
    addAction(act_quit);
    // slot
    connect(act_yoda, SIGNAL(triggered()), this, SLOT(TriggerOpenDialog()));
    connect(m_act_flip_h, SIGNAL(triggered()), this, SLOT(TriggerFlip()));
    connect(m_act_flip_v, SIGNAL(triggered()), this, SLOT(TriggerFlip()));
    connect(act_top, SIGNAL(triggered()), this, SLOT(TrigerAlwaysOnTop()));
    connect(act_help, SIGNAL(triggered()), this, SLOT(TriggerHelp()));
    connect(act_quit, SIGNAL(triggered()), this, SLOT(close()));
}

/* 单纯单击 */
void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && !m_bMouseMove)
    {
        // 偏离中心点超过1/4的宽度才启动browse逻辑
        qDebug() << "abs:" << qAbs(event->globalPos().x() - frameGeometry().center().x()) << " 1/4:" << (frameGeometry().width()/4 ) ;
        if( qAbs(event->globalPos().x() - frameGeometry().center().x()) > (frameGeometry().width() / 4) )
        {
            bool bDirection = (event->globalPos().x() > frameGeometry().center().x()) ? true : false;
            Browse(bDirection);
        }
    }
}

/* 拖拽移动窗体：点击 */
void MainWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton )
    {
        *m_originPos = event->globalPos() - frameGeometry().topLeft();
        event->accept();
        m_bMouseMove = false;
    }
}

/* 拖拽移动窗体：移动 */
void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        qDebug() << *m_originPos;
        Move(event->globalPos() - *m_originPos);
        event->accept();
        m_bMouseMove = true;
    }
}

//// 双击事件
//void MainWidget::mouseDoubleClickEvent(QMouseEvent *)
//{

//}

void MainWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}


void MainWidget::dropEvent(QDropEvent *event)
{
    qDebug() << "dropEvent" << event->mimeData()->urls();
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }

    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty())
    {
        QString strUrl = urls.first().toString();
        OpenPic(QUrl(strUrl));
        return;
    }

    OpenPic(fileName);
}


/* 绘制窗体 */
void MainWidget::paintEvent(QPaintEvent *)
{
    qDebug() << "paintEvent...";
    QPainter painter(this);
    painter.fillRect(0, 0, m_transferdPixmap.width(), m_transferdPixmap.height(), m_transferdPixmap);
//    painter.drawPixmap(QPointF(0, 0), m_transferdPixmap);
}


void MainWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event ->delta() / 8;
    int step = numDegrees / 15;
    // 方向判定
    bool bDirection = step > 0 ? true : false;
    // 按住ctrl的旋转
    if ( QApplication::keyboardModifiers () == Qt::ControlModifier)
    {
        Rotate(bDirection);
    }
    // 未按住ctrl的缩放
    else
    {
        Zoom(bDirection);
    }

    event->accept();
}

void MainWidget::SetMask(bool bNeedFixPos)
{
    // 缩放
    QPixmap &pixmap = m_originPixmap->scaled(m_originPixmap->width() * m_scale,
                                             m_originPixmap->height() * m_scale);
    // 旋转
    QMatrix leftmatrix;
    leftmatrix.rotate(m_degrees);
    pixmap = pixmap.transformed(leftmatrix, Qt::SmoothTransformation);

    // 翻转
    qDebug() << "flip:" << m_act_flip_h->isChecked() << m_act_flip_v->isChecked();
    pixmap = QPixmap::fromImage( pixmap.toImage().mirrored(m_act_flip_h->isChecked(), m_act_flip_v->isChecked()) );

    // 修正移动的位置,因为图片的大小肯定不一致，而我们希望每次显示的图片都和原来的中心点位置一致
    if(bNeedFixPos)
    {
        QPoint currentPicCenter = this->frameGeometry().center();
        QPoint newPicCenter = QRect(this->frameGeometry().topLeft(), pixmap.size()).center();
        QPoint deltaPos = currentPicCenter - newPicCenter ;
        QPoint finalTopleft = this->frameGeometry().topLeft() + deltaPos;
        Move(finalTopleft);
    }

    // mask
    m_transferdPixmap = pixmap;
    resize(m_transferdPixmap.width(), m_transferdPixmap.height());
    clearMask();
    setMask(m_transferdPixmap.mask());
    update();
}

void MainWidget::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
        case Qt::Key_Left:
        {
            Browse(false);
            break;
        }
        case Qt::Key_Right:
        {
            qDebug() << "right";
            Browse(true);
            break;
        }
        case Qt::Key_Up:
        {
            // 按住ctrl的旋转
            if ( QApplication::keyboardModifiers () == Qt::ControlModifier)
            {
                Rotate(true);
            }
            // 未按住ctrl的缩放
            else
            {
                Zoom(true);
            }
            break;
        }
        case Qt::Key_Down:
        {
            // 按住ctrl的旋转
            if ( QApplication::keyboardModifiers () == Qt::ControlModifier)
            {
                Rotate(false);
            }
            // 未按住ctrl的缩放
            else
            {
                Zoom(false);
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void MainWidget::keyReleaseEvent(QKeyEvent *e)
{
//    if(e->isAutoRepeat()){
//        qDebug() <<  "repeat release ...";
//    }
//    else{
//        qDebug() << "release";
//    }
}

void MainWidget::Browse(bool bDirection)
{
    // 如果之前paintEvent有未处理的消息，则执行。
    QCoreApplication::processEvents();

    if(bDirection)
    {
        // 下一个文件
        QSet<QString>::const_iterator iter = m_dirPic.find(m_strCurrentPic);
        QString strPic;
        if(iter != m_dirPic.end())
        {
          iter++;
          if(iter == m_dirPic.end())
          {
              // 到结尾了
              strPic = *(m_dirPic.begin());
          }
          else
          {
              strPic = *iter;
          }
          OpenPic(strPic, NULL);
        }
    }
    else
    {
        // 上一个文件
        QSet<QString>::const_iterator iter = m_dirPic.find(m_strCurrentPic);

        QString strPic;
        if(iter != m_dirPic.end())
        {
            if(m_dirPic.begin() == iter )
            {
                // 已经到开头了
                iter = m_dirPic.end();
                iter--;
                strPic = *iter;
            }
            else
            {
                iter--;
                strPic = *iter;
            }
            OpenPic(strPic, NULL);
        }
    }
}

void MainWidget::Zoom(bool bDirection)
{
    // 如果之前paintEvent有未处理的消息，则执行。
    QCoreApplication::processEvents();
    qDebug() << "zoom...";
    if(bDirection)
    {
        if(m_scale > 5)
        {
            return; // 限制一个最大放大数值
        }
        m_scale *= 1.05;
    }
    else
    {
        if(m_scale < 0.2)
        {
            return; // 限制一个最小缩小数值
        }
        m_scale /= 1.05;
    }
    SetMask(false);
}

void MainWidget::Rotate(bool bDirection)
{
    // 如果之前paintEvent有未处理的消息，则执行。
    QCoreApplication::processEvents();
    qDebug() << "rotate...";

    if(bDirection)
    {
        m_degrees += 3;
    }
    else
    {
        m_degrees -= 3;
    }
    SetMask(false);
}

void MainWidget::GetPictures(const QDir &dir)
{
    m_dirPic.clear();

    QStringList::Iterator iter;
    // 遍历文件
    QStringList listDir = dir.entryList(QDir::Files, QDir::Name);

    for(iter = listDir.begin(); iter != listDir.end(); ++iter)
    {
        const QString &strFileName = *iter;
        foreach(const QString &strFormat, m_supportFormatList)
        {
            if(strFileName.endsWith(strFormat, Qt::CaseInsensitive ))
            {
                QString strFullFileName = QString("%1/%2").arg(dir.path()).arg(strFileName);
                m_dirPic.insert(strFullFileName);
                break;
            }
        }
    }
}

void MainWidget::Move(const QPoint &moveTo)
{
    this->move(moveTo);
    m_msgWidget->move(moveTo);
    m_msgWidget->raise();
    qDebug() << "move to: " << moveTo;
}
