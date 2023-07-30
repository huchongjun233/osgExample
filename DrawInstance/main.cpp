/* -*-c++-*- OpenSceneGraph Cookbook
 * Chapter 3 Recipe 10
 * Author: Wang Rui <wangray84 at gmail dot com>
*/

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

#include "CommonFunctions"

const char* vertCode = {
    "uniform sampler2D defaultTex;\n"
    "const float PI2 = 6.2831852;\n"
    "void main()\n"
    "{\n"
    "    float r = float(gl_InstanceID) / 256.0;\n"
    "    vec2 uv = vec2(fract(r), floor(r) / 256.0);\n"
    "    vec4 pos = gl_Vertex + vec4(uv.s * 384.0, 32.0 * sin(uv.s * PI2), uv.t * 384.0, 1.0);\n"
    "    gl_FrontColor = texture2D(defaultTex, uv);\n"
    "    gl_Position = gl_ModelViewProjectionMatrix * pos;\n"
    "}\n"
};

osg::Geometry* createInstancedGeometry( unsigned int numInstances )
{
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(4);
    (*vertices)[0].set(-0.5f, 0.0f,-0.5f );
    (*vertices)[1].set( 0.5f, 0.0f,-0.5f );
    (*vertices)[2].set( 0.5f, 0.0f, 0.5f );
    (*vertices)[3].set(-0.5f, 0.0f, 0.5f );

    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
    geom->setUseDisplayList( false );
    geom->setUseVertexBufferObjects( true );
    geom->setVertexArray( vertices.get() );


//    Using the draw instanced extension may be easier than you think. Either the
//    osg::DrawArrays or osg::DrawElements* class has a numInstances
//    argument (at the last of the argument list) that indicates the number of instanced
//    objects. Set a non-zero value to enable draw instanced. And set up a customized
//    bounding box again as the system can't decide the actual bound according to only
//    four original points
    geom->addPrimitiveSet( new osg::DrawArrays(GL_QUADS, 0, 4, numInstances) );
    geom->setInitialBound( osg::BoundingBox(-1.0f,-32.0f,-1.0f, 192.0f, 32.0f, 192.0f) );

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage( osgDB::readImageFile("Images/osg256.png") );
    texture->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR );
    texture->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR );
    geom->getOrCreateStateSet()->setTextureAttributeAndModes( 0, texture.get() );
    geom->getOrCreateStateSet()->addUniform( new osg::Uniform("defaultTex", 0) );

    osg::ref_ptr<osg::Program> program = new osg::Program;
    program->addShader( new osg::Shader(osg::Shader::VERTEX, vertCode) );
    geom->getOrCreateStateSet()->setAttributeAndModes( program.get() );
    return geom.release();
}


//The draw instanced extension requires OpenGL 2.0 to work properly. It greatly reduces the
//memory usage of vertices and primitives on the CPU side, but can still perform as effectively
//as the traditional way to build geometries. It introduces a new read-only, built-in GLSL
//variable gl_InstanceID, which contains the current instance ID (from 0 to number of
//instances) in the rendering pipeline. With the texture as a parameter table, we can look for
//data corresponding to the ID and set gl_Position, glTexCoord[*], and other outputs to
//suitable values. This makes it possible to set up the positions and attributes of a low-polygon
//human, and even some scientific visualization work, for instance, the rendering of point-cloud
//data (we will discover this in Chapter 8).

//The gl_Vertex variable represents the same vertex data used by each instance, and so
//do the gl_Normal and gl_MultiTexCoord* variables. You must apply a transformation
//matrix or customized offset to them to move the instance to a different location in the
//3D world.

//In the shader code of this recipe, the pos variable represents the position of each instanced
//quad in the world coordinate (by adding an offset to the original gl_Vertex variable). Then,
//we multiply the model-view-projection (MVP) matrix with it to obtain the position in projection
//coordinate, which is actually required for final vertex composition in the rendering pipeline.
//Note that the shader code here is not perfect because it fixes the number of row and column
//instances instead of using uniforms. You may try to alter it to provide a more scalable draw
//instanced implementation.

int main( int argc, char** argv )
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( createInstancedGeometry(256*256) );

    osg::ref_ptr<osg::Group> root = new osg::Group;
    root->addChild( geode.get() );

    osgViewer::Viewer viewer;
    viewer.setSceneData( root.get() );
    return viewer.run();
}
