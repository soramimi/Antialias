#include "MyWidget.h"
#include <QPainter>
#include "antialias.h"

MyWidget::MyWidget(QWidget *parent)
	: QWidget(parent)
{

}

void MyWidget::paintEvent(QPaintEvent *event)
{
	int w = width();
	int h = height();
	if (image.width() != w || image.height() != h) {
		if (w > 0 && h > 0) {
			image = QImage(w, h, QImage::Format_Grayscale8);
			image.fill(Qt::black);
			{
				QPainter pr(&image);
				pr.setPen(Qt::white);
				pr.setBrush(Qt::white);
				pr.drawEllipse(1, 1, w - 3, h - 3);
			}
			image::antialias(&image);
		} else {
			image = QImage();
		}
	}
	if (!image.isNull()) {
		QPainter pr(this);
		pr.drawImage(0, 0, image);
	}
}
