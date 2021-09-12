# GHString

A platform-independent string utility library for C++.

* Dependencies: None!
* See LICENSE.txt for license info.
* For further info email dev@goldenhammersoftware.com.

## GHString

GHString is a safety and convenience wrapper for char* and const char*.  They can be created using a CharHandlingType to specify how the memory is managed.  
* CHT_REFERENCE will do no memory copying or deletion.  
* CHT_CLAIM will do no memory copying but will make the GHString take responsibility for deletion.  
* CHT_COPY will make a copy of the input and be responsible for deletion.

## GHIdentifier

GHIdentifier is a hashed string that can be used as an efficient lookup key.  It is generated through the GHStringIdFactory interface.  GHStringIdFactoryDJB2 is provided as one implementation.  Define GHRETAIL to remove debug string representations from the memory.

## GHStringTokenizer

GHStringTokenizer is a utility for breaking up a string into tokens using arbitrary delineators.

## GHNumberListParser

GHNumberListParser is a utility for reading in a list of numbers from a string buffer.  For example turning "5 1 19 5" into an array of 4 ints.

## GHHashListParser

GHHashListParser is a utility for reading in a list of strings and turning them into GHIdentifiers.
