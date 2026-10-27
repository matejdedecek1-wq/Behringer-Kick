#ifndef _CONFIG_H_
#define _CONFIG_H_

#pragma once

// define your version_libinterface
#define MT_VERSION_MAJOR "0"
#define MT_VERSION_MINOR "3"
#define MT_VERSION_PATCH "3"

#define MT_VERSION_STR   "0.3.3"
#define MT_VERSION_NUM    033

#define MT_SOURCE_SRCDIR "."

struct ProjectInfo
{
    inline static const char* const projectName{ "MultiTouch" };
    inline static const char* const companyName{ "MusicTribe" };
    inline static const char* const versionString { "0.3.3" };
    inline static const int         versionNumber{ 033 };
};

#endif // _CONFIG_H_
