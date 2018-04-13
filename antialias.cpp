#include "antialias.h"

#include <QImage>
#include <stdint.h>
#include <functional>
#include <QDebug>

namespace image {

static void antialias_(int length, uint8_t const *line0, uint8_t const *line1, uint8_t const *line2, std::function<void(int pos, uint8_t v)> writer)
{
	for (int pos = 0; pos < length; pos++) {
		if (line1[pos + 1] != line1[pos + 2]) {
			int a = (line1[pos + 1] + line1[pos + 2]) / 2;
			int n1 = 0;
			int n2 = 0;
			if (line1[pos + 1] < a) {
				if (line0[pos + 2] < a || line2[pos + 2] < a) {
					for (int i = pos; i > 0; i--) {
						if (line1[i + 1] > a) break;
						if (line2[i + 1] < a) break;
						n1++;
					}
				}
				if (line0[pos] > a || line2[pos] > a) {
					for (int i = pos + 1; i < length; i++) {
						if (line1[i + 1] < a) break;
						if (line0[i + 1] > a) break;
						n2++;
					}
				}
			} else {
				if (line0[pos + 2] > a || line2[pos + 2] > a) {
					for (int i = pos; i > 0; i--) {
						if (line1[i + 1] < a) break;
						if (line0[i + 1] > a) break;
						n1++;
					}
				}
				if (line0[pos] < a || line2[pos] < a) {
					for (int i = pos + 1; i < length; i++) {
						if (line1[i + 1] > a) break;
						if (line2[i + 1] < a) break;
						n2++;
					}
				}
			}
			n1 /= 2;
			if (n1 > 0) {
				int b = line1[pos + 1 - n1];
				for (int i = 0; i < n1; i++) {
					writer(pos - i, a + (b - a) * (i + 1) / (n1 + 1));
				}
			}
			n2 /= 2;
			if (n2 > 0) {
				int b = line1[pos + 1 + n2];
				for (int i = 0; i < n2; i++) {
					writer(pos + i + 1, a + (b - a) * (i + 1) / (n2 + 1));
				}
				pos += n2;
			}
		}
	}
}

static void antialias_gray8(QImage *image)
{
	int w = image->width();
	int h = image->height();

	int span = w > h ? w : h;
	uint8_t *tmp0 = (uint8_t *)alloca(span + 2);
	uint8_t *tmp1 = (uint8_t *)alloca(span + 2);
	uint8_t *tmp2 = (uint8_t *)alloca(span + 2);

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

	auto Do = [&](int length, int rows, std::function<void(int, uint8_t *)> reader, std::function<void(int, uint8_t)> writer){
		for (row = 0; row < rows; row++) {
			if (row == 0) {
				reader(row, tmp0);
			} else {
				memcpy(tmp0, tmp1, length + 2);
			}
			reader(row, tmp1);
			if (row + 1 < rows) {
				reader(row + 1, tmp2);
			}
			antialias_(length, tmp0, tmp1, tmp2, writer);
			antialias_(length, tmp2, tmp1, tmp0, writer);
		}
	};

	Do(w, h, ReaderH, WriterH);
	Do(h, w, ReaderV, WriterV);
}

static void antialias_rgb888(QImage *image)
{
	int w = image->width();
	int h = image->height();

	int span = w > h ? w : h;
	uint8_t *tmp0 = (uint8_t *)alloca(span + 2);
	uint8_t *tmp1 = (uint8_t *)alloca(span + 2);
	uint8_t *tmp2 = (uint8_t *)alloca(span + 2);

	int row;
	int byte_offset;

	auto ReaderH = [&](int line, uint8_t *out){
		uint8_t const *src = (uint8_t const *)image->scanLine(line);
		for (int i = 0; i < w; i++) {
			out[i + 1] = src[i * 3 + byte_offset];
		}
		out[0] = tmp0[1];
		out[w + 1] = tmp0[w];
	};

	auto WriterH = [&](int pos, uint8_t v){
		uint8_t *ptr = (uint8_t *)image->scanLine(row) + pos * 3;
		ptr[byte_offset] = v;
	};

	auto ReaderV = [&](int line, uint8_t *out){
		for (int y = 0; y < h; y++) {
			uint8_t const *src = (uint8_t const *)image->scanLine(y) + line * 3;
			out[y + 1] = src[byte_offset];
		}
		out[0] = tmp0[1];
		out[h + 1] = tmp0[h];
	};

	auto WriterV = [&](int pos, uint8_t v){
		uint8_t *ptr = (uint8_t *)image->scanLine(pos) + row * 3;
		ptr[byte_offset] = v;
	};

	auto Do = [&](int length, int rows, std::function<void(int, uint8_t *)> reader, std::function<void(int, uint8_t)> writer){
		for (row = 0; row < rows; row++) {
			if (row == 0) {
				reader(row, tmp0);
			} else {
				memcpy(tmp0, tmp1, length + 2);
			}
			reader(row, tmp1);
			if (row + 1 < rows) {
				reader(row + 1, tmp2);
			}
			antialias_(length, tmp0, tmp1, tmp2, writer);
			antialias_(length, tmp2, tmp1, tmp0, writer);
		}
	};

	byte_offset = 0; // red
	Do(w, h, ReaderH, WriterH);
	Do(h, w, ReaderV, WriterV);
	byte_offset++; // green
	Do(w, h, ReaderH, WriterH);
	Do(h, w, ReaderV, WriterV);
	byte_offset++; // blue
	Do(w, h, ReaderH, WriterH);
	Do(h, w, ReaderV, WriterV);
}

bool antialias(QImage *image)
{
	if (!image) {
		qDebug() << "antialias: Null pointer.";
		return false;
	}
	int w = image->width();
	int h = image->height();
	if (w < 1 || h < 1) {
		qDebug() << "antialias: Empty image.";
		return false;
	}

	if (image->format() == QImage::Format_Grayscale8) {
		antialias_gray8(image);
		return true;
	}

	if (image->format() == QImage::Format_RGB888) {
		antialias_rgb888(image);
		return true;
	}

	qDebug() << "antialias: Unsupported image format.";
	return false;
}

} // namespace image

