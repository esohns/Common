
#include "ace/Log_Msg.h"

#include "common_macros.h"

template <typename ValueType>
Common_Math_SampleIterator_T<ValueType>::Common_Math_SampleIterator_T (uint8_t* buffer_in)
 : buffer_ (buffer_in)
 , reverseByteOrder_ (false)
 , sampleSize_ (0)
 , subSampleSize_ (0)
 /////////////////////////////////////////
 , isInitialized_ (false)
 , isSignedSampleFormat_ (true)
 , subSampleByteOrder_ (ACE_BYTE_ORDER)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_SampleIterator_T_T::Common_Math_SampleIterator_T_T"));

}

template <typename ValueType>
ValueType
Common_Math_SampleIterator_T<ValueType>::get (unsigned int index_in,
                                              unsigned int subSampleIndex_in) // i.e. channel#
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_SampleIterator_T::get"));

  // sanity check(s)
  ACE_ASSERT (buffer_);
  ACE_ASSERT (isInitialized_);

  // *TODO*: this isn't entirely portable
  switch (subSampleSize_)
  {
    case 1: // --> data is single-byte (possibly non-integer)
      return buffer_[(index_in * sampleSize_) + subSampleIndex_in];
    case 2:
      return (reverseByteOrder_ ? (isSignedSampleFormat_ ? ACE_SWAP_WORD (*reinterpret_cast<short*> (&buffer_[(index_in * sampleSize_) + (subSampleIndex_in * subSampleSize_)]))
                                                         : ACE_SWAP_WORD (*reinterpret_cast<unsigned short*> (&buffer_[(index_in * sampleSize_) + (subSampleIndex_in * subSampleSize_)])))
                                : (isSignedSampleFormat_ ? *reinterpret_cast<short*> (&buffer_[(index_in * sampleSize_) + (subSampleIndex_in * subSampleSize_)])
                                                         : *reinterpret_cast<unsigned short*> (&buffer_[(index_in * sampleSize_) + (subSampleIndex_in * subSampleSize_)])));
    case 4:
    { // --> non-integer type data is (most probably) a 32-bit IEEE float
      // *TODO*: this isn't very portable
      ACE_ASSERT (ACE_SIZEOF_FLOAT == 4);
      return (subSampleByteOrder_ ? (reverseByteOrder_ ? (isSignedSampleFormat_ ? ACE_SWAP_LONG (*reinterpret_cast<int*> (&buffer_[(index_in * sampleSize_) + (subSampleIndex_in * subSampleSize_)]))
                                                                                : ACE_SWAP_LONG (*reinterpret_cast<unsigned int*> (&buffer_[(subSampleIndex_in * sampleSize_) + (subSampleIndex_in * subSampleSize_)])))
                                                       : (isSignedSampleFormat_ ? *reinterpret_cast<int*> (&buffer_[(index_in * sampleSize_) + (subSampleIndex_in * subSampleSize_)])
                                                                                : *reinterpret_cast<unsigned int*> (&buffer_[(index_in * sampleSize_) + (subSampleIndex_in * subSampleSize_)])))
                                  : *reinterpret_cast<float*> (&buffer_[(index_in * sampleSize_) + (subSampleIndex_in * subSampleSize_)]));
    }
    case 8:
    {
      // --> non-integer type data is (most probably) a 64-bit IEEE float (aka double)
      ACE_ASSERT (ACE_SIZEOF_DOUBLE == 8);
      return *reinterpret_cast<double*> (&buffer_[(index_in * sampleSize_) + (subSampleIndex_in * subSampleSize_)]);
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("unknown/invalid subsample size (was: %d), aborting\n"),
                  subSampleSize_));
      break;
    }
  } // end SWITCH

  return 0;
}

template <typename ValueType>
bool
Common_Math_SampleIterator_T<ValueType>::initialize (unsigned int sampleSize_in,
                                                     unsigned int subSampleSize_in,
                                                     bool isSignedSampleFormat_in,
                                                     int subSampleByteOrder_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_SampleIterator_T::initialize"));

  // sanity check(s)
  ACE_ASSERT (sizeof (ValueType) >= subSampleSize_in);

  sampleSize_ = sampleSize_in;
  subSampleSize_ = subSampleSize_in;
  isSignedSampleFormat_ = isSignedSampleFormat_in;
  subSampleByteOrder_ = subSampleByteOrder_in;

  reverseByteOrder_ =
    subSampleByteOrder_ && (ACE_BYTE_ORDER != subSampleByteOrder_);

  isInitialized_ = true;

  return true;
}

//////////////////////////////////////////

template <typename ValueType>
Common_Math_Sample_T<ValueType>::Common_Math_Sample_T ()
 : buffer_ (NULL)
 , channels_ (0)
 , slots_ (0)
 , sampleRate_ (0)
 /////////////////////////////////////////
 , isInitialized_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Sample_T::Common_Math_Sample_T"));

}

template <typename ValueType>
Common_Math_Sample_T<ValueType>::Common_Math_Sample_T (unsigned int channels_in,
                                                       unsigned int slots_in,
                                                       unsigned int sampleRate_in)
 : buffer_ (NULL)
 , channels_ (channels_in)
 , slots_ (slots_in)
 , sampleRate_ (sampleRate_in)
 /////////////////////////////////////////
 , isInitialized_ (false)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Sample_T::Common_Math_Sample_T"));

  isInitialized_ = Initialize (slots_in,
                               sampleRate_in);
}

template <typename ValueType>
Common_Math_Sample_T<ValueType>::~Common_Math_Sample_T ()
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Sample_T::~Common_Math_Sample_T"));

  if (buffer_)
    for (unsigned int i = 0; i < channels_; ++i)
      delete [] buffer_[i];
  delete [] buffer_;
}

template <typename ValueType>
bool
Common_Math_Sample_T<ValueType>::Initialize (unsigned int channels_in,
                                             unsigned int slots_in,
                                             unsigned int sampleRate_in)
{
  COMMON_TRACE (ACE_TEXT ("Common_Math_Sample_T::Initialize"));

  // sanity check(s)
  if (slots_in % 2)
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("buffer size must be a power of 2 (was: %d), continuing\n"),
                slots_in));

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

    isInitialized_ = false;
  } // end IF

  ACE_NEW_NORETURN (buffer_,
                    ValueType*[channels_in]);
  if (!buffer_)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory, aborting\n")));
    return false;
  } // end IF
  for (unsigned int i = 0; i < channels_in; ++i)
  {
    ACE_NEW_NORETURN (buffer_[i],
                      ValueType[slots_in]);
    if (!buffer_[i])
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, aborting\n")));
      return false;
    } // end IF
  } // end FOR
#if 0
  // 1 kHz calibration wave
  for (unsigned int i = 0; i < slots_in; ++i)
    buffer_[i][j] = 1600 * sin (2.0 * M_PI * 1000.0 * j / sampleRate_in);
#else
  for (unsigned int i = 0; i < channels_in; ++i)
    for (unsigned int j = 0; j < slots_in; ++j)
      buffer_[i][j] = 0;
#endif // 0
  channels_ = channels_in;
  slots_ = slots_in;
  sampleRate_ = sampleRate_in;

  isInitialized_ = true;

  return true;
}

//void
//Common_Math_Sample_T<ValueType,
//                     Aggregation>::CopyIn (ITERATOR_T* iterator_in)
//{
//  COMMON_TRACE (ACE_TEXT ("Common_Math_Sample_T::CopyIn"));
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
