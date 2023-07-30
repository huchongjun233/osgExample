/* -*-c++-*- OpenSceneGraph Cookbook
 * Chapter 4 Recipe 5
 * Author: Wang Rui <wangray84 at gmail dot com>
*/

#include <osg/Camera>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgViewer/CompositeViewer>

#include "CommonFunctions"


//An OSG scene viewer can have only one view (osgViewer::Viewer) or multiple views
//(osgViewer::CompositeViewer). Every view has its own scene graph, camera
//manipulator, and event handlers. A view can have one main camera, which is controlled by
//the manipulator, and some slave cameras with customized offsets to the main one (see the
//second recipe in this chapter). Cameras can also be added as a normal node into the scene
//graph (see the fourth recipe). It will multiply or reset current view and projection matrices, and
//use current context or choose a different one to render the sub-graph.


//The camera must be attached with a graphics context to create and enable the OpenGL
//rendering window. If different screen numbers are specified while creating the contexts,
//we may even enable the cameras to work in a multi-monitor environment (see the first
//recipe). Multiple cameras can share one graphics context and use setViewport()
//and setRenderOrder() to set the rendering range and orders.

//Thus, to design an application with multiple windows and multiple scenes, we can either make
//use of the composite viewer or add more cameras in a single viewer framework. The former
//provides event handlers and manipulators for each view to update user data. But the latter
//could also implement the similar work with node callbacks. At last, it's up to you to choose a
//view/camera framework to create multi-windows applications.


osgViewer::View* create2DView( int x, int y, int width, int height,
                               const osg::Vec3& lookDir, const osg::Vec3& up,
                               osg::GraphicsContext* gc, osg::Node* scene )
{
    osg::ref_ptr<osgViewer::View> view = new osgViewer::View;
    view->getCamera()->setGraphicsContext( gc );
    view->getCamera()->setViewport( x, y, width, height );
    view->setSceneData( scene );

    osg::Vec3 center = scene->getBound().center();
    double radius = scene->getBound().radius();
    view->getCamera()->setViewMatrixAsLookAt( center - lookDir*(radius*3.0), center, up );
    view->getCamera()->setProjectionMatrixAsPerspective(
        30.0f, static_cast<double>(width)/static_cast<double>(height), 1.0f, 10000.0f );
    return view.release();
}

int main( int argc, char** argv )
{
    osg::ArgumentParser arguments( &argc, argv );

    osg::ref_ptr<osg::Node> scene = osgDB::readNodeFiles( arguments );
    if ( !scene ) scene = osgDB::readNodeFile("cessna.osg");

    unsigned int width = 800, height = 600;
    osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if ( wsi )
        wsi->getScreenResolution( osg::GraphicsContext::ScreenIdentifier(0), width, height );

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = 0;
    traits->y = 0;
    traits->width = width;
    traits->height = height;
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;

    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext( traits.get() );
    if ( !gc || !scene ) return 1;

    int w_2 = width/2, h_2 = height/2;
    osg::ref_ptr<osgViewer::View> top = create2DView(
        0, h_2, w_2, h_2,-osg::Z_AXIS, osg::Y_AXIS, gc.get(), scene.get());
    osg::ref_ptr<osgViewer::View> front = create2DView(
        w_2, h_2, w_2, h_2, osg::Y_AXIS, osg::Z_AXIS, gc.get(), scene.get());
    osg::ref_ptr<osgViewer::View> left = create2DView(
        0, 0, w_2, h_2, osg::X_AXIS, osg::Z_AXIS, gc.get(), scene.get());

    osg::ref_ptr<osgViewer::View> mainView = new osgViewer::View;
    mainView->getCamera()->setGraphicsContext( gc.get() );
    mainView->getCamera()->setViewport( w_2, 0, w_2, h_2 );
    mainView->setSceneData( scene.get() );
    mainView->setCameraManipulator( new osgGA::TrackballManipulator );

    osgViewer::CompositeViewer viewer;
    viewer.addView( top.get() );
    viewer.addView( front.get() );
    viewer.addView( left.get() );
    viewer.addView( mainView.get() );


//    Start the simulation. Here, we don't use the viewer.run() method but instead
//    write a simple loop that calls the frame() method all the time. That's because the
//    run() method will automatically apply a trackball manipulator to every view unless it
//    already has a valid one. As we don't want the 2D views to be controlled improperly by
//    manipulators, we have to avoid using the run() method in this case:
    while ( !viewer.done() )
    {
        viewer.frame();
    }
    return 0;
}
