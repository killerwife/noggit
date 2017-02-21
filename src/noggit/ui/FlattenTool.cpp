// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/FlattenTool.hpp>

#include <noggit/tool_enums.hpp>
#include <noggit/World.h>

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>


namespace ui
{
  FlattenTool::FlattenTool()
    : QWidget(nullptr)
    , _radius(10.0f)
    , _speed(2.0f)
    , _angle(45.0f)
    , _orientation(0.0f)
    , _flatten_type(eFlattenType_Linear)
    , _flatten_mode(eFlattenMode_Both)
  {
    setWindowTitle("Flatten / Blur");
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);

    auto layout (new QFormLayout (this));

    _type_button_box = new QButtonGroup (this);
    QRadioButton* radio_flat = new QRadioButton ("Flat");
    QRadioButton* radio_linear = new QRadioButton ("Linear");
    QRadioButton* radio_smooth = new QRadioButton ("Smooth");
    QRadioButton* radio_origin = new QRadioButton ("Origin");

    _type_button_box->addButton (radio_flat, (int)eFlattenType_Flat);
    _type_button_box->addButton (radio_linear, (int)eFlattenType_Linear);
    _type_button_box->addButton (radio_smooth, (int)eFlattenType_Smooth);
    _type_button_box->addButton (radio_origin, (int)eFlattenType_Origin);

    radio_linear->toggle();

    QGridLayout* flatten_type_layout (new QGridLayout (this));
    flatten_type_layout->addWidget (radio_flat, 0, 0);
    flatten_type_layout->addWidget (radio_linear, 0, 1);
    flatten_type_layout->addWidget (radio_smooth, 1, 0);
    flatten_type_layout->addWidget (radio_origin, 1, 1);

    QGroupBox* flatten_type_group (new QGroupBox ("Type"));
    flatten_type_group->setLayout (flatten_type_layout);
    layout->addRow (flatten_type_group);

    _radius_spin = new QDoubleSpinBox (this);
    _radius_spin->setRange (0.0f, 1000.0f);
    _radius_spin->setDecimals (2);
    _radius_spin->setValue (_radius);

    layout->addRow ("Radius:", _radius_spin);

    _radius_slider = new QSlider (Qt::Orientation::Horizontal, this);
    _radius_slider->setRange (0, 1000);
    _radius_slider->setSliderPosition (_radius);
    
    layout->addRow (_radius_slider);

    _speed_spin = new QDoubleSpinBox (this);
    _speed_spin->setRange (0.0f, 10.0f);
    _speed_spin->setDecimals (2);
    _speed_spin->setValue (_speed);

    layout->addRow ("Speed:", _speed_spin);

    _speed_slider = new QSlider (Qt::Orientation::Horizontal, this);
    _speed_slider->setRange (0, 10 * 100);
    _speed_slider->setSingleStep (50);
    _speed_slider->setSliderPosition (_speed * 100);

    layout->addRow(_speed_slider);
    
    layout->addRow(new QLabel("Flatten/Blur:"));

    QGridLayout* lock_checkbox_layout(new QGridLayout());
    lock_checkbox_layout->addWidget(_lock_up_checkbox = new QCheckBox(this), 0, 0);
    lock_checkbox_layout->addWidget(_lock_down_checkbox = new QCheckBox(this), 0, 1);

    _lock_up_checkbox->setChecked(true);
    _lock_up_checkbox->setText("up");
    _lock_up_checkbox->setToolTip("Raise the terrain when using the tool");
    _lock_down_checkbox->setChecked(true);
    _lock_down_checkbox->setText("down");
    _lock_down_checkbox->setToolTip("Lower the terrain when using the tool");

    layout->addRow(lock_checkbox_layout);

    layout->addRow(new QLabel("Flatten only:"));

    QGridLayout* angle_layout(new QGridLayout());

    angle_layout->addWidget(_orientation_dial = new QDial(this), 0, 0);
    _orientation_dial->setRange(0, 360);
    _orientation_dial->setWrapping(true);
    _orientation_dial->setSliderPosition(_orientation - 90); // to get ingame orientation
    _orientation_dial->setToolTip("Orientation");
    _orientation_dial->setSingleStep(10);    

    _angle_slider = new QSlider(this);
    _angle_slider->setRange(0, 89);
    _angle_slider->setSliderPosition(_angle);
    _angle_slider->setToolTip("Angle");
    angle_layout->addWidget(_angle_slider, 0, 1);
    
    _angle_group = new QGroupBox("Angled mode");
    _angle_group->setCheckable(true);
    _angle_group->setChecked(false);
    _angle_group->setLayout(angle_layout);
    layout->addRow(_angle_group);

    QFormLayout* lock_layout(new QFormLayout());
    lock_layout->addRow("X:", _lock_x = new QDoubleSpinBox(this));
    lock_layout->addRow("Z:", _lock_z = new QDoubleSpinBox(this));
    lock_layout->addRow("H:", _lock_h = new QDoubleSpinBox(this));

    _lock_group = new QGroupBox("Lock mode");
    _lock_group->setCheckable(true);
    _lock_group->setChecked(false);
    _lock_group->setLayout(lock_layout); 

    layout->addRow(_lock_group);    

    _lock_x->setRange(0.0, 34133.0);
    _lock_x->setDecimals(3);
    _lock_z->setRange(0.0, 34133.0);
    _lock_z->setDecimals(3);
    _lock_h->setRange(0.0, 34133.0);
    _lock_h->setMinimumWidth(30);

    connect ( _type_button_box, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked)
            , [&] (int id)
              {
                _flatten_type = id;
              }
            );

    connect ( _radius_spin, static_cast<void (QDoubleSpinBox::*) (double)> (&QDoubleSpinBox::valueChanged)
            , [&] (double v)
              {
                _radius = v;
                QSignalBlocker const blocker(_radius_slider);
                _radius_slider->setSliderPosition ((int)std::round (v));
               }
            );

    connect ( _radius_slider, static_cast<void (QSlider::*) (int)> (&QSlider::valueChanged)
            , [&] (int v)
              {
                _radius = v;
                 QSignalBlocker const blocker(_radius_spin);
                 _radius_spin->setValue(v);
              }
            );

    connect ( _speed_spin, static_cast<void (QDoubleSpinBox::*) (double)> (&QDoubleSpinBox::valueChanged)
              , [&] (double v)
                {
                  _speed = v;
                  QSignalBlocker const blocker(_speed_slider);
                  _speed_slider->setSliderPosition ((int)std::round (v * 100.0f));
                }
              );

    connect ( _speed_slider, static_cast<void (QSlider::*) (int)> (&QSlider::valueChanged)
              , [&] (int v)
                {
                  _speed = v / 100.0f;
                  QSignalBlocker const blocker(_speed_spin);
                  _speed_spin->setValue (_speed);
                }
              );

    connect( _lock_up_checkbox, static_cast<void (QCheckBox::*) (int)> (&QCheckBox::stateChanged)
             , [&] (int state) 
               {
                 if (state)
                 {
                   _flatten_mode |= eFlattenMode_Raise;
                 }
                 else
                 {
                   _flatten_mode &= eFlattenMode_Lower;
                 }
               }
           );

    connect( _lock_down_checkbox, static_cast<void (QCheckBox::*) (int)> (&QCheckBox::stateChanged)
             , [&] (int state) 
               {
                 if (state)
                 {
                   _flatten_mode |= eFlattenMode_Lower;
                 }
                 else
                 {
                   _flatten_mode &= eFlattenMode_Raise;
                 }
               }
           );

    connect ( _angle_slider, static_cast<void (QSlider::*) (int)> (&QSlider::valueChanged)
              , [&] (int v)
                {
                  _angle = v;
                }
              );

    connect ( _orientation_dial, static_cast<void (QDial::*) (int)> (&QDial::valueChanged)
              , [this] (int v)
                {
                  setOrientation(v + 90.0f);
                }
              );

    connect ( _lock_x, static_cast<void (QDoubleSpinBox::*) (double)> (&QDoubleSpinBox::valueChanged)
              , [&] (double v)
                {
                  _lock_pos.x = v;
                }
              );

    connect ( _lock_h, static_cast<void ( QDoubleSpinBox::* ) ( double )> ( &QDoubleSpinBox::valueChanged )
              , [&] (double v)
                {
                  _lock_pos.y = v;
                }
            );

    connect ( _lock_z, static_cast<void ( QDoubleSpinBox::* ) ( double )> ( &QDoubleSpinBox::valueChanged )
              , [&] (double v)
                {
                  _lock_pos.z = v;
                }
            );
  }

  void FlattenTool::flatten(math::vector_3d const& cursor_pos, float dt)
  {
    gWorld->flattenTerrain ( cursor_pos
                           , 1.f - pow (0.5f, dt *_speed)
                           , _radius
                           , _flatten_type
                           , _flatten_mode
                           , use_ref_pos() ? _lock_pos : cursor_pos
                           , math::degrees (angled_mode() ? _angle : 0.0f)
                           , math::degrees (angled_mode() ? _orientation : 0.0f)
                           );
  }

  void FlattenTool::blur(math::vector_3d const& cursor_pos, float dt)
  {
    gWorld->blurTerrain ( cursor_pos
                        , 1.f - pow (0.5f, dt * _speed)
                        , _radius
                        , _flatten_type
                        );
  }

  void FlattenTool::nextFlattenType()
  {
    _flatten_type = ( ++_flatten_type ) % eFlattenType_Count;
    _type_button_box->button (_flatten_type)->toggle();
  }

  void FlattenTool::nextFlattenMode()
  {
    _flatten_mode = std::max((int)eFlattenMode_Raise, (++_flatten_mode) % 4);
    
    QSignalBlocker const up_lock(_lock_up_checkbox);
    QSignalBlocker const down_lock(_lock_down_checkbox);
    _lock_up_checkbox->setChecked(_flatten_mode & eFlattenMode_Raise);
    _lock_down_checkbox->setChecked(_flatten_mode & eFlattenMode_Lower);
  }

  void FlattenTool::toggleFlattenAngle()
  {
    _angle_group->setChecked(!angled_mode());
  }

  void FlattenTool::toggleFlattenLock()
  {
    _lock_group->setChecked(!use_ref_pos());
  }

  void FlattenTool::lockPos (math::vector_3d const& cursor_pos)
  {
    _lock_pos = cursor_pos;
    _lock_x->setValue (_lock_pos.x);
    _lock_h->setValue (_lock_pos.y);
    _lock_z->setValue (_lock_pos.z);    

    if (!use_ref_pos())
    {
      toggleFlattenLock();
    }
  }

  void FlattenTool::changeRadius(float change)
  {
    _radius_spin->setValue (_radius + change);
  }

  void FlattenTool::changeSpeed(float change)
  {
    _speed_spin->setValue(_speed + change);
  }

  void FlattenTool::changeOrientation(float change)
  {
    setOrientation(_orientation + change);    
  }

  void FlattenTool::setOrientation (float orientation)
  {
    QSignalBlocker const blocker (_orientation_dial);

    _orientation = orientation;
    if (_orientation > 360.0f)
    {
      _orientation -= 360.0f;
    }
    else if (_orientation < 0.0f)
    {
      _orientation += 360.0f;
    }
    _orientation_dial->setSliderPosition(_orientation - 90.0f);
  }

  void FlattenTool::changeAngle(float change)
  {
    _angle = std::min(89.0f, std::max(0.0f, _angle + change));
    _angle_slider->setSliderPosition(_angle);
  }

  void FlattenTool::changeHeight(float change)
  {
    _lock_h->setValue(_lock_pos.y + change);
  }

  void FlattenTool::setRadius(float radius)
  {
    _radius_slider->setValue(radius);
  }
}


