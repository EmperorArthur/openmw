///Functions used to read raw binary data from .nif files

#ifndef OPENMW_COMPONENTS_NIF_NIFSTREAM_HPP
#define OPENMW_COMPONENTS_NIF_NIFSTREAM_HPP

#include <stdint.h>
#include <stdexcept>
#include <typeinfo>
#include <string>

#include <OgreDataStream.h>
#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreVector4.h>
#include <OgreMatrix3.h>
#include <OgreQuaternion.h>
#include <OgreStringConverter.h>

#include "niffile.hpp"
#include "niftypes.hpp"

namespace Nif
{

class NIFStream {

    /// Input stream
    Ogre::DataStreamPtr inp;

    uint8_t read_byte();
    uint16_t read_le16();
    uint32_t read_le32();
    float read_le32f();

public:

    NIFFile * const file;

    NIFStream (NIFFile * file, Ogre::DataStreamPtr inp): file (file), inp (inp) {}

    /*************************************************
               Parser functions
    ****************************************************/

    template <typename T>
    struct GetHandler
    {
        typedef T (NIFStream::*fn_t)();

        static const fn_t sValue; // this is specialized per supported type in the .cpp file

        static T read (NIFStream* nif)
        {
            return (nif->*sValue) ();
        }
    };

    template <typename T>
    void read (NIFStream* nif, T & Value)
    {
        Value = GetHandler <T>::read (nif);
    }

    void skip(size_t size) { inp->skip(size); }
    void read (void * data, size_t size) { inp->read (data, size); }

    char getChar() { return read_byte(); }
    short getShort() { return read_le16(); }
    unsigned short getUShort() { return read_le16(); }
    int getInt() { return read_le32(); }
    unsigned int getUInt() { return read_le32(); }
    float getFloat() { return read_le32f(); }

    Ogre::Vector2 getVector2();
    Ogre::Vector3 getVector3();
    Ogre::Vector4 getVector4();
    Ogre::Matrix3 getMatrix3();
    Ogre::Quaternion getQuaternion();
    Transformation getTrafo();

    ///Read in a string of the given length
    std::string getString(size_t length);
    ///Read in a string of the length specified in the file
    std::string getString();
    ///This is special since the version string doesn't start with a number, and ends with "\n"
    std::string getVersionString();
    ///This is a strange type used by newer nif formats
    std::string getShortString();

    //Templated functions to handle reads
    template <typename T>
    T get(){throw std::runtime_error("Can not read a <"+std::string(typeid(T).name())+"> from a NIF File!  The get() function was called with the wrong template!");}

    ///Return a vector of whatever object is needed
    template <typename T>
    std::vector<T> getItems(size_t number_of_items)
    {
        std::vector<T> items;
        items.reserve(number_of_items);
        for(size_t i=0; i < number_of_items; ++i)
        {
            items.push_back(get<T>());
        }
        return items;
    }

    template <typename T>
    T getIfVer(unsigned int testVersion)
    {
        if (file->getVersion() == testVersion)
            return get<T>();
        else
            return T();
    }

};

}

#endif
