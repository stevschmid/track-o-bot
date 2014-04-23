#include <string>
#include <map>
#include <iostream>
using namespace std;

#include <QPixmap>

#define DEFINE_SINGLETON(CLASSNAME) \
public: \
  static CLASSNAME* Instance( void ) { \
    static Guard g; \
    if( !_singletonptr ) \
      _singletonptr = new CLASSNAME(); \
    return _singletonptr; \
  } \
private: \
  static CLASSNAME* _singletonptr; \
  CLASSNAME( void ); \
  CLASSNAME( const CLASSNAME& ); \
  ~CLASSNAME( void ); \
  class Guard \
  { \
  public: \
    ~Guard() { \
      if( CLASSNAME::_singletonptr != NULL ) { \
        delete CLASSNAME::_singletonptr; \
        CLASSNAME::_singletonptr = NULL; \
      } \
    } \
  }; \
  friend class Guard;

#define DEFINE_SINGLETON_SCOPE( CLASSNAME ) \
  CLASSNAME* CLASSNAME::_singletonptr = NULL;

