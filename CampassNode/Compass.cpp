/* -*-c++-*- OpenSceneGraph Cookbook
 * Chapter 2 Recipe 10
 * Author: Wang Rui <wangray84 at gmail dot com>
*/

#include "Compass"

Compass::Compass()
{
}

Compass::Compass( const Compass& copy, osg::CopyOp copyop )
:   osg::Camera(copy, copyop),
    _plateTransform(copy._plateTransform),
    _needleTransform(copy._needleTransform),
    _mainCamera(copy._mainCamera)
{
}

Compass::~Compass()
{
}

void Compass::traverse( osg::NodeVisitor& nv )
{
    if ( _mainCamera.valid() && nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR )
    {
        osg::Matrix matrix = _mainCamera->getViewMatrix();
        matrix.setTrans( osg::Vec3() );
        
        osg::Vec3 northVec = osg::Z_AXIS * matrix;
        northVec.z() = 0.0f;
        northVec.normalize();
        
        osg::Vec3 axis = osg::Y_AXIS ^ northVec;
        float angle = atan2(axis.length(), osg::Y_AXIS*northVec);
        axis.normalize();
        
        if ( _plateTransform.valid() )
            _plateTransform->setMatrix( osg::Matrix::rotate(angle, axis) );
    }
    

//    While calling the traverse() method of its super class, the Compass class (and other
//    classes derived from osg::Group) will actually iterate each child and call the accept()
//    method on them, to make the traversal continue. But here, the work is done by directly
//    calling the accept() method on the transformation nodes _plateTransform and
//    _needleTransform. It means that these two nodes will be traversed as if they were
//    children of the compass. This sometimes brings flexibility


//    But be careful, without being added to the scene graph, a node will
//    lose some functionalities such as contributing to the bounding box
//    computation and executing update callbacks applied on it.


    _plateTransform->accept( nv );
    _needleTransform->accept( nv );
    osg::Camera::traverse( nv );
}
