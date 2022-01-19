#include <cmath>

#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/OS_Memory.h"

#include "common_macros.h"

template <typename ValueType>
Common_Math_FFT_T<ValueType>::Common_Math_FFT_T (unsigned int channels_in,
                                                 unsigned int slots_in,
                                                 unsigned int sampleRate_in)
 : isInitialized_ (false)
 , buffer_ (NULL)
 , X_ (NULL)
 , bitReverseMap_ (NULL)
 , channels_ (channels_in)
 , halfSlots_ (slots_in / 2)
 , slots_ (slots_in)
 , sampleRate_ (sampleRate_in)
 /////////////////////////////////////////
 , logSlots_ (0)
 , sqrtSlots_ (0.0)
 , W_ (NULL)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Common_Math_FFT_T"));

  isInitialized_ = Initialize (channels_in,
                               slots_in,
                               sampleRate_in);
}

template <typename ValueType>
Common_Math_FFT_T<ValueType>::~Common_Math_FFT_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::~Common_Math_FFT_T"));

  if (buffer_)
  {
    for (unsigned int i = 0; i < channels_; ++i)
      delete [] buffer_[i];
    delete [] buffer_;
  } // end IF
  if (bitReverseMap_)
    delete [] bitReverseMap_;
  if (W_)
  {
    for (unsigned int l = 1; l <= static_cast<unsigned int> (logSlots_); ++l)
      delete [] W_[l];
    delete [] W_;
  } // end IF
  if (X_)
  {
    for (unsigned int i = 0; i < channels_; ++i)
      delete [] X_[i];
    delete [] X_;
  } // end IF
}

template <typename ValueType>
bool
Common_Math_FFT_T<ValueType>::Initialize (unsigned int channels_in,
                                          unsigned int slots_in,
                                          unsigned int sampleRate_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Initialize"));

  // sanity check(s)
  if (unlikely (!channels_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid #channels, aborting\n")));
    return false;
  } // end IF
  if (unlikely (!slots_in ||
                (slots_in % 2)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid #slots (must be > 0 and a power of 2; was: %d), aborting\n"),
                slots_in));
    return false;
  } // end IF
  if (unlikely (!sampleRate_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid sample rate, aborting\n")));
    return false;
  } // end IF

  if (isInitialized_)
  {
    if (buffer_)
    {
      for (unsigned int i = 0; i < channels_; ++i)
        delete [] buffer_[i];
      delete [] buffer_; buffer_ = NULL;
    } // end IF
    channels_ = 0;
    halfSlots_ = 0;
    slots_ = 0;
    sampleRate_ = 0;

    if (bitReverseMap_)
    {
      delete [] bitReverseMap_; bitReverseMap_ = NULL;
    } // end IF
    if (W_)
    {
      for (int l = 1; l <= logSlots_; ++l)
        delete [] W_[l];
      delete [] W_; W_ = NULL;
    } // end IF
    logSlots_ = 0;
    sqrtSlots_ = 0.0;
    if (X_)
    {
      for (unsigned int i = 0; i < channels_; ++i)
        delete [] X_[i];
      delete [] X_; X_ = NULL;
    } // end IF

    isInitialized_ = false;
  } // end IF

  int rev = 0;
  int _2_l = 2;

  ACE_NEW_NORETURN (buffer_,
                    ValueType*[channels_in]);
  if (unlikely (!buffer_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (unsigned int i = 0; i < channels_in; ++i)
  {
    ACE_NEW_NORETURN (buffer_[i],
                      ValueType[slots_in]);
    if (unlikely (!buffer_[i]))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      goto error;
    } // end IF
  } // end FOR
#if 0
  // 1 kHz calibration wave
  for (unsigned int i = 0; i < channels_in; ++i)
    for (unsigned int j = 0; j < slots_in; ++j)
        buffer_[i][j] = 1600 * sin (2.0 * M_PI * 1000.0 * j / sampleRate_in);
#else
  ACE_OS::memset (buffer_, 0, channels_in * slots_in * sizeof (ValueType));
#endif // 0
  channels_ = channels_in;
  halfSlots_ = slots_in / 2;
  slots_ = slots_in;
  sampleRate_ = sampleRate_in;

  // set up bit-reverse mapping
  ACE_NEW_NORETURN (bitReverseMap_,
                    int[slots_]);
  if (unlikely (!bitReverseMap_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (unsigned int i = 0; i < slots_ - 1; ++i)
  {
    bitReverseMap_[i] = rev;
    int mask = halfSlots_;
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

  // precompute complex exponentials
  //  ACE_NEW_NORETURN (W_,
  //                    std::complex<ValueType>*[logSlots_ + 1]);
  W_ =
      reinterpret_cast<std::complex<ValueType>**> (ACE_OS::malloc (sizeof (std::complex<ValueType>*) * (logSlots_ + 1)));
  if (unlikely (!W_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (int l = 1; l <= logSlots_; ++l)
  {
    ACE_NEW_NORETURN (W_[l],
                      std::complex<ValueType>[slots_]);
    if (unlikely (!W_[l]))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      goto error;
    } // end IF
    for (unsigned int i = 0; i < slots_; ++i)
      W_[l][i] = std::complex<ValueType> ( cos (2.0 * M_PI * i / _2_l),
                                          -sin (2.0 * M_PI * i / _2_l));
    _2_l *= 2;
  } // end FOR

  ACE_NEW_NORETURN (X_,
                    std::complex<ValueType>*[channels_]);
  if (unlikely (!X_))
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    goto error;
  } // end IF
  for (unsigned int i = 0; i < channels_; ++i)
  {
    ACE_NEW_NORETURN (X_[i],
                      std::complex<ValueType>[slots_]);
    if (unlikely (!X_[i]))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      goto error;
    } // end IF
  } // end FOR

  isInitialized_ = true;

  return true;

error:
  if (buffer_)
  {
    for (unsigned int i = 0; i < channels_in; ++i)
      delete [] buffer_[i];
    delete [] buffer_; buffer_ = NULL;
  } // end IF
  if (bitReverseMap_)
  {
    delete [] bitReverseMap_; bitReverseMap_ = NULL;
  } // end IF
  if (W_)
  {
    for (int l = 1; l <= logSlots_; ++l)
      delete [] W_[l];
    delete [] W_; W_ = NULL;
  } // end IF
  if (X_)
  {
    for (unsigned int i = 0; i < channels_in; ++i)
      delete [] X_[i];
    delete [] X_; X_ = NULL;
  } // end IF

  return false;
}

//void
//Common_Math_FFT_T::CopyIn (unsigned int channel_in,
//                         ITERATOR_T* iterator_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::CopyIn"));
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
//                   (slots_ - number_of_samples) * sizeof (ValueType));
//  // copy the sample data to the tail end of the buffer
//  int tail_index  = slots_ - number_of_samples;
//  for (int i = 0; i < number_of_samples; ++i, iterator_in.Advance ())
//    buffer_[channel_in][tail_index + i] = static_cast<ValueType> (iterator_in.GetSample ());
//
//  // initialize the FFT working set buffer
//  for (int i = 0; i < slots_; ++i)
//    X_[channel_in][bitReverseMap_[i]] = std::complex<ValueType> (buffer_[channel_in][i]);
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

template <typename ValueType>
void
Common_Math_FFT_T<ValueType>::Compute (unsigned int channel_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_FFT_T::Compute"));

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
        std::complex<ValueType> T = W_[level][j];
        T *= X_[channel_in][i + step];
        X_[channel_in][i + step]  = X_[channel_in][i];
        X_[channel_in][i + step] -= T;
        X_[channel_in][i]        += T;
      } // end FOR
    } // end FOR
    step *= 2;
  } // end FOR
}
