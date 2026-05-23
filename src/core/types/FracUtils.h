
#pragma once
#include "boost/rational.hpp"
#include <QObject>
#include <QString>

using Frac = boost::rational<int32_t>;

inline QString StringFromFrac(Frac frac)
{
    int32_t num = frac.numerator();
    int32_t denum = frac.denominator();

    if (denum == 1)
        return QString::number(num);

    return QString::number(num) + "/" + QString::number(denum);
}

inline Frac FracFromFloat(float f)
{
    return Frac((int)(f * 1000), 1000);
}

inline QString MixedNumberFromFrac(Frac frac)
{
    int32_t num = frac.numerator();
    int32_t denum = frac.denominator();
    int whole = (int)(num / denum);
    Frac f = Frac(num % denum, denum);
    QString wholePart = whole == 0 ? "" : QString::number(whole);
    QString fracPart =  (num % denum) == 0 ? "" : QString::number(num % denum) + "/" + QString::number(denum);
    return wholePart + " " + fracPart;
}

inline Frac FracFromString(const QString& str)
{
    QStringList parts = str.trimmed().split(' ', Qt::SkipEmptyParts);
    if (parts.size() == 2) {
        // mixed number: "2 1/2"
        int whole = parts[0].toInt();
        QStringList fracParts = parts[1].split('/');
        int num = fracParts[0].toInt();
        int den = fracParts[1].toInt();
        return Frac(whole * den + num, den);
    }
    if (parts[0].contains('/')) {
        // improper fraction: "5/2"
        QStringList fracParts = parts[0].split('/');
        return Frac(fracParts[0].toInt(), fracParts[1].toInt());
    }
    return Frac(parts[0].toInt(), 1);
}
