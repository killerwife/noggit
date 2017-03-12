// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <noggit/ui/widget.hpp>

#include <math/vector_4d.hpp>

#include <QtWidgets/QWidget>
#include <QDockWidget>

class UICursorSwitcher : public noggit::ui::widget
{
public:
  UICursorSwitcher(math::vector_4d& color, int& cursor_type);
};
