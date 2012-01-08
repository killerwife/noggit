#include <noggit/ModelInstance.h>

#include <math/matrix_4x4.h>

#include <noggit/Log.h>
#include <noggit/Model.h> // Model, etc.
#include <noggit/World.h>
#include <noggit/mpq/file.h>

::math::vector_3d TransformCoordsForModel (const ::math::vector_3d& pIn)
{
  ::math::vector_3d lTemp (pIn);
  lTemp.y (pIn.z());
  lTemp.z (-pIn.y());
  return lTemp;
}

void DrawABox( ::math::vector_3d pMin, ::math::vector_3d pMax, ::math::vector_4d pColor, float pLineWidth )
{
  glEnable( GL_LINE_SMOOTH );
  glLineWidth( pLineWidth );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );

  glColor4fv( pColor );

  glBegin( GL_LINE_STRIP );
  glVertex3f( pMin.x(), pMax.y(), pMin.z() );
  glVertex3f( pMin.x(), pMin.y(), pMin.z() );
  glVertex3f( pMax.x(), pMin.y(), pMin.z() );
  glVertex3f( pMax.x(), pMin.y(), pMax.z() );
  glVertex3f( pMax.x(), pMax.y(), pMax.z() );
  glVertex3f( pMax.x(), pMax.y(), pMin.z() );
  glVertex3f( pMin.x(), pMax.y(), pMin.z() );
  glVertex3f( pMin.x(), pMax.y(), pMax.z() );
  glVertex3f( pMin.x(), pMin.y(), pMax.z() );
  glVertex3f( pMin.x(), pMin.y(), pMin.z() );
  glEnd();
  glBegin( GL_LINES );
  glVertex3f( pMin.x(), pMin.y(), pMax.z() );
  glVertex3f( pMax.x(), pMin.y(), pMax.z() );
  glEnd();
  glBegin( GL_LINES );
  glVertex3f( pMax.x(), pMax.y(), pMin.z() );
  glVertex3f( pMax.x(), pMin.y(), pMin.z() );
  glEnd();
  glBegin( GL_LINES );
  glVertex3f( pMin.x(), pMax.y(), pMax.z() );
  glVertex3f( pMax.x(), pMax.y(), pMax.z() );
  glEnd();
}

ModelInstance::ModelInstance (World* world)
  : _world (world)
{
}

ModelInstance::ModelInstance (World* world, Model *m)
  : model (m)
  , _world (world)
{
}

ModelInstance::ModelInstance (World* world, Model *m, noggit::mpq::file* f)
  : model (m)
  , _world (world)
{
  float ff[3];

  f->read(&d1, 4);
  f->read(ff,12);
  pos = ::math::vector_3d(ff[0],ff[1],ff[2]);
  f->read(ff,12);
  dir = ::math::vector_3d(ff[0],ff[1],ff[2]);
  int16_t scale;
  f->read( &scale, 2 );
  // scale factor - divide by 1024. blizzard devs must be on crack, why not just use a float?
  sc = scale / 1024.0f;
  nameID=0xFFFFFFFF;
}

ModelInstance::ModelInstance(World* world, Model *m, ENTRY_MDDF *d)
  : model (m)
  , _world (world)
{
  d1 = d->uniqueID;
  pos = ::math::vector_3d(d->pos[0],d->pos[1],d->pos[2]);
  dir = ::math::vector_3d(d->rot[0],d->rot[1],d->rot[2]);
  // scale factor - divide by 1024. blizzard devs must be on crack, why not just use a float?
  sc = d->scale / 1024.0f;
  nameID=0xFFFFFFFF;
}

void ModelInstance::init2(Model *m, noggit::mpq::file* f)
{
  nameID=0xFFFFFFFF;
  model = m;
  nameID = _world->selection_names().add( this );
  f->read(pos,12);
  const float temp (pos.z());
  pos.z (-pos.y());
  pos.y (temp);
  f->read(&w,4);
  f->read(dir,12);
  f->read(&sc,4);
  f->read(&d1,4);
  lcol = ::math::vector_3d ( ( ( d1 & 0xff0000 ) >> 16 ) / 255.0f
                           , ( ( d1 & 0x00ff00 ) >> 8 ) / 255.0f
                           , ( d1 & 0x0000ff ) / 255.0f
                           );
}

void ModelInstance::draw (bool draw_fog)
{
/*  float dist = ( pos - _world->camera ).length() - model->rad;

  if( dist > 2.0f * _world->modeldrawdistance )
    return;
  if( CheckUniques( d1 ) )
    return;*/

  if( !_world->frustum.intersectsSphere( pos, model->rad * sc ) )
    return;

  glPushMatrix();

  glTranslatef( pos.x(), pos.y(), pos.z() );
  glRotatef( dir.y() - 90.0f, 0.0f, 1.0f, 0.0f );
  glRotatef( -dir.x(), 0.0f, 0.0f, 1.0f );
  glRotatef( dir.z(), 1.0f, 0.0f, 0.0f );
  glScalef( sc, sc, sc );

  model->draw (draw_fog);

  if( _world->IsSelection( eEntry_Model ) && _world->GetCurrentSelection()->data.model->d1 == d1 )
  {
    if (draw_fog)
      glDisable( GL_FOG );

    glDisable( GL_LIGHTING );

    glDisable( GL_COLOR_MATERIAL );
    glActiveTexture( GL_TEXTURE0 );
    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    DrawABox( TransformCoordsForModel( model->header.VertexBoxMin ), TransformCoordsForModel( model->header.VertexBoxMax ), ::math::vector_4d( 1.0f, 1.0f, 1.0f, 1.0f ), 1.0f );
    DrawABox( TransformCoordsForModel( model->header.BoundingBoxMin ), TransformCoordsForModel( model->header.BoundingBoxMax ), ::math::vector_4d( 1.0f, 1.0f, 0.0f, 1.0f ), 1.0f );

    glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );
    glBegin( GL_LINES );
      glVertex3f( 0.0f, 0.0f, 0.0f );
      glVertex3f( model->header.VertexBoxMax.x() + model->header.VertexBoxMax.x() / 5.0f, 0.0f, 0.0f );
    glEnd();

    glColor4f( 0.0f, 1.0f, 0.0f, 1.0f );
    glBegin( GL_LINES );
      glVertex3f( 0.0f, 0.0f, 0.0f );
      glVertex3f( 0.0f, model->header.VertexBoxMax.z() + model->header.VertexBoxMax.z() / 5.0f, 0.0f );
    glEnd();

    glColor4f( 0.0f, 0.0f, 1.0f, 1.0f );
    glBegin( GL_LINES );
      glVertex3f( 0.0f, 0.0f, 0.0f );
      glVertex3f( 0.0f, 0.0f, model->header.VertexBoxMax.y() + model->header.VertexBoxMax.y() / 5.0f );
    glEnd();

    glActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_2D );

    glEnable( GL_LIGHTING );

    if (draw_fog)
      glEnable( GL_FOG );
  }

  glPopMatrix();
}

//! \todo  Get this drawn on the 2D view.
/*void ModelInstance::drawMapTile ()
{
  if(CheckUniques(d1))
    return;

  glPushMatrix();

  glTranslatef(pos.x/CHUNKSIZE, pos.z/CHUNKSIZE, pos.y);
  glRotatef(-90.0f, 1, 0, 0);
  glRotatef(dir.y - 90.0f, 0, 1, 0);
  glRotatef(-dir.x, 0, 0, 1);
  glRotatef(dir.z, 1, 0, 0);
  glScalef(1/CHUNKSIZE,1/CHUNKSIZE,1/CHUNKSIZE);
  glScalef(sc,sc,sc);

  model->draw ();

  glPopMatrix();
}*/

void ModelInstance::drawSelect ()
{
  /*float dist = ( pos - _world->camera ).length() - model->rad;

  if( dist > 2.0f * _world->modeldrawdistance )
    return;
  if( CheckUniques( d1 ) )
    return;*/

  if( !_world->frustum.intersectsSphere( pos, model->rad * sc ) )
    return;

  glPushMatrix();

  glTranslatef( pos.x(), pos.y(), pos.z() );
  glRotatef( dir.y() - 90.0f, 0.0f, 1.0f, 0.0f );
  glRotatef( -dir.x(), 0.0f, 0.0f, 1.0f );
  glRotatef( dir.z(), 1.0f, 0.0f, 0.0f );
  glScalef( sc, sc, sc );

  if( nameID == 0xFFFFFFFF )
    nameID = _world->selection_names().add( this );

  glPushName( nameID );

  model->drawSelect();

  glPopName();

  glPopMatrix();
}



ModelInstance::~ModelInstance()
{
  if( nameID != 0xFFFFFFFF )
  {
    //Log << "Destroying Selection " << nameID << "\n";
    _world->selection_names().del( nameID );
    nameID = 0xFFFFFFFF;
  }
}

void glQuaternionRotate(const ::math::vector_3d& vdir, float w)
{
  glMultMatrixf
    (::math::matrix_4x4::new_rotation_matrix (::math::quaternion (vdir, w)));
}

void ModelInstance::draw2 (const ::math::vector_3d& ofs, const float rot)
{
  ::math::vector_3d tpos(ofs + pos);
  ::math::rotate (ofs.x(), ofs.z(), &tpos.x(), &tpos.z(), rot);
  //if ( (tpos - _world->camera).length_squared() > (_world->doodaddrawdistance2*model->rad*sc) ) return;
  if (!_world->frustum.intersectsSphere(tpos, model->rad*sc)) return;

  glPushMatrix();

  glTranslatef(pos.x(), pos.y(), pos.z());
  ::math::vector_3d vdir(-dir.z(),dir.x(),dir.y());
  glQuaternionRotate(vdir,w);
  glScalef(sc,-sc,-sc);

  model->draw (_world);
  glPopMatrix();
}

void ModelInstance::draw2Select (const ::math::vector_3d& ofs, const float rot)
{
  ::math::vector_3d tpos(ofs + pos);
  ::math::rotate (ofs.x(), ofs.z(), &tpos.x(), &tpos.z(), rot);
  if ( (tpos - _world->camera).length_squared() > ((doodaddrawdistance*doodaddrawdistance)*model->rad*sc) ) return;
  if (!_world->frustum.intersectsSphere(tpos, model->rad*sc)) return;

  glPushMatrix();

  glTranslatef(pos.x(), pos.y(), pos.z());
  ::math::vector_3d vdir(-dir.z(),dir.x(),dir.y());
  glQuaternionRotate(vdir,w);
  glScalef(sc,-sc,-sc);

  model->drawSelect();
  glPopMatrix();
}

void ModelInstance::resetDirection(){
  dir.x (0);
  //dir.y=0; only reset incline
  dir.z (0);
}

