// This file is part of Noggit3, licensed under GNU General Public License (version 3).

#include <noggit/ui/MapViewGUI.h>

#include <sstream>
#include <algorithm>
#include <vector>

#include <noggit/DBC.h>
#include <noggit/Environment.h>
#include <noggit/MapChunk.h>
#include <noggit/MapView.h>
#include <noggit/application.h> // app.getStates(), gPop, app.getArial14(), arial...
#include <noggit/Project.h>
#include <noggit/ui/CurrentTexture.h>
#include <noggit/ui/DetailInfos.h> // ui::detail_infos
#include <noggit/ui/FlattenTool.hpp>
#include <noggit/ui/Help.h>
#include <noggit/ui/shader_tool.hpp>
#include <noggit/ui/terrain_tool.hpp>
#include <noggit/ui/texturing_tool.hpp>
#include <noggit/ui/TexturePicker.h> //
#include <noggit/ui/TexturingGUI.h>
#include <noggit/ui/ZoneIDBrowser.h> //
#include <noggit/ui/Water.h> //
#include <noggit/ui/ObjectEditor.h>
#include <noggit/Video.h> // video
#include <noggit/WMOInstance.h>
#include <noggit/World.h>
#include <noggit/texture_set.hpp>
#include <noggit/map_index.hpp>
#include <noggit/Misc.h>

#include <noggit/ModelManager.h>

#include <noggit/Settings.h>



UIMapViewGUI::UIMapViewGUI (MapView *setMapview)
  : UIFrame(0.0f, 0.0f, (float)video::width, (float)video::height)
{
  objectEditor = new UIObjectEditor(setMapview);
  objectEditor->hide();

  _terrain = new QDockWidget ("Raise / Lower", setMapview);
  _terrain->setFeatures ( QDockWidget::DockWidgetMovable
                        | QDockWidget::DockWidgetFloatable
                        );
  _terrain->setWidget (terrainTool = new ui::terrain_tool());
  setMapview->_main_window->addDockWidget (Qt::RightDockWidgetArea, _terrain);


  flattenTool = new ui::FlattenTool();
  flattenTool->hide();

  shaderTool = new ui::shader_tool(setMapview->cursor_color);
  shaderTool->hide();

  texturingTool = new ui::texturing_tool (&setMapview->_camera.position);
  texturingTool->hide();

  guiCurrentTexture = new noggit::ui::current_texture(TexturePalette);

  TexturePalette = UITexturingGUI::createTexturePalette(guiCurrentTexture);
  TexturePalette->hide();
  addChild(TexturePalette);
  addChild(UITexturingGUI::createTilesetLoader());
  addChild(UITexturingGUI::createTextureFilter());

  // DetailInfoWindow
  guidetailInfos = new ui::detail_infos(1.0f, video::height - 282.0f, 600.0f, 250.0f);
  guidetailInfos->hide();

  // ZoneIDBrowser
  ZoneIDBrowser = new ui::zone_id_browser();
  ZoneIDBrowser->hide();

  TexturePicker = new UITexturePicker ( video::width / 2 - 100.0f, video::height / 2 - 100.0f, 490.0f, 170.0f
                                      , guiCurrentTexture
                                      );
  TexturePicker->hide();
  TexturePicker->movable(true);
  addChild(TexturePicker);

  _help = new UIHelp();

  guiWater = new UIWater();
  guiWater->hide();
  guiWater->movable(true);
  addChild(guiWater);
}
