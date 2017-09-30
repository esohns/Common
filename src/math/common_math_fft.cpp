#include "stdafx.h"

#include "common_math_fft.h"

#include <cmath>

#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/OS_Memory.h"

#include "common_macros.h"

Common_Math_FFT_SampleIterator::Common_Math_FFT_SampleIterator (char* buffer_in)
 : buffer_ (buffer_in)
 , dataSampleSize_ (0)
 , reverseEndianness_ (false)
 , soundSampleSize_ (0)
 /////////////////////////////////////////
 , isInitialized_ (false)
 , isSignedSampleFormat_ (true)
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
      return (reverseEndianness_ ? (isSignedSampleFormat_ ? ACE_SWAP_WORD (*reinterpret_cast<short*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))
                                                          : ACE_SWAP_WORD (*reinterpret_cast<unsigned short*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])))
                                 : (isSignedSampleFormat_ ? *reinterpret_cast<short*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                                          : *reinterpret_cast<unsigned short*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])));
    case 4:
    { // --> non-integer type data is (most probably) a 32-bit IEEE float
      // *TODO*: this isn't very portable
      ACE_ASSERT (ACE_SIZEOF_FLOAT == 4);
      return (sampleByteOrder_ ? (reverseEndianness_ ? (isSignedSampleFormat_ ? ACE_SWAP_LONG (*reinterpret_cast<int*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]))
                                                                              : ACE_SWAP_LONG (*reinterpret_cast<unsigned int*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])))
                                                     : (isSignedSampleFormat_ ? *reinterpret_cast<int*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])
                                                                              : *reinterpret_cast<unsigned int*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)])))
                               : *reinterpret_cast<float*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]));
    }
    case 8:
    {
      // --> non-integer type data is (most probably) a 64-bit IEEE float (aka double)
      // *TODO*: this isn't very portable
      ACE_ASSERT (ACE_SIZEOF_DOUBLE == 8);
      return *reinterpret_cast<double*> (&buffer_[(index_in * dataSampleSize_) + (channel_in * soundSampleSize_)]);
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
                                            int sampleByteOrder_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_SampleIterator::initialize"));

  dataSampleSize_ = dataSampleSize_in;
  soundSampleSize_ = soundSampleSize_in;
  isSignedSampleFormat_ = isSignedSampleFormat_;
  sampleByteOrder_ = sampleByteOrder_in;

  reverseEndianness_ =
    sampleByteOrder_ && (ACE_BYTE_ORDER != sampleByteOrder_);

  isInitialized_ = true;

  return true;
}

//////////////////////////////////////////

Common_Math_FFT::Common_Math_FFT (unsigned int channels_in,
                                  unsigned int slots_in,
                                  unsigned int sampleRate_in)
 : buffer_ (NULL)
 , X_ (NULL)
 , bitReverseMap_ (NULL)
 , channels_ (channels_in)
 , slots_ (slots_in)
 , sampleRate_ (sampleRate_in)
 /////////////////////////////////////////
 , logSlots_ (0)
 , sqrtSlots_ (0.0)
 , W_ (NULL)
 , isInitialized_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT::Common_Math_FFT"));

  isInitialized_ = Initialize (channels_in,
                               slots_in,
                               sampleRate_in);
}

Common_Math_FFT::~Common_Math_FFT ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT::~Common_Math_FFT"));

  if (buffer_)
    for (unsigned int i = 0; i < channels_; ++i)
      delete[] buffer_[i];
  delete [] buffer_;
  delete [] bitReverseMap_;
  if (W_)
    for (unsigned int l = 1; l <= static_cast<unsigned int> (logSlots_); ++l)
      delete [] W_[l];
  delete [] W_;
  if (X_)
    for (unsigned int i = 0; i < channels_; ++i)
      delete[] X_[i];
  delete [] X_;
}

bool
Common_Math_FFT::Initialize (unsigned int channels_in,
                             unsigned int slots_in,
                             unsigned int sampleRate_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT::Initialize"));

  // sanity check(s)
  ACE_ASSERT (channels_in > 0);
  if (slots_in % 2)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("buffer size must be a power of 2 (was: %d), aborting\n"),
                slots_in));
    return false;
  } // end IF

  if (isInitialized_)
  {
    if (buffer_)
    {
      for (unsigned int i = 0; i < channels_; ++i)
        delete [] buffer_[i];
      delete [] buffer_;
      buffer_ = NULL;
    } // end IF
    channels_ = 0;
    slots_ = 0;
    sampleRate_ = 0;

    if (bitReverseMap_)
    {
      delete [] bitReverseMap_;
      bitReverseMap_ = NULL;
    } // end IF
    if (W_)
    {
      for (int l = 1; l <= logSlots_; ++l)
        delete [] W_[l];
      delete [] W_;
      W_ = NULL;
    } // end IF
    logSlots_ = 0;
    sqrtSlots_ = 0.0;
    if (X_)
    {
      for (unsigned int i = 0; i < channels_; ++i)
        delete [] X_[i];
      delete [] X_;
      X_ = NULL;
    } // end IF

    isInitialized_ = false;
  } // end IF

  ACE_NEW_NORETURN (buffer_,
                    double* [channels_in]);
  if (!buffer_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF
  for (unsigned int i = 0; i < channels_in; ++i)
  {
    ACE_NEW_NORETURN (buffer_[i],
                      double [slots_in]);
    if (!buffer_[i])
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      return false;
    } // end IF
  } // end FOR
#if 0
  // 1 kHz calibration wave
  for (unsigned int i = 0; i < channels_in; ++i)
    for (unsigned int j = 0; j < slots_in; ++j)
        buffer_[i][j] = 1600 * sin (2.0 * M_PI * 1000.0 * j / sampleRate_in);
#else
  for (unsigned int i = 0; i < channels_in; ++i)
    for (unsigned int j = 0; j < slots_in; ++j)
      buffer_[i][j] = 0.0;
#endif
  channels_ = channels_in;
  slots_ = slots_in;
  sampleRate_ = sampleRate_in;

  ACE_NEW_NORETURN (bitReverseMap_,
                    int [slots_]);
  if (!bitReverseMap_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF

  // set up bit-reverse mapping
  int rev = 0;
  int half_slots = slots_ / 2;
  for (unsigned int i = 0; i < slots_ - 1; ++i)
  {
    bitReverseMap_[i] = rev;
    int mask = half_slots;
    // add 1 backwards
    while (rev >= mask)
    {
      rev -= mask; // turn off this bit
      mask >>= 1;
    } // end WHILE
    rev += mask;
  } // end FOR
  bitReverseMap_[slots_ - 1] = slots_ - 1;

  // calculate binary log
  slots_in--;
  while (slots_in)
  {
    slots_in >>= 1;
    logSlots_++;
  } // end WHILE
  sqrtSlots_ = sqrt (static_cast<double> (slots_));

  ACE_NEW_NORETURN (W_,
                    std::complex<double>* [logSlots_ + 1]);
  if (!W_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF
  // precompute complex exponentials
  int _2_l = 2;
  for (int l = 1; l <= logSlots_; ++l)
  {
    ACE_NEW_NORETURN (W_[l],
                      std::complex<double> [slots_]);
    if (!W_[l])
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      return false;
    } // end IF

    for (unsigned int i = 0; i < slots_; ++i)
      W_[l][i] = std::complex<double> ( cos (2.0 * M_PI * i / _2_l),
                                       -sin (2.0 * M_PI * i / _2_l));

    _2_l *= 2;
  } // end FOR

  ACE_NEW_NORETURN (X_,
                    std::complex<double>* [channels_]);
  if (!X_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF
  for (unsigned int i = 0; i < channels_; ++i)
  {
    ACE_NEW_NORETURN (X_[i],
                      std::complex<double> [slots_]);
    if (!X_[i])
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      return false;
    } // end IF
  } // end FOR

  isInitialized_ = true;

  return true;
}

//void
//Common_Math_FFT::CopyIn (unsigned int channel_in,
//                         ITERATOR_T* iterator_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT::CopyIn"));
//
//  // sanity check(s)
//  ACE_ASSERT (channel_in > 0);
//
//  int number_of_samples = iterator_in.Count ();
//  // sanity check(s)
//  if (number_of_samples > slots_)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("buffer too small (expected: %d, is: %d), returning\n"),
//                number_of_samples, slots_));
//    return;
//  } // end IF
//
//  // make space for inbound samples at the end of the buffer,
//  // shifting previous samples towards the beginning
//  ACE_OS::memmove (buffer_[channel_in], &buffer_[channel_in][number_of_samples],
//                   (slots_ - number_of_samples) * sizeof (double));
//  // copy the sample data to the tail end of the buffer
//  int tail_index  = slots_ - number_of_samples;
//  for (int i = 0; i < number_of_samples; ++i, iterator_in.Advance ())
//    buffer_[channel_in][tail_index + i] = static_cast<double> (iterator_in.GetSample ());
//
//  // initialize the FFT working set buffer
//  for (int i = 0; i < slots_; ++i)
//    X_[channel_in][bitReverseMap_[i]] = std::complex<double> (buffer_[channel_in][i]);
//}

//
//               0   1   2   3   4   5   6   7
//  level   1
//  step    1                                     0
//  increm  2                                   W 
//  j = 0        <--->   <--->   <--->   <--->   1
//  level   2
//  step    2
//  increm  4                                     0
//  j = 0        <------->       <------->      W      1
//  j = 1            <------->       <------->   2   W
//  level   3                                         2
//  step    4
//  increm  8                                     0
//  j = 0        <--------------->              W      1
//  j = 1            <--------------->           3   W      2
//  j = 2                <--------------->            3   W      3
//  j = 3                    <--------------->             3   W
//                                                              3
//

void
Common_Math_FFT::Compute (unsigned int channel_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT::Compute"));

  // step = 2 ^ (level - 1)
  // increment = 2 ^ level;
  unsigned int step = 1;
  for (unsigned int level = 1;
       level <= static_cast<unsigned int> (logSlots_);
       ++level)
  {
    unsigned int increment = step * 2;
    for (unsigned int j = 0; j < step; ++j)
    {
      // U = exp ( - 2 PI j / 2 ^ level )
      //std::complex<double> U = W_[level][j];
      for (unsigned int i = j; i < slots_; i += increment)
      {
        // butterfly
        //std::complex<double> T = U;
        std::complex<double> T = W_[level][j];
        T *= X_[channel_in][i + step];
        X_[channel_in][i + step]  = X_[channel_in][i];
        X_[channel_in][i + step] -= T;
        X_[channel_in][i]        += T;
      } // end FOR
    } // end FOR
    step *= 2;
  } // end FOR
}
