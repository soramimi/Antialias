#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>

class MyWidget : public QWidget
{
	Q_OBJECT
private:
	QImage image;
public:
	explicit MyWidget(QWidget *parent = nullptr);

signals:

public slots:

	// QWidget interface
protected:
	void paintEvent(QPaintEvent *event);
};

#endif // MYWIDGET_H
