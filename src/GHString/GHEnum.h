// Copyright Golden Hammer Software

#include "GHString/GHIdentifierMap.h"
#include "GHString/GHStringIdFactory.h"

#undef GHENUMBEGIN
#undef GHENUMEND
#undef GHENUM
#undef GHENUMVAL
#undef GHIDENTIFIERBEGIN
#undef GHIDENTIFIEREND
#undef GHIDENTIFIER

#ifdef GHPUTSTRINGSINTABLES
    #define GHENUMBEGIN(x) namespace x { void generateEnum(GHIdentifierMap<int>& stringMap) {
    #define GHENUMEND } }
    #define GHENUM(x) stringMap.add(#x, x);
    #define GHENUMVAL(x, val) GHENUM(x)
#else
#ifdef GHPUTDECLARATIONS
    #define GHENUMBEGIN(x)
    #define GHENUMEND 
    #define GHENUM(x)
    #define GHENUMVAL(x, val)
#else
    #define GHENUMBEGIN(x) namespace x { void generateEnum(GHIdentifierMap<int>& stringMap); enum Enum {
    #define GHENUMEND }; }
    #define GHENUM(x) x,
    #define GHENUMVAL(x, val) x = val,
#endif
#endif

#ifdef GHPUTSTRINGSINTABLES
    #define GHIDENTIFIERBEGIN(x) namespace x { void generateIdentifiers(const GHStringIdFactory& hashTable) {
    #define GHIDENTIFIEREND } }
    #define GHIDENTIFIER(x) x = hashTable.generateHash(#x);
#else
#ifdef GHPUTDECLARATIONS
    #define GHIDENTIFIERBEGIN(x) namespace x { 
    #define GHIDENTIFIEREND }
    #define GHIDENTIFIER(x) GHIdentifier x;
#else
    #define GHIDENTIFIERBEGIN(x) namespace x { void generateIdentifiers(const GHStringIdFactory& hashTable);
    #define GHIDENTIFIEREND }
    #define GHIDENTIFIER(x) extern GHIdentifier x;
#endif
#endif

// MyEnum.h
//
//  #if defined(GHPUTSTRINGSINTABLES) || !defined(_MYENUM_H_)
//  #ifndef _MYENUM_H_
//      #define _MYENUM_H_
//  #endif
//
//    #include "GHString/GHEnum.h"
//    GHENUMBEGIN(MyEnum)
//      GHENUM(val1)
//      GHENUMVAL(val2, 5)
//      GHENUM(val3)
//    GHENUMEND
//    #include "GHString/GHEnumEnd.h"
//
//  #endif

// MyEnumUser.cpp
//
//  #include "MyEnum.h"
//
//     //use enums as normal

// String-To-Enum map loader.cpp
//
//  #include "MyEnum.h"
//  #include "MyEnum2.h"
//  ...
//      //the names of the variables in this function definition must be these
//  void loadIntoMap(GHIdentifierMap<int> stringMap) 
//  {
//      #define GHPUTSTRINGSINTABLES
//      #include "MyEnumVals.h"
//      #include "MyEnum2Vals.h"
//      ...
//      #undef GPUTSTRINGSINTABLES
//  }
//  

