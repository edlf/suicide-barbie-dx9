#ifndef MUTANT_READER_H_
#define MUTANT_READER_H_

#include "cfg.h"

#include "binary_io.h"
#include "data.h"
#include "types.h"

#include "Modules/Mutalisk/scene.h"
#include "Modules/Mutalisk/mesh.h"
#include "Modules/Mutalisk/dx9Mesh.h"

namespace mutant
{
  struct binary_input_utils
  {
  public:
    binary_input_utils( std::auto_ptr<binary_input> input )
    :  mInput( input )
    {
    }

    ~binary_input_utils()
    {
    };

    std::string readString();
    std::string& readString( std::string& str );

    uint32_t readDword()
    {
      uint32_t i = 0;
      mInput->read( &i, sizeof(i), 0 );
      return i;
    }

    uint16_t readWord()
    {
      uint16_t i = 0;
      mInput->read( &i, sizeof(i), 0 );
      return i;
    }

    uint8_t readByte()
    {
      uint8_t c = 0;
      mInput->read( &c, sizeof(c), 0 );
      return c;
    }

    bool readBool()
    {  return (readByte() != 0x00); }

    template<typename T>
    void readType( T& d )
    {
      mInput->read( &d, sizeof(d), 0 );
    }

/*_    template<>
    void readType<std::string>( std::string& d )*/
    void readType( std::string& d )
    {
      readString( d );
    }

    std::auto_ptr<binary_input> release_input() {
      return std::auto_ptr<binary_input>( mInput.release() );
    }

    void readOpaqueData( void* ptr, size_t size)
    {
      mInput->read( ptr, size, 0 );
    }

  private:
    std::auto_ptr<binary_input>    mInput;
  };

  //
  struct mutant_reader
    :  public binary_input_utils
  {
  public:

    mutant_reader( std::auto_ptr<binary_input> input );
    virtual ~mutant_reader();

    virtual void read( anim_character_set& char_set );
    virtual void read( simple_skinned& data ) {
      (void) data;
    }

    // virtual void read( simple_scene& scene );
    virtual void read( mutalisk::data::base_mesh& mesh );
    virtual void read( mutalisk::data::dx9_mesh& mesh );
    virtual void read( mutalisk::data::skin_info& skin );

    std::string readCharacter( anim_character& anim_char );
    std::string readClip( anim_clip& clip );
    std::string readBundle( anim_bundle& bundle, anim_clip& clip );

    void readAnimation( anim_bundle& bundle, anim_clip& clip );
    void readHierarchy( anim_hierarchy& hier );

    template<typename _T>
    void readVector(_T& v, int count)
    {
      typename _T::value_type data;
      v.resize(v.size() + count);
      while( count-- )
      {
        readType( data );
        v.push_back( data );
      }
    }
    template<typename _T>
    void readArray(_T* v, int count)
    {
      for(int q = 0; q < count; ++q)
        readType(v[q]);
    }
    template<typename _I>
    void readArray(_I from, _I end)
    {
      for(; from != end; ++from)
        readType(*from);
    }
    template<typename _T>
    void readData(_T* v, int count)
    {
      readOpaqueData(&v[0], sizeof(_T) * count);
    }
    template<typename _I>
    void readData(_I from, _I end)
    {
      readOpaqueData(&(*from), sizeof(typename _I::value_type) * std::distance(from, end));
    }

/*    template<typename _I>
    void readArray( _I from, _I end )
    {
      readData(&(*from), sizeof(_I::value_type) * std::distance(from, end));
    }

    template<typename _T>
    void readVector( std::vector<_T>& v, int count )
    {
      v.resize(v.size() + count);
      readData( &v[0], sizeof(_T) * count);
    }
    template<typename _T>
    void readArray(typename std::vector<_T>::iterator from, typename std::vector<_T>::iterator end )
    {
      readData(&(*from), sizeof(_T) * std::distance(from, end));
    }*/

    void enableLog( bool enabled = true ) { mutlog.setEnabled( enabled ); }

  private:
    mutlog_t  mutlog;
  };
}

#endif
