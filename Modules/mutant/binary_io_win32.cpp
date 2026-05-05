#include "binary_io_win32.h"
#include "types_ios.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace mutant
{
  file_input::file_input( std::string const& name )
  :  mFile( NULL )
  {
    std::wstring wc( name.size(), L'#' );
    mbstowcs( &wc[0], name.c_str(), name.size() );

    mFile = CreateFile(
      wc.c_str(),
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,
      NULL );

    if( mFile == INVALID_HANDLE_VALUE )
      throw EIoError( IO_NOFILE, "Failed to open `" + name + "' for reading" );
  }

/*  file_input::file_input( std::wstring const& name )
  :  mFile( NULL )
  {
    mFile = CreateFileW(
      name.c_str(),
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,
      NULL );

    if( mFile == INVALID_HANDLE_VALUE )
      throw EIoError( IO_NOFILE, "Failed to open `???' for reading" );
  }
*/
  file_input::~file_input() {
    if( mFile ) {
      CloseHandle( (HANDLE)mFile );
    }
  }

  void file_input::read( void* dest, size_t n, int* wasRead )
  {
    DWORD rd = 0;

    bool good = ReadFile( (HANDLE)mFile, dest, (DWORD) n, &rd, NULL ) != 0;

    if( wasRead ) {
      *wasRead = rd;
    }

    (void) GetLastError();

    if( good && rd == 0 ) {
      throw EIoEof();
    }

    if( !good ) {
      throw EIoError( IO_ERROR, std::string("Failed to read `") + n + "' bytes from file" );
    }
  }

  file_output::file_output( std::string const& name ) {
    std::wstring wc( name.size(), L'#' );
    mbstowcs( &wc[0], name.c_str(), name.size() );

    mFile = CreateFile(
      wc.c_str(),
      GENERIC_WRITE,
      FILE_SHARE_READ,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      NULL );

    if( mFile == INVALID_HANDLE_VALUE ) {
      throw EIoError( IO_NOFILE, "Failed to open `" + name + "' for writing" );
    }
  }

  file_output::~file_output() {
    if( mFile ) {
      CloseHandle( (HANDLE)mFile );
    }
  }

  void file_output::write( void const* src, size_t n, int* wasWritten ) {
    DWORD wr = 0;

    bool good = WriteFile( (HANDLE)mFile, src, (DWORD) n, &wr, NULL ) != 0;

    if( wasWritten  ) {
      *wasWritten = wr;
    }

    (void) GetLastError();
    if( !good ) {
      throw EIoError( IO_ERROR, std::string("Failed to write `") + n + "' bytes to file" );
    }
  }

}
