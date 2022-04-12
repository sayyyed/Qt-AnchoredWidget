#include "anchoredwidget.h"
#include <QScrollArea>
#include <QFrame>
#include <QPushButton>
#include <QDesktopWidget>
#include <QApplication>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QStyle>

AnchoredWidget::AnchoredWidget(Position pos, int width, int height, const QColor &bgColor)
    : QWidget(Q_NULLPTR)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::SubWindow);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    setLayoutDirection(Qt::LeftToRight);

    if(m_opacity < 1)
        this->setWindowOpacity(m_opacity);

    resize(width, height);

    m_pos = pos;
    m_bgColor = bgColor;
    setupStyleeSheet();

    m_mainGridLayout = new QGridLayout;
    m_mainGridLayout->setMargin(6);
    m_mainGridLayout->setSpacing(0);
    setLayout(m_mainGridLayout);

    QFrame *mainFrame = new QFrame;
    mainFrame->setObjectName("mainFrame");
    m_mainGridLayout->addWidget(mainFrame);

    QGridLayout *mainFrameLayout = new QGridLayout;
    mainFrameLayout->setMargin(0);
    mainFrameLayout->setSpacing(1);
    mainFrame->setLayout(mainFrameLayout);

    QFrame *scrollAreaFrame = new QFrame;
    scrollAreaFrame->setObjectName("scrollAreaFrame");
    QVBoxLayout *scrollAreaFrameLayout = new QVBoxLayout;
    scrollAreaFrameLayout->setMargin(2);
    scrollAreaFrame->setLayout(scrollAreaFrameLayout);

    QScrollArea *mainScrollArea = new QScrollArea;
    scrollAreaFrameLayout->addWidget(mainScrollArea);
    mainScrollArea->setObjectName("mainScrollArea");
    mainScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *scrollAreaWidget = new QWidget;
    scrollAreaWidget->setObjectName("scrollAreaWidget");
    mainScrollArea->setWidget(scrollAreaWidget);
    mainScrollArea->setWidgetResizable(true);
    m_scrollAreaWidgetLayout = new QGridLayout;
    m_scrollAreaWidgetLayout->setMargin(2);
    m_scrollAreaWidgetLayout->setSpacing(1);
    scrollAreaWidget->setLayout(m_scrollAreaWidgetLayout);

    m_handleBtn = new QPushButton;
    m_handleBtn->setObjectName("handleBtn");
    m_handleBtn->setCursor(Qt::PointingHandCursor);

    int handleSize = 25;
    int x = 0;
    int y = 0;

    QDesktopWidget *desktop = QApplication::desktop();
    switch (pos) {
    case Left:
        m_handleBtn->setMinimumSize(handleSize, 100);
        m_handleBtn->setMaximumSize(handleSize, 100);
        x = desktop->screenGeometry().x() - this->width() + m_handleBtn->width()+m_mainGridLayout->margin();
        y = desktop->screenGeometry().height()/2 - this->height()/2;
        mainFrameLayout->addWidget(scrollAreaFrame, 0, 0);
        mainFrameLayout->addWidget(m_handleBtn, 0, 1);
        break;

    case Right:
        m_handleBtn->setMinimumSize(handleSize, 100+handleSize);
        m_handleBtn->setMaximumSize(handleSize, 100+handleSize);
        x = desktop->screenGeometry().width() - m_handleBtn->width()-m_mainGridLayout->margin();
        y = desktop->screenGeometry().height()/2 - this->height()/2;
        mainFrameLayout->addWidget(m_handleBtn, 0, 0);
        mainFrameLayout->addWidget(scrollAreaFrame, 0, 1);
        break;

    case Top:
        m_handleBtn->setMinimumSize(100, handleSize);
        m_handleBtn->setMaximumSize(100, handleSize);
        x = desktop->screenGeometry().width()/2 - this->width()/2;
        y = desktop->screenGeometry().y() - this->height() + m_handleBtn->height()+m_mainGridLayout->margin();
        mainFrameLayout->addWidget(scrollAreaFrame, 0, 0);
        mainFrameLayout->addWidget(m_handleBtn, 1, 0, 1, 1, Qt::AlignCenter);
        break;
    case Bottom:
        m_handleBtn->setMinimumSize(100, handleSize);
        m_handleBtn->setMaximumSize(100, handleSize);
        x = desktop->screenGeometry().width()/2 - this->width()/2;
        y = desktop->screenGeometry().height() - m_handleBtn->height()-m_mainGridLayout->margin();
        mainFrameLayout->addWidget(m_handleBtn, 0, 0, 1, 1, Qt::AlignCenter);
        mainFrameLayout->addWidget(scrollAreaFrame, 1, 0);
        break;
    default:
        break;
    }

    move(x, y);

    m_animation = new QPropertyAnimation(this, "geometry", this);
    m_animation->setDuration(200);
    m_animation->setEasingCurve(QEasingCurve::InOutBack);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect;
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 200));
    shadow->setBlurRadius(10);
    mainFrame->setGraphicsEffect(shadow);

    connect(m_handleBtn, &QPushButton::clicked, this, &AnchoredWidget::toggleVisibility);
}

void AnchoredWidget::addWidget(QWidget *widget, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment)
{
    m_scrollAreaWidgetLayout->addWidget(widget, row, column, rowSpan, columnSpan, alignment);
}

void AnchoredWidget::addLayout(QLayout *layout, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment)
{
    m_scrollAreaWidgetLayout->addLayout(layout, row, column, rowSpan, columnSpan, alignment);
}

AnchoredWidget::~AnchoredWidget()
{

}

void AnchoredWidget::setupStyleeSheet()
{
    int borderTopLeftRadius = m_pos == Right || m_pos == Bottom ? 10 : 0;
    int borderBottomLeftRadius = m_pos == Right || m_pos == Top ? 10 : 0;
    int borderTopRightRadius = m_pos == Left || m_pos == Bottom ? 10 : 0;
    int borderBottomRightRadius = m_pos == Left || m_pos == Top ? 10 : 0;

    QString handleImageUrl = m_pos == Right || m_pos == Left ? "://AnchoredWidgetResources/vertical_3_lines.png"
                                                               : "://AnchoredWidgetResources/horizontal_3_lines.png";
    this->setStyleSheet(QString(R"(
                                #handleBtn{
                                background-color: %5;
                                border: 1.5px solid %6;
                                border-top-left-radius: %0px;
                                border-bottom-left-radius: %1px;
                                border-top-Right-radius: %2px;
                                border-bottom-Right-radius: %3px;
                                image: url(%4);
                                }

                                #handleBtn:hover{
                                background-color: %6;
                                }

                                #scrollAreaFrame{
                                background-color: %5;
                                border-top-left-radius: %0px;
                                border-bottom-left-radius: %1px;
                                border-top-Right-radius: %2px;
                                border-bottom-Right-radius: %3px;
                                border: 1.5px solid %6;
                                }

                                QScrollArea,#scrollAreaWidget{
                                background-color: transparent;
                                border: none;
                                }

                                QScrollBar:horizontal
                                {
                                height: 8px;
                                margin: 2px 0px;
                                border: 1px transparent #2A2929;
                                border-radius: 2px;
                                background-color: rgb(220, 220, 220);
                                }

                                QScrollBar::handle:horizontal
                                {
                                background-color: rgb(86, 86, 86);
                                min-height: 12px;
                                height: 8px;
                                border-radius: 3px;
                                }

                                QScrollBar::handle:horizontal:hover
                                {
                                background-color: rgb(120, 120, 120);
                                }

                                QScrollBar::up-arrow:horizontal, QScrollBar::down-arrow:horizontal
                                {
                                background: none;
                                }


                                QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal
                                {
                                background: none;
                                }

                                QScrollBar::add-line:horizontal
                                {
                                width: 0px;
                                background: none;
                                border: none;
                                }

                                QScrollBar::sub-line:horizontal
                                {
                                width: 0px;
                                background: none;
                                border: none;
                                }

                                QScrollBar:vertical
                                {
                                width: 8px;
                                margin: 0px 2px;
                                border: 1px #2A2929;
                                border-radius: 2px;
                                background-color: rgb(220, 220, 220);
                                }

                                QScrollBar::handle:vertical
                                {
                                background-color: rgb(86, 86, 86);
                                min-width: 8px;
                                width: 8px;
                                border-radius: 2px;
                                }

                                QScrollBar::handle:vertical:hover
                                {
                                background-color: rgb(120, 120, 120);
                                }

                                QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical
                                {
                                background: none;
                                }

                                QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical
                                {
                                background: none;
                                }

                                QScrollBar::add-line:vertical
                                {
                                width: 0px;
                                background: none;
                                border: none;
                                }

                                QScrollBar::sub-line:vertical
                                {
                                width: 0px;
                                background: none;
                                border: none;
                                }
                                )")
                        .arg(borderTopLeftRadius)
                        .arg(borderBottomLeftRadius)
                        .arg(borderTopRightRadius)
                        .arg(borderBottomRightRadius)
                        .arg(handleImageUrl)
                        .arg(m_bgColor.name())
                        .arg(m_bgColor.darker().name()));
}

void AnchoredWidget::setOpacity(float opacity)
{
    m_opacity = opacity;
    if(m_opacity < 1)
        this->setWindowOpacity(m_opacity);
}

void AnchoredWidget::setBgColor(const QColor &bgColor)
{
    m_bgColor = bgColor;
    this->setupStyleeSheet();
    this->style()->polish(this);
}

void AnchoredWidget::toggleVisibility()
{
    QDesktopWidget *desktop = QApplication::desktop();

    int oldX = this->x();
    int oldY = this->y();

    int newX = 0;
    int newY = 0;
    if(m_isHidden)//to show
    {
        if(m_pos == Left)
        {
            newX = desktop->screenGeometry().x() - m_mainGridLayout->margin();
            newY = desktop->screenGeometry().height()/2 - this->height()/2;
        }
        else if(m_pos == Right)
        {
            newX = desktop->screenGeometry().width() - this->width() + m_mainGridLayout->margin();
            newY = desktop->screenGeometry().height()/2 - this->height()/2;
        }
        else if(m_pos == Top)
        {
            newX = desktop->screenGeometry().width()/2 - this->width()/2;
            newY = desktop->screenGeometry().y() - m_mainGridLayout->margin();
        }
        else if(m_pos == Bottom)
        {
            newX = desktop->screenGeometry().width()/2 - this->width()/2;
            newY = desktop->screenGeometry().height() - this->height()+m_mainGridLayout->margin();
        }
    }
    else//to hide
    {
        if(m_pos == Left)
        {
            newX = desktop->screenGeometry().x() - this->width() + m_handleBtn->width()+m_mainGridLayout->margin();
            newY = desktop->screenGeometry().height()/2 - this->height()/2;
        }
        else if(m_pos == Right)
        {
            newX = desktop->screenGeometry().width() - m_handleBtn->width()-m_mainGridLayout->margin();
            newY = desktop->screenGeometry().height()/2 - this->height()/2;
        }
        else if(m_pos == Top)
        {
            newX = desktop->screenGeometry().width()/2 - this->width()/2;
            newY = desktop->screenGeometry().y() - this->height() + m_handleBtn->height()+m_mainGridLayout->margin();
        }
        else if(m_pos == Bottom)
        {
            newX = desktop->screenGeometry().width()/2 - this->width()/2;
            newY = desktop->screenGeometry().height() - m_handleBtn->height()-m_mainGridLayout->margin();
        }
    }

    m_animation->setStartValue(QRect(oldX, oldY, this->width(), this->height()));
    m_animation->setEndValue(QRect(newX, newY, this->width(), this->height()));

    m_animation->start();

    m_isHidden = !m_isHidden;
}


void AnchoredWidget::enterEvent(QEvent *)
{
    if(m_isHidden && m_opacity < 1)
        this->setWindowOpacity(1.0);
}


void AnchoredWidget::leaveEvent(QEvent *)
{
    if(m_isHidden && m_opacity < 1)
        this->setWindowOpacity(m_opacity);
}
