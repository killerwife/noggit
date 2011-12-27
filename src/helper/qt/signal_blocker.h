#ifndef __SIGNAL_BLOCKER_H
#define __SIGNAL_BLOCKER_H

#include <QObject>

namespace helper
{
  namespace qt
  {
    class signal_blocker
    {
    public:
      signal_blocker (QObject* object)
        : _object (object)
        , _old_state (_object->blockSignals (true))
      { }

      signal_blocker (QObject& object)
        : _object (&object)
        , _old_state (_object->blockSignals (true))
      { }

      ~signal_blocker()
      {
        _object->blockSignals (_old_state);
      }

    private:
      QObject* _object;
      bool _old_state;
    };
  }
}

#endif
