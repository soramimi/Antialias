#include "antialias.h"

#include <QImage>
#include <stdint.h>
#include <functional>
#include <QDebug>

namespace image {

void antialias(QImage *image)
{
	if (!image) return;

	int w = image->width();
	int h = image->height();
	if (w < 1 || h < 1) return;

	if (image->format() != QImage::Format_Grayscale8) {
		qDebug() << "This image format is not supported yet.";
	}

	int span = w > h ? w : h;
	uint8_t *tmp0 = (uint8_t *)alloca(span + 2);
	uint8_t *tmp1 = (uint8_t *)alloca(span + 2);
	uint8_t *tmp2 = (uint8_t *)alloca(span + 2);

	auto Process = [](int length, uint8_t const *line0, uint8_t const *line1, uint8_t const *line2, std::function<void(int pos, uint8_t v)> writer){
		for (int pos = 0; pos < length; pos++) {
			int v = (line1[pos + 1] + line1[pos + 2]) / 2;
			if ((line0[pos + 1] < v && line0[pos + 2] < v) || (line0[pos + 1] > v && line0[pos + 2] > v)) {
				int n1, n2;
				n1 = n2 = 0;
				if (line1[pos + 1] < v) {
					for (int i = pos; i > 0; i--) {
						if (line1[i + 1] >= v) break;
						if (line2[i + 1] <= v) break;
						n1++;
					}
					for (int i = pos + 1; i < length; i++) {
						if (line1[i + 1] <= v) break;
						if (line0[i + 1] >= v) break;
						n2++;
					}
				} else {
					for (int i = pos; i > 0; i--) {
						if (line1[i + 1] <= v) break;
						if (line0[i + 1] >= v) break;
						n1++;
					}
					for (int i = pos + 1; i < length; i++) {
						if (line1[i + 1] >= v) break;
						if (line2[i + 1] <= v) break;
						n2++;
					}
				}
				n1 /= 2;
				if (n1 > 0) {
					int a = v;
					int b = line1[pos + 1 - n1];
					for (int i = 0; i < n1; i++) {
						int v = a + (b - a) * (i + 1) / (n1 + 1);
						writer(pos - i, v);
					}
				}
				n2 /= 2;
				if (n2 > 0) {
					int a = v;
					int b = line1[pos + 1 + n2];
					for (int i = 0; i < n2; i++) {
						int v = a + (b - a) * (i + 1) / (n2 + 1);
						writer(pos + i + 1, v);
					}
				}
				pos += n2;
			}
		}
	};

	int row;

	auto ReaderH = [&](int line, uint8_t *out){
		memcpy(out + 1, image->scanLine(line), w);
		out[0] = tmp0[1];
		out[w + 1] = tmp0[w];
	};

	auto WriterH = [&](int pos, uint8_t v){
		image->scanLine(row)[pos] = v;
	};

	auto ReaderV = [&](int line, uint8_t *out){
		for (int y = 0; y < h; y++) {
			out[y + 1] = ((uint8_t const *)image->scanLine(y))[line];
		}
		out[0] = tmp0[1];
		out[h + 1] = tmp0[h];
	};

	auto WriterV = [&](int pos, uint8_t v){
		image->scanLine(pos)[row] = v;
	};

	auto Do = [&](int length, int rows, std::function<void(int, uint8_t *)> loader, std::function<void(int, uint8_t)> writer){
		for (row = 0; row < rows; row++) {
			if (row == 0) {
				loader(row, tmp0);
			} else {
				memcpy(tmp0, tmp1, length + 2);
			}
			loader(row, tmp1);
			if (row + 1 < rows) {
				loader(row + 1, tmp2);
			}
			Process(length, tmp0, tmp1, tmp2, writer);
			Process(length, tmp2, tmp1, tmp0, writer);
		}
	};

	Do(w, h, ReaderH, WriterH);
	Do(h, w, ReaderV, WriterV);
}

} // namespace image

