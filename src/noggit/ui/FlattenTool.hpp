// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/vector_3d.hpp>

#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>


namespace ui
{
  class FlattenTool : public QWidget
  {  
  public:
    FlattenTool();

    void flatten(math::vector_3d const& cursor_pos, float dt);
    void blur(math::vector_3d const& cursor_pos, float dt);

    void nextFlattenType();
    void nextFlattenMode();
    void toggleFlattenAngle();
    void toggleFlattenLock();
    void lockPos (math::vector_3d const& cursor_pos);

    void changeRadius(float change);
    void changeSpeed(float change);
    void changeOrientation(float change);
    void changeAngle(float change);
    void changeHeight(float change);

    void setRadius(float radius);
    void setOrientation(float orientation);

    float brushRadius() const { return _radius; }
    float angle() const { return _angle; }
    float orientation() const { return _orientation; }
    bool angled_mode() const { return _angle_group->isChecked(); }
    bool use_ref_pos() const  { return _lock_group->isChecked(); }
    math::vector_3d ref_pos() const { return _lock_pos; }

  private:

    float _radius;
    float _speed;
    float _angle;
    float _orientation;

    math::vector_3d _lock_pos;

    int _flatten_type;
    int _flatten_mode;

  private:
    QButtonGroup* _type_button_box;
    QSlider* _radius_slider;
    QSlider* _speed_slider;
    QDoubleSpinBox* _radius_spin;
    QDoubleSpinBox* _speed_spin;

    QGroupBox* _angle_group;
    QSlider* _angle_slider;
    QDial* _orientation_dial;

    QGroupBox* _lock_group;
    QDoubleSpinBox* _lock_x;
    QDoubleSpinBox* _lock_z;
    QDoubleSpinBox* _lock_h;

    QCheckBox* _angle_checkbox;
    QCheckBox* _lock_checkbox;
    QCheckBox* _lock_up_checkbox;
    QCheckBox* _lock_down_checkbox;
  };
}

