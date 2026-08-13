#include "ui/MoneyFormatter.h"

#include <QtGlobal>

namespace pos::ui
{

QString formatMoney(Money amount)
{
    const Money rubles = amount / 100;
    const Money kopecks = amount % 100;

    return QStringLiteral("%1,%2 ₽")
        .arg(static_cast<qlonglong>(rubles))
        .arg(
            static_cast<qlonglong>(kopecks),
            2,
            10,
            QLatin1Char('0'));
}

}