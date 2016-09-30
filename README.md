# Cloud Burst File System 

Cloud Burst File System is a file system implementation with Cloud Burst Buffer feature.

## Required Software
- [CMake](http://www.cmake.org/) >= 2.8.12
- [MessagePack-RPC for C++](http://download.jubat.us/files/source/jubatus_msgpack-rpc/) >= 0.4.4
- [jubatus-mpio](http://download.jubat.us/files/source/jubatus_mpio/) >= 0.4.0
- [FUSE](http://fuse.sourceforge.net/) >= 2.8.3

## Build and Install
1. $ cd .; mkdir build; cd build
2. $ export JUBATUS\_MSGPACK\_RPC\_DIR=/path/to/{jubatus\_msgpack-rpc}; export JUBATUS\_MPIO\_DIR=/path/to/{jubatus\_mpio}
2. $ cmake -DCMAKE\_INSTALL\_PREFIX=/path/to/directory ..
3. $ make; make install (root privilege may be required)

## License
Cloud Burst File System is released under [Apache License Version 2.0](http://www.apache.org/licenses/LICENSE-2.0).
