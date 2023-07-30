/* -*-c++-*- OpenSceneGraph Cookbook
 * Chapter 4 Recipe 3
 * Author: Wang Rui <wangray84 at gmail dot com>
*/

#include <osg/Texture2D>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>

#include "CommonFunctions"

const double radius_earth = 6378.137;
const double radius_sun = 695990.0;
const double AU = 149697900.0;


//Is it possible to draw the real sun, earth, mars, and even the solar system in OpenGL and
//OSG? The answer is absolutely yes. But you may encounter some problems while actually
//working on this topic.
//The most serious one is the computation of near and far planes. As many early devices have
//only a 16-bit depth buffer or 24-bit one, you may not be able to maintain a huge distance (in
//the solar system) between the near and the far plane. If you force setting the near plane to a
//small value and the far plane to a very large one, the rendering of nearby objects will cause
//the classic Z-fighting problem because there is not enough precision to resolve the distance.
//The explaination of Z-fighting can be found at: http://en.wikipedia.org/wiki/Zfighting.

//The best solution is to buy a new graphics card supporting a 32-bit buffer. But to keep the
//program portable, we had better find some other solutions, for instance, the depth partition
//algorithm in this recipe. To introduce this algorithm in one sentence—it partitions the scene
//into several parts and renders them separately. Every part has its own near and far values
//and the distance between them is short enough to avoid the precision problem.

osg::Node* createScene()
{
    // Create the earth
    osg::ref_ptr<osg::ShapeDrawable> earth_sphere = new osg::ShapeDrawable;
    earth_sphere->setShape( new osg::Sphere(osg::Vec3(), radius_earth) );

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage( osgDB::readImageFile("Images/land_shallow_topo_2048.jpg") );

    osg::ref_ptr<osg::Geode> earth_node = new osg::Geode;
    earth_node->addDrawable( earth_sphere.get() );
    earth_node->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get() );

    // Create the sun
    osg::ref_ptr<osg::ShapeDrawable> sun_sphere = new osg::ShapeDrawable;
    sun_sphere->setShape( new osg::Sphere(osg::Vec3(), radius_sun) );
    sun_sphere->setColor( osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) );

    osg::ref_ptr<osg::Geode> sun_geode = new osg::Geode;
    sun_geode->addDrawable( sun_sphere.get() );

    osg::ref_ptr<osg::MatrixTransform> sun_node = new osg::MatrixTransform;
    sun_node->setMatrix( osg::Matrix::translate(0.0, AU, 0.0) );
    sun_node->addChild( sun_geode.get() );

    // Create the scene graph
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild( earth_node.get() );
    root->addChild( sun_node.get() );
    return root.release();
}

int main( int argc, char** argv )
{
    osg::ArgumentParser arguments(&argc,argv);

    double zNear = 1.0, zMid = 1e4, zFar = 2e8;
    arguments.read( "--depth-partition", zNear, zMid, zFar );

    osg::ref_ptr<osgViewer::DepthPartitionSettings> dps = new osgViewer::DepthPartitionSettings;
    dps->_mode = osgViewer::DepthPartitionSettings::FIXED_RANGE;
    dps->_zNear = zNear;
    dps->_zMid = zMid;
    dps->_zFar = zFar;

    osgViewer::Viewer viewer;
    viewer.getCamera()->setClearColor( osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f) );
    viewer.setSceneData( createScene() );
    viewer.setUpDepthPartition(dps.get());
    viewer.setCameraManipulator( new osgGA::TrackballManipulator );
    viewer.getCameraManipulator()->setHomePosition(
        osg::Vec3d(0.0,-12.5*radius_earth,0.0), osg::Vec3d(), osg::Vec3d(0.0,0.0,1.0) );
    return viewer.run();
}
