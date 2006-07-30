/*=========================================================================

  Program: GDCM (Grass Root DICOM). A DICOM library
  Module:  $URL$

  Copyright (c) 2006 Mathieu Malaterre
  Copyright (c) 1993-2005 CREATIS
  All rights reserved.
  See Copyright.txt or http://gdcm.sourceforge.net/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "gdcmImageCodec.h"
#include "gdcmTS.h"
#include "gdcmOStream.h"
#include "gdcmIStream.h"
#include "gdcmStringStream.h"
#include "gdcmByteSwap.txx"
#include "gdcmTrace.h"


namespace gdcm
{

class ImageInternals
{
public:
};

ImageCodec::ImageCodec()
{
  PlanarConfiguration = 2;
  RequestPlanarConfiguration = false;
  RequestPaddedCompositePixelCode = false;
  PI = PhotometricInterpretation::UNKNOW;
  //LUT = LookupTable(LookupTable::UNKNOWN);
  NeedByteSwap = false;
}

ImageCodec::~ImageCodec()
{
}

const PhotometricInterpretation &ImageCodec::GetPhotometricInterpretation() const
{
  return PI;
}

void ImageCodec::SetPhotometricInterpretation(
  PhotometricInterpretation const &pi)
{
  PI = pi;
}

bool ImageCodec::DoByteSwap(IStream &is, OStream &os)
{
  // FIXME: Do some stupid work:
  std::streampos start = is.Tellg();
  assert( 0 - start == 0 );
  is.Seekg( 0, std::ios::end);
  std::streampos buf_size = is.Tellg();
  char *dummy_buffer = new char[buf_size];
  is.Seekg(start, std::ios::beg);
  is.Read( dummy_buffer, buf_size);
  is.Seekg(start, std::ios::beg); // reset
  SwapCode sc = is.GetSwapCode();

  assert( !(buf_size % 2) );
  ByteSwap<uint16_t>::SwapRangeFromSwapCodeIntoSystem((uint16_t*)
    dummy_buffer, SwapCode::BigEndian, buf_size/2);
  os.Write(dummy_buffer, buf_size);
  return true;
}

bool ImageCodec::DoYBR(IStream &is, OStream &os)
{
  // FIXME: Do some stupid work:
  std::streampos start = is.Tellg();
  assert( 0 - start == 0 );
  is.Seekg( 0, std::ios::end);
  std::streampos buf_size = is.Tellg();
  char *dummy_buffer = new char[buf_size];
  is.Seekg(start, std::ios::beg);
  is.Read( dummy_buffer, buf_size);
  is.Seekg(start, std::ios::beg); // reset
  SwapCode sc = is.GetSwapCode();

    assert( !(buf_size % 3) );
    unsigned long size = buf_size/3;
    unsigned char *copy = new unsigned char[ buf_size ];
    memmove( copy, dummy_buffer, buf_size);

    const unsigned char *a = copy + 0;
    const unsigned char *b = copy + size;
    const unsigned char *c = copy + size + size;
    int R, G, B;

    unsigned char *p = (unsigned char*)dummy_buffer;
    for (unsigned long j = 0; j < size; ++j)
      {
      R = 38142 *(*a-16) + 52298 *(*c -128);
      G = 38142 *(*a-16) - 26640 *(*c -128) - 12845 *(*b -128);
      B = 38142 *(*a-16) + 66093 *(*b -128);

      R = (R+16384)>>15;
      G = (G+16384)>>15;
      B = (B+16384)>>15;

      if (R < 0)   R = 0;
      if (G < 0)   G = 0;
      if (B < 0)   B = 0;
      if (R > 255) R = 255;
      if (G > 255) G = 255;
      if (B > 255) B = 255;

      *(p++) = (unsigned char)R;
      *(p++) = (unsigned char)G;
      *(p++) = (unsigned char)B;
      a++;
      b++;
      c++;
      }
    delete[] copy;

  os.Write(dummy_buffer, buf_size);
  return true;
}

bool ImageCodec::DoPlanarConfiguration(IStream &is, OStream &os)
{
  // FIXME: Do some stupid work:
  std::streampos start = is.Tellg();
  assert( 0 - start == 0 );
  is.Seekg( 0, std::ios::end);
  std::streampos buf_size = is.Tellg();
  char *dummy_buffer = new char[buf_size];
  is.Seekg(start, std::ios::beg);
  is.Read( dummy_buffer, buf_size);
  is.Seekg(start, std::ios::beg); // reset
  SwapCode sc = is.GetSwapCode();

    // US-RGB-8-epicard.dcm
    //assert( image.GetNumberOfDimensions() == 3 );
    assert( !(buf_size % 3) );
    unsigned long size = buf_size/3;
    char *copy = new char[ buf_size ];
    memmove( copy, dummy_buffer, buf_size);

    const char *r = copy;
    const char *g = copy + size;
    const char *b = copy + size + size;

    char *p = dummy_buffer;
    for (unsigned long j = 0; j < size; ++j)
      {
      *(p++) = *(r++);
      *(p++) = *(g++);
      *(p++) = *(b++);
      }
    delete[] copy;

  os.Write(dummy_buffer, buf_size);
  return true;
}

bool ImageCodec::DoSimpleCopy(IStream &is, OStream &os)
{
  std::streampos start = is.Tellg();
  assert( 0 - start == 0 );
  is.Seekg( 0, std::ios::end);
  std::streampos buf_size = is.Tellg();
  char *dummy_buffer = new char[buf_size];
  is.Seekg(start, std::ios::beg);
  is.Read( dummy_buffer, buf_size);
  is.Seekg(start, std::ios::beg); // reset
  os.Write( dummy_buffer, buf_size);
  delete[] dummy_buffer ;

  return true;
}

bool ImageCodec::DoPaddedCompositePixelCode(IStream &is, OStream &os)
{
  // FIXME: Do some stupid work:
  std::streampos start = is.Tellg();
  assert( 0 - start == 0 );
  is.Seekg( 0, std::ios::end);
  std::streampos buf_size = is.Tellg();
  char *dummy_buffer = new char[buf_size];
  is.Seekg(start, std::ios::beg);
  is.Read( dummy_buffer, buf_size);
  is.Seekg(start, std::ios::beg); // reset
  SwapCode sc = is.GetSwapCode();

  assert( !(buf_size % 2) );
//   assert( !(check%2) );
//   std::string rle8 = os.Str();
   for(unsigned long i = 0; i < buf_size/2; ++i)
     {
     //buffer[2*i]= rle8[i+check/2];
     //buffer[2*i+1] = rle8[i];
     os.Write( dummy_buffer+i+buf_size/2, 1 );
     os.Write( dummy_buffer+i, 1 );
     }
  //os.Write(dummy_buffer, buf_size);
  return true;
}

bool ImageCodec::Decode(IStream &is, OStream &os)
{
  assert( PlanarConfiguration == 0 || PlanarConfiguration == 1);
  assert( PI != PhotometricInterpretation::UNKNOW );
  StringStream bs_os; // ByteSwap
  StringStream pcpc_os; // Padeed Composite Pixel Code
  StringStream pi_os; // PhotometricInterpretation
  IStream *cur_is = &is;

  // First thing do the byte swap:
  if( NeedByteSwap )
    {
    //MR_GE_with_Private_Compressed_Icon_0009_1110.dcm
    DoByteSwap(*cur_is,bs_os);
    cur_is = &bs_os;
    }
  if ( RequestPaddedCompositePixelCode )
    {
    // D_CLUNIE_CT2_RLE.dcm
    DoPaddedCompositePixelCode(*cur_is,pcpc_os);
    cur_is = &pcpc_os;
    }

  // Second thing do palette color.
  // This way PALETTE COLOR will be applied before we do
  // Planar Configuration
  if (PI == PhotometricInterpretation::MONOCHROME2
   || PI == PhotometricInterpretation::RGB )
    {
    }
  else if (PI == PhotometricInterpretation::MONOCHROME1)
    {
    // TODO
    abort();
    }
  else if ( PI == PhotometricInterpretation::YBR_FULL )
    {
    DoYBR(*cur_is,pi_os);
    cur_is = &pi_os;
    }
  else if ( PI == PhotometricInterpretation::PALETTE_COLOR )
    {
    LUT->Decode(*cur_is, pi_os);
    cur_is = &pi_os;
    }
  else
    {
    abort();
    }

  if( PlanarConfiguration || RequestPlanarConfiguration )
    {
    if ( PI == PhotometricInterpretation::YBR_FULL )
      {
      // ACUSON-24-YBR_FULL-RLE.dcm declare PlanarConfiguration=1
      // but it's only pure YBR...
      gdcmWarningMacro( "Not sure what to do" );
      DoSimpleCopy(*cur_is,os);
      }
    else
      {
      DoPlanarConfiguration(*cur_is,os);
      }
    }
  else
    {
    DoSimpleCopy(*cur_is,os);
    }

  return true;
}

} // end namespace gdcm
