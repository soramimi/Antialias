#include "MyWidget.h"
#include <QPainter>
#include "antialias.h"

MyWidget::MyWidget(QWidget *parent)
	: QWidget(parent)
{

}

void MyWidget::paintEvent(QPaintEvent *event)
{
	int w = width() / 2;
	int h = height();
	if (image1.width() != w || image1.height() != h) {
		if (w > 0 && h > 0) {
			image1 = QImage(w, h, QImage::Format_RGB888);
			image1.fill(Qt::black);
			{
				QPainter pr(&image1);
				pr.setPen(Qt::white);
				pr.setBrush(Qt::white);
				pr.drawEllipse(1, 1, w - 3, h - 3);
			}
			image2 = image1;
			image::antialias(&image2);
		} else {
			image1 = QImage();
			image2 = QImage();
		}
	}
	if (!image1.isNull()) {
		QPainter pr(this);
		pr.fillRect(0, 0, width(), height(), Qt::black);
		pr.drawImage(0, 0, image1);
		pr.drawImage(width() - w, 0, image2);
	}
}
