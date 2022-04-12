#ifndef DRAWER_H
#define DRAWER_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QPropertyAnimation>

class AnchoredWidget : public QWidget
{
    Q_OBJECT
public:

    enum Position{
        Top,
        Bottom,
        Left,
        Right
    };

    Q_ENUM(Position)

    explicit AnchoredWidget(Position pos = Right, int width = 300, int height = 600, const QColor &bgColor = Qt::gray);
    ~AnchoredWidget();

    void addWidget(QWidget *widget, int row, int column, int rowSpan = 1, int columnSpan = 1, Qt::Alignment alignment = Qt::Alignment());
    void addLayout(QLayout *layout, int row, int column, int rowSpan = 1, int columnSpan = 1, Qt::Alignment alignment = Qt::Alignment());

    void setBgColor(const QColor &bgColor);

    void setOpacity(float opacity);

signals:

public slots:
    void toggleVisibility();

private:
    void setupStyleeSheet();

    Position m_pos;
    QColor m_bgColor;
    bool m_isHidden = true;
    float m_opacity = 1.0;

    QGridLayout *m_mainGridLayout = Q_NULLPTR;
    QGridLayout *m_scrollAreaWidgetLayout = Q_NULLPTR;
    QPushButton *m_handleBtn = Q_NULLPTR;

    QPropertyAnimation *m_animation = Q_NULLPTR;

    // QWidget interface
protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
};

#endif // DRAWER_H
