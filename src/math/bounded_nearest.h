// bounded_nearest.h is part of Noggit3, licensed via GNU General Public License (version 3).
// Bernd Lörwald <bloerwald+noggit@googlemail.com>

#pragma once

#include <cmath>
#include <QtGlobal>
#include <limits>

namespace math
{
  template<typename U, typename T>
  U bounded_nearest (const T& value)
  {
    return qBound ( T (std::numeric_limits<U>::min())
                  , T (value > T() ? ceil (value) : floor (value))
                  , T (std::numeric_limits<U>::max())
                  );
  }
}
