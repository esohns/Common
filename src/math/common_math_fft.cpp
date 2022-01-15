#include "stdafx.h"

#include "common_math_fft.h"

Common_Math_FFT_SampleIterator::Common_Math_FFT_SampleIterator (uint8_t* buffer_in)
 : isInitialized_ (false)
 , buffer_ (buffer_in)
 , dataSampleSize_ (0)
 , isSignedSampleFormat_ (true)
 , reverseEndianness_ (false)
 , soundSampleSize_ (0)
 /////////////////////////////////////////
 , isFloatingPointFormat_ (false)
 , sampleByteOrder_ (ACE_BYTE_ORDER)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_SampleIterator_T::Common_Math_FFT_SampleIterator_T"));

}

double
Common_Math_FFT_SampleIterator::get (unsigned int index_in,
                                     unsigned int channel_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_SampleIterator::get"));

  // sanity check(s)
  ACE_ASSERT (buffer_);
  ACE_ASSERT (isInitialized_);

  switch (soundSampleSize_)
  {
    case 1: // --> data is single-byte (possibly non-integer)
      return buffer_[(index_in * dataSampleSize_) + channel_in];
    case 2:
      return (reverseEndianness_ ? (isSignedSampleFormat_ ? ACE_SWAP_WORD (*reinterpret_cast<int16_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))
                                                          : ACE_SWAP_WORD (*reinterpret_cast<uint16_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])))
                                 : (isSignedSampleFormat_ ? *reinterpret_cast<int16_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                                          : *reinterpret_cast<uint16_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])));
    case 4:
    { ACE_ASSERT (ACE_SIZEOF_FLOAT == 4);
      return (isFloatingPointFormat_ ? *reinterpret_cast<float*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                     : (reverseEndianness_ ? (isSignedSampleFormat_ ? ACE_SWAP_LONG (*reinterpret_cast<int32_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))
                                                                                    : ACE_SWAP_LONG (*reinterpret_cast<uint32_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])))
                                                           : (isSignedSampleFormat_ ? *reinterpret_cast<int32_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                                                                    : *reinterpret_cast<uint32_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))));
    }
    case 8:
    { ACE_ASSERT (ACE_SIZEOF_DOUBLE == 8);
      return (isFloatingPointFormat_ ? *reinterpret_cast<double*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                     : (reverseEndianness_ ? (isSignedSampleFormat_ ? ACE_SWAP_LONG_LONG (*reinterpret_cast<int64_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))
                                                                                    : ACE_SWAP_LONG_LONG (*reinterpret_cast<uint64_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])))
                                                           : (isSignedSampleFormat_ ? *reinterpret_cast<int64_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                                                                    : *reinterpret_cast<uint64_t*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))));
    }
    case 16:
    { ACE_ASSERT (ACE_SIZEOF_LONG_DOUBLE == 16);
      ACE_ASSERT (isFloatingPointFormat_);
      return *reinterpret_cast<long double*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]);
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("unknown/invalid sound sample size (was: %d), aborting\n"),
                  soundSampleSize_));
      break;
    }
  } // end SWITCH

  return 0.0;
}

bool
Common_Math_FFT_SampleIterator::initialize (unsigned int dataSampleSize_in,
                                            unsigned int soundSampleSize_in,
                                            bool isSignedSampleFormat_in,
                                            bool isFloatingPointFormat_in,
                                            int sampleByteOrder_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_SampleIterator::initialize"));

  dataSampleSize_ = dataSampleSize_in;
  soundSampleSize_ = soundSampleSize_in;
  isSignedSampleFormat_ = isSignedSampleFormat_in;
  isFloatingPointFormat_ = isFloatingPointFormat_in;
  sampleByteOrder_ = sampleByteOrder_in;

  reverseEndianness_ =
    sampleByteOrder_ && (ACE_BYTE_ORDER != sampleByteOrder_);

  isInitialized_ = true;

  return true;
}

//////////////////////////////////////////
