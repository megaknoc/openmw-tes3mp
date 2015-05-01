#ifndef OPENMW_BULLET_SHAPE_LOADER_H_
#define OPENMW_BULLET_SHAPE_LOADER_H_

#include <OgreResource.h>
#include <OgreResourceManager.h>
#include <btBulletCollisionCommon.h>
#include <OgreVector3.h>

#include <osg/Vec3f>
#include <osg/Quat>

namespace OEngine {
namespace Physic
{

/**
*Define a new resource which describe a Shape usable by bullet.See BulletShapeManager for how to get/use them.
*/
class BulletShape : public Ogre::Resource
{
protected:
    void loadImpl();
    void unloadImpl();
    size_t calculateSize() const;

    void deleteShape(btCollisionShape* shape);

public:

    BulletShape(Ogre::ResourceManager *creator, const Ogre::String &name,
        Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false,
        Ogre::ManualResourceLoader *loader = 0);

    virtual ~BulletShape();

    // Stores animated collision shapes. If any collision nodes in the NIF are animated, then mCollisionShape
    // will be a btCompoundShape (which consists of one or more child shapes).
    // In this map, for each animated collision shape,
    // we store the node's record index mapped to the child index of the shape in the btCompoundShape.
    std::map<int, int> mAnimatedShapes;

    btCollisionShape* mCollisionShape;

    // Does this .nif have an autogenerated collision mesh?
    bool mAutogenerated;

    osg::Vec3f mBoxTranslation;
    osg::Quat mBoxRotation;
};

/**
*
*/

typedef Ogre::SharedPtr<BulletShape> BulletShapePtr;

/**
*Hold any BulletShape that was created by the ManualBulletShapeLoader.
*
*To get a bulletShape, you must load it first.
*First, create a manualBulletShapeLoader. Then call ManualBulletShapeManager->load(). This create an "empty" resource.
*Then use BulletShapeManager->load(). This will fill the resource with the required info.
*To get the resource,use BulletShapeManager::getByName.
*When you use the resource no more, just use BulletShapeManager->unload(). It won't completly delete the resource, but it will
*"empty" it.This allow a better management of memory: when you are leaving a cell, just unload every useless shape.
*
*Alternatively, you can call BulletShape->load() in order to actually load the resource.
*When you are finished with it, just call BulletShape->unload().
*
*IMO: prefere the first methode, i am not completly sure about the 2nd.
*
*Important Note: i have no idea of what happen if you try to load two time the same resource without unloading.
*It won't crash, but it might lead to memory leaks(I don't know how Ogre handle this). So don't do it!
*/
class BulletShapeManager : public Ogre::ResourceManager
{
protected:

    // must implement this from ResourceManager's interface
    Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
        const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
        const Ogre::NameValuePairList *createParams);

    static BulletShapeManager *sThis;

private:
    /** \brief Explicit private copy constructor. This is a forbidden operation.*/
    BulletShapeManager(const BulletShapeManager &);

    /** \brief Private operator= . This is a forbidden operation. */
    BulletShapeManager& operator=(const BulletShapeManager &);

    // Not intended to be used, declared here to keep the compiler from complaining
    // about hidden virtual methods.
    virtual Ogre::ResourcePtr load(const Ogre::String &name, const Ogre::String &group,
            bool isManual, Ogre::ManualResourceLoader *loader, const Ogre::NameValuePairList *loadParams,
            bool backgroundThread);

public:

    BulletShapeManager();
    virtual ~BulletShapeManager();


    /// Get a resource by name
    /// @see ResourceManager::getByName
    BulletShapePtr getByName(const Ogre::String& name,
            const Ogre::String& groupName = Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);

    /// Create a new shape
    /// @see ResourceManager::createResource
    BulletShapePtr create (const Ogre::String& name, const Ogre::String& group,
                        bool isManual = false, Ogre::ManualResourceLoader* loader = 0,
                        const Ogre::NameValuePairList* createParams = 0);

    virtual BulletShapePtr load(const Ogre::String &name, const Ogre::String &group);

    static BulletShapeManager &getSingleton();
    static BulletShapeManager *getSingletonPtr();
};

class BulletShapeLoader : public Ogre::ManualResourceLoader
{
public:

    BulletShapeLoader(){};
    virtual ~BulletShapeLoader() {}

    virtual void loadResource(Ogre::Resource *resource);

    virtual void load(const std::string &name,const std::string &group);
};

}
}

#endif
