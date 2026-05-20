
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
