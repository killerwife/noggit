// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#pragma once

#include <math/vector_4d.hpp>
#include <noggit/AppState.h>
#include <noggit/Selection.h>
#include <noggit/tool_enums.hpp>

class UIFrame;
class World;

enum eViewMode
{
  eViewMode_Minimap,
  eViewMode_2D,
  eViewMode_3D
};

class MapView : public AppState
{
private:
  bool _mod_alt_down = false;
  bool _mod_ctrl_down = false;
  bool _mod_shift_down = false;
  bool _mod_space_down = false;

  float _camera_ah;
  float _camera_av;
  math::vector_3d _camera_lookat;
  float _2d_zoom = 0.25f;
  float moving, strafing, updown, mousedir, movespd, turn, lookat;
  math::vector_3d _cursor_pos;
  bool key_w;
  bool look;
  bool _GUIDisplayingEnabled;

  bool _draw_contour = false;
  bool _draw_mfbo = false;
  bool _draw_wireframe = false;
  bool _draw_lines = false;
  bool _draw_terrain = true;
  bool _draw_wmo = true;
  bool _draw_water = true;
  bool _draw_wmo_doodads = true;
  bool _draw_models = true;
  bool _draw_model_animations = false;

  void save();
  void savecurrent();
  void saveall();

  float lastBrushUpdate;

  selection_result intersect_result(bool terrain_only);
  void doSelection(bool selectTerrainOnly);
  void update_cursor_pos();

  int mViewMode;

  void displayViewMode_2D(float t, float dt);
  void displayViewMode_3D(float t, float dt);

  void displayGUIIfEnabled();

  void createGUI();

  float mTimespeed;

  void checkWaterSave();

public:
  math::vector_4d cursor_color = math::vector_4d(1.0f, 1.0f, 1.0f, 1.0f);
  int cursor_type = 1;

  MapView(float ah0, float av0, math::vector_3d camera_lookat);
  ~MapView();

  void tick(float t, float dt);
  void display(float t, float dt);

  void mousemove(SDL_MouseMotionEvent *e);
  virtual void keyReleaseEvent (SDL_KeyboardEvent*) override;
  virtual void keyPressEvent (SDL_KeyboardEvent*) override;
  virtual void mouseReleaseEvent (SDL_MouseButtonEvent*) override;
  virtual void mousePressEvent (SDL_MouseButtonEvent*) override;
  void resizewindow();

  void quit();
  void quitask();
  void inserObjectFromExtern(int model);
  void selectModel(selection_type entry);

  void set_editing_mode (editing_mode);
};
