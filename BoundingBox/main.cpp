/* -*-c++-*- OpenSceneGraph Cookbook
 * Chapter 2 Recipe 3
 * Author: Wang Rui <wangray84 at gmail dot com>
*/
/**场景图中的每个节点都有自己的局部坐标系。平移和旋转转换节点时，
   意味着要更改其在转换节点父坐标系中的位置和姿态。这使得所有的
   转换都发生在局部空间中，而不是发生在世界空间中。应用于节点的
   矩阵也可以被视为将节点空间映射到其父空间的转置矩阵。要计算指
   定点的世界坐标，我们必须首先找出它所在的局部空间，然后获取节
   点的父节点、父节点的父节点，等等，直到我们到达场景根目录。然
   后，我们将有一个从根到包含该点的节点的节点路径。利用节点路径，
   我们可以乘以所有的转置矩阵，得到一个完整的局部到世界矩阵。
   父节点路径的收集是用 getParentalNodePaths()方法来完成的。它
   有多个路径，因为一个OSG节点可能有多个父节点。并且为了计
   算局部到世界的矩阵，使用osg::computeLocalToWorld()函数
   直接与节点路径作为参数。还有另一个函数称为osg::computeWorldToLocal()
   的函数，它用于计算的本地表示
 **/


/**
    Every node in the scene graph has its own local coordinate system. When you translate
    and rotate a transformation node, it means that you change its position and attitude in its
    parent's coordinate system. This makes all the transformations occur in local space rather
    than the world one. And the matrix applied to the node can also be treated as the transpose
    matrix that maps the node space to its parent's space.

    To compute the world coordinates of a specified point, we have to first find out the local
    space it lives in, and then fetch the node's parent, parent's parent, and so on, until we
    reach the scene root. Then we will have a node path from the root to the node containing
    the point. With the node path, we can multiply all the transpose matrices and get a complete
    local-to-world matrix.

    The collection of parental node paths is done with the getParentalNodePaths()
    method. It has multiple paths because an OSG node may have more than one parent
    node. And to compute the local-to-world matrix, use osg::computeLocalToWorld()
    function directly with the node path as argument. There is another function named
    osg::computeWorldToLocal(), which is for computing the local representation
    of a point in world space.
**/

#include <osg/ComputeBoundsVisitor>
#include <osg/ShapeDrawable>
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

class BoundingBoxCallback : public osg::NodeCallback
{
public:
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv )
    {
        osg::BoundingBox bb;
        for ( unsigned int i=0; i<_nodesToCompute.size(); ++i )
        {
            osg::Node* node = _nodesToCompute[i];
            osg::ComputeBoundsVisitor cbbv;
            node->accept( cbbv );

            osg::BoundingBox localBB = cbbv.getBoundingBox();
            osg::Matrix localToWorld = osg::computeLocalToWorld( node->getParent(0)->getParentalNodePaths()[0] );
            for ( unsigned int i=0; i<8; ++i )
                bb.expandBy( localBB.corner(i) * localToWorld );//expandby 扩充box到包含该点
        }

        osg::MatrixTransform* trans = static_cast<osg::MatrixTransform*>(node);
        trans->setMatrix(
            osg::Matrix::scale(bb.xMax()-bb.xMin(), bb.yMax()-bb.yMin(), bb.zMax()-bb.zMin()) *
            osg::Matrix::translate(bb.center()) );

        traverse(node, nv);
    }

    osg::NodePath _nodesToCompute;
};

osg::AnimationPath* createAnimationPath( float radius, float time )
{
    osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
    path->setLoopMode( osg::AnimationPath::LOOP );

    unsigned int numSamples = 32;
    float delta_yaw = 2.0f * osg::PI/((float)numSamples - 1.0f);
    float delta_time = time / (float)numSamples;
    for ( unsigned int i=0; i<numSamples; ++i )
    {
        float yaw = delta_yaw * (float)i;
        osg::Vec3 pos( sinf(yaw)*radius, cosf(yaw)*radius, 0.0f );
        osg::Quat rot( -yaw, osg::Z_AXIS );
        path->insert( delta_time * (float)i, osg::AnimationPath::ControlPoint(pos, rot) );
    }
    return path.release();
}

int main( int argc, char** argv )
{
    osg::ref_ptr<osg::MatrixTransform> cessna = new osg::MatrixTransform;
    cessna->addChild( osgDB::readNodeFile("cessna.osg.0,0,90.rot") );

    osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
    apcb->setAnimationPath( createAnimationPath(50.0f, 6.0f) );
    cessna->addUpdateCallback( apcb.get() );

    osg::ref_ptr<osg::MatrixTransform> dumptruck = new osg::MatrixTransform;
    dumptruck->addChild( osgDB::readNodeFile("dumptruck.osg") );
    dumptruck->setMatrix( osg::Matrix::translate(0.0f, 0.0f, -100.0f) );

    osg::ref_ptr<osg::MatrixTransform> models = new osg::MatrixTransform;
    models->addChild( cessna.get() );
    models->addChild( dumptruck.get() );
    models->setMatrix( osg::Matrix::translate(0.0f, 0.0f, 200.0f) );

    // World bounding box callback & node
    osg::ref_ptr<BoundingBoxCallback> bbcb = new BoundingBoxCallback;
    bbcb->_nodesToCompute.push_back( cessna.get() );
    bbcb->_nodesToCompute.push_back( dumptruck.get() );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( new osg::ShapeDrawable(new osg::Box) );

    osg::ref_ptr<osg::MatrixTransform> boundingBoxNode = new osg::MatrixTransform;
    boundingBoxNode->addChild( geode.get() );
    boundingBoxNode->addUpdateCallback( bbcb.get() );
    boundingBoxNode->getOrCreateStateSet()->setAttributeAndModes(
        new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE) );
    boundingBoxNode->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    // Build the scene
    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild( models.get() );
    root->addChild( osgDB::readNodeFile("lz.osg") );
    root->addChild( boundingBoxNode.get() );

    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );

    return viewer.run();
}
