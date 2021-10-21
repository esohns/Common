#ifndef COMMON_MATH_SAMPLE_T
#define COMMON_MATH_SAMPLE_T

#include "ace/Global_Macros.h"

// *TODO*: 'macroize' this code. Data access should really have (nearly) zero
//         overhead. If that is not possible, sub-class ASAP
template <typename ValueType>
class SampleIterator_T
{
 public:
  SampleIterator_T (char*); // buffer
  inline virtual ~SampleIterator_T () {}

  // *TODO*: needs more serious consideration
  bool initialize (unsigned int, // bytes / ('buffer'/'chunk'/'frame'/...) sample
                   unsigned int, // bytes / subsample (if any)
                   bool,         // signed subsample format ?
                   int);         // subsample byte order (ACE-style, 0: N/A)
  //inline void set (char* buffer_in) { buffer_ = buffer_in; };
  ValueType get (unsigned int,  // index (i.e. #sample into buffer)
                 unsigned int); // subsample index (e.g. 0: mono/stereo left,
                                //                       1: stereo right, ...)

  char*        buffer_;
  bool         reverseByteOrder_;
  unsigned int sampleSize_;
  unsigned int subSampleSize_;

 private:
  ACE_UNIMPLEMENTED_FUNC (SampleIterator_T ())
  ACE_UNIMPLEMENTED_FUNC (SampleIterator_T (const SampleIterator_T&))
  ACE_UNIMPLEMENTED_FUNC (SampleIterator_T& operator= (const SampleIterator_T&))

  bool         isInitialized_;
  bool         isSignedSampleFormat_;
  int          subSampleByteOrder_; // ACE-style, 0: N/A
};

//////////////////////////////////////////

template <typename ValueType>
class Common_Math_Sample_T
{
 public:
  Common_Math_Sample_T ();
  Common_Math_Sample_T (unsigned int,  // #channels
                        unsigned int,  // #buffered samples ('magnitude' of the buffer)
                        unsigned int); // source sample rate (Hz)
  virtual ~Common_Math_Sample_T ();

  typedef SampleIterator_T<ValueType> ITERATOR_T;

  //void CopyIn (ITERATOR_T*); // sample iterator
  bool Initialize (unsigned int,  // #channels
                   unsigned int,  // #slots
                   unsigned int); // source sample rate (Hz)
  // *WARNING*: indexes are 0-based
  virtual void Process (unsigned int,      // channel#
                        unsigned int,      // starting slot index
                        unsigned int) = 0; // ending slot index (inclusive)

  inline unsigned int Slots () const { return slots_; }

  virtual ValueType Value (unsigned int,            // channel#
                           unsigned int) const = 0; // slot#
//  { ACE_ASSERT (slot# < slots_);
//    ACE_ASSERT (channel# < Aggregation);
//    return static_cast<ValueType> (buffer_[channel#][slot#]);
//  }

  inline unsigned int HzToSlot (unsigned int frequency_in) const { return (static_cast<unsigned int> (slots_ * frequency_in) / sampleRate_); }

 protected:
  ValueType**  buffer_;     // sample data (may contain several 'channels')
  unsigned int channels_;   // #channels
  unsigned int slots_;      // #buffered samples ('magnitude' of the buffer)
  unsigned int sampleRate_; // source sample rate

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Math_Sample_T (const Common_Math_Sample_T&))
  ACE_UNIMPLEMENTED_FUNC (Common_Math_Sample_T& operator= (const Common_Math_Sample_T&))

  bool         isInitialized_;
};

// include template definition
#include "common_math_sample.inl"

#endif
