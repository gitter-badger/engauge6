#include "mmsubs.h"
#include <QImage>
#include <QPointF>
#include <qmath.h>

const double PI = 3.1415926535;

double angleBetweenVectors (const QPointF &v1,
                            const QPointF &v2)
{
  double v1Mag = qSqrt (v1.x() * v1.x() + v1.y() * v1.y());
  double v2Mag = qSqrt (v2.x() * v2.x() + v2.y() * v2.y());

  double angle = 0;
  if ((v1Mag > 0) || (v2Mag > 0)) {

    double cosArg = (v1.x() * v2.x() + v1.y() * v2.y()) / (v1Mag * v2Mag);
    cosArg = qMin (qMax (cosArg, -1.0), 1.0);
    angle = qAcos (cosArg);
  }

  return angle;
}

double angleFromVectorToVector (const QPointF &vFrom,
                                const QPointF &vTo)
{
  double angleFrom = qAtan2 (vFrom.y(), vFrom.x());
  double angleTo   = qAtan2 (vTo.y()  , vTo.x());

  // Rotate both angles to put from vector along x axis. Note that angleFrom-angleFrom is zero,
  // and angleTo-angleFrom is -pi to +pi radians
  double angleSeparation = angleTo - angleFrom;

  while (angleSeparation < -1.0 * PI) {
    angleSeparation += 2.0 * PI;
  }
  while (angleSeparation > PI) {
    angleSeparation -= 2.0 * PI;
  }

  return angleSeparation;
}

QRgb pixelRGB(const QImage &image, int x, int y)
{
  switch (image.depth())
  {
    case 1:
      return pixelRGB1(image, x, y);
    case 8:
      return pixelRGB8(image, x, y);
    default:
      return pixelRGB32(image, x, y);
  }
}

QRgb pixelRGB1(const QImage &image1Bit, int x, int y)
{
  unsigned int bit;
  if (image1Bit.format () == QImage::Format_MonoLSB) {
    bit = *(image1Bit.scanLine (y) + (x >> 3)) & (1 << (x & 7));
  } else {
    bit = *(image1Bit.scanLine (y) + (x >> 3)) & (1 << (7 - (x & 7)));
  }

  unsigned int tableIndex = ((bit == 0) ? 0 : 1);
  return image1Bit.color(tableIndex);
}

QRgb pixelRGB8(const QImage &image8Bit, int x, int y)
{
  unsigned int tableIndex = *(image8Bit.scanLine(y) + x);
  return image8Bit.color(tableIndex);
}

QRgb pixelRGB32(const QImage &image32Bit, int x, int y)
{
  unsigned int* p = (unsigned int *) image32Bit.scanLine(y) + x;
  return *p;
}

void projectPointOntoLine(double xCenter,
                          double yCenter,
                          double xStart,
                          double yStart,
                          double xStop,
                          double yStop,
                          double *xProjection,
                          double* yProjection)
{
  if (qAbs (yStart - yStop) < 0.000001) {

    // Special case - line segment is vertical
    *yProjection = yStart;
    double s = (xCenter - xStart) / (xStop - xStart);
    if (s < 0) {
      *xProjection = xStart;
    } else if (s > 1) {
      *xProjection = xStop;
    } else {
      *xProjection = (1.0 - s) * xStart + s * xStop;
    }
  } else {

    // General case - compute slope and intercept of line through (xCenter, yCenter)
    double slope = (xStop - xStart) / (yStart - yStop);
    double yintercept = yCenter - slope * xCenter;

    // Intersect center point line (slope-intercept form) with start-stop line (parametric form x=(1-s)*x1+s*x2, y=(1-s)*y1+s*y2)
    double s = (slope * xStart + yintercept - yStart) /
      (yStop - yStart + slope * (xStart - xStop));

    if (s < 0) {
      *xProjection = xStart;
      *yProjection = yStart;
    } else if (s > 1) {
      *xProjection = xStop;
      *yProjection = yStop;
    } else {
      *xProjection = (1.0 - s) * xStart + s * xStop;
      *yProjection = (1.0 - s) * yStart + s * yStop;
    }
  }
}

void setPixelRGB(QImage &image, int x, int y, QRgb q)
{
  switch (image.depth())
  {
    case 1:
      setPixelRGB1(image, x, y, q);
      return;
    case 8:
      setPixelRGB8(image, x, y, q);
      return;
    case 32:
      setPixelRGB32(image, x, y, q);
      return;
  }
}

void setPixelRGB1(QImage &image1Bit, int x, int y, QRgb q)
{
  for (int index = 0; index < image1Bit.colorCount(); index++) {
    if (q == image1Bit.color(index))
    {
      if (image1Bit.format () == QImage::Format_MonoLSB)
      {
        *(image1Bit.scanLine (y) + (x >> 3)) &= ~(1 << (x & 7));
        if (index > 0)
          *(image1Bit.scanLine (y) + (x >> 3)) |= index << (x & 7);
      }
      else
      {
        *(image1Bit.scanLine (y) + (x >> 3)) &= ~(1 << (7 - (x & 7)));
        if (index > 0)
          *(image1Bit.scanLine (y) + (x >> 3)) |= index << (7 - (x & 7));
      }
      return;
    }
  }
}

void setPixelRGB8(QImage &image8Bit, int x, int y, QRgb q)
{
  for (int index = 0; index < image8Bit.colorCount(); index++) {
    if (q == image8Bit.color(index))
    {
      *(image8Bit.scanLine(y) + x) = index;
      return;
    }
  }
}

void setPixelRGB32(QImage &image32Bit, int x, int y, QRgb q)
{
  int* p = (int *)image32Bit.scanLine(y) + x;
  *p = q;
}
