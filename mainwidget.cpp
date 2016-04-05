#include <assert.h>
#include <QtGui>
#include <qDebug>
#include <QPoint>
#include <QDebug>
#include <QLabel>
#include "mainwidget.h"
#include "messagewidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent, Qt::FramelessWindowHint),
    m_bMouseMove(false),
    m_originPos(new QPoint),
    m_originPixmap(new QPixmap()),
    m_scale(1.0),
    m_bKeepScale(true),
    m_degrees(0),
    m_msgWidget( new MessageWidget())
{
    m_supportFormatList << "png" << "jpg" << "gif" << "bmp" << "tiff" << "ico" << "svg";
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

bool MainWidget::OpenPic(const QString &strPic)
{
//    qDebug() << this->pos();
    QFileInfo picInfo(strPic);
    if(!picInfo.exists())
    {
        qDebug() << "file not exists:" << strPic;
        return false;
    }

    return OpenPic(picInfo.absoluteFilePath(), &(picInfo.absoluteDir()));
}

bool MainWidget::OpenPic(const QString &strPic, const QDir *dir)
{
    qDebug() << "OpenPic" << this->pos();
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
            QString strMsg = QString("open file: %1 fail!").arg(strPic);
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
    // 因为打开了新文件，所以缩放和角度恢复初始值
    if(!m_bKeepScale) // 只有非保持缩放的时候才初始化scale为1
    {
        m_scale = 1.0;
    }
    m_degrees = 0;

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

    QByteArray content = help.readAll();

    QMessageBox *msgBox = new QMessageBox(QMessageBox::Information,
                                          "Help",
                                          QString::fromUtf8(content),
                                          QMessageBox::Ok);

//    msgBox->setIconPixmap(QPixmap(":/images/yoda.png"));
    msgBox->show();
}

void MainWidget::TrigerStayOnTop()
{
    //! 置顶
    Qt::WindowFlags flags = this->windowFlags();
    flags ^= Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags);
    this->show();
}

void MainWidget::TrigerKeepScale()
{
    QAction* act = dynamic_cast<QAction*>(sender());
    if(act->isChecked())
    {
        //! lock
        m_bKeepScale = true;
    }
    else
    {
        //! unlock
        m_bKeepScale = false;
    }
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
    QAction *act_yoda = new QAction(QIcon(":/images/yoda.png"), tr("Open"), this);
    // top
    QAction *act_top = new QAction(/*QIcon(":/images/star_beast.png"),*/ tr("Always on Top"), this);
    act_top->setCheckable(true);
    // lock scale
    QAction *act_keep_scale = new QAction(/*QIcon(":/images/star_beast.png"),*/ tr("Keep Scale"), this);
    act_keep_scale->setCheckable(true);
    act_keep_scale->setChecked(true);
    // help
    QAction *act_rabbit = new QAction(QIcon(":/images/rabbit.png"), tr("Help"), this);
    // quit
    QAction *act_quit = new QAction(QIcon(":/images/cyclops.png"), tr("&Quit"), this);

    // adds
    addAction(act_yoda);
    addAction(act_top);
    addAction(act_keep_scale);
    addAction(act_rabbit);
    addAction(act_quit);
    // slot
    connect(act_yoda, SIGNAL(triggered()), this, SLOT(TriggerOpenDialog()));
    connect(act_top, SIGNAL(triggered()), this, SLOT(TrigerStayOnTop()));
    connect(act_keep_scale, SIGNAL(triggered()), this, SLOT(TrigerKeepScale()));
    connect(act_rabbit, SIGNAL(triggered()), this, SLOT(TriggerHelp()));
    connect(act_quit, SIGNAL(triggered()), this, SLOT(close()));
}

/* 单纯单击 */
void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
//    qDebug() << m_bMouseMove;
    if(event->button() == Qt::LeftButton && !m_bMouseMove)
    {
        bool bDirection = (event->globalPos().x() > frameGeometry().center().x()) ? true : false;
        Browse(bDirection);
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
    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty()) {
        return;
    }

    QString fileName = urls.first().toLocalFile();
    if (fileName.isEmpty()) {
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

    if(step > 0)
        Transfer(true);
    else
        Transfer(false);

    event->accept();
}

void MainWidget::SetMask(bool bNeedFixPos)
{
//    qDebug() << "degress:" << m_degrees << " scale" << m_scale;

    // 缩放
    QPixmap &pixmap = m_originPixmap->scaled(m_originPixmap->width() * m_scale,
                                             m_originPixmap->height() * m_scale);

    // 旋转
    QMatrix leftmatrix;
    leftmatrix.rotate(m_degrees);
    pixmap = pixmap.transformed(leftmatrix, Qt::SmoothTransformation);

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
            Transfer(true);
            break;
        }
        case Qt::Key_Down:
        {
            Transfer(false);
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

void MainWidget::Transfer(bool bDirection)
{
    // 如果之前paintEvent有未处理的消息，则执行。
    QCoreApplication::processEvents();
    qDebug() << "Transfer";
    // 按住ctrl的旋转
    if ( QApplication::keyboardModifiers () == Qt::ControlModifier)
    {
        if(bDirection)
        {
            m_degrees += 5;
        }
        else
        {
            m_degrees -= 5;
        }
    }
    // 没按住ctrl的旋转缩放
    else
    {
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
