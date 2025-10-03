/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef COMMON_TOOLS_H
#define COMMON_TOOLS_H

#include <random>
#include <sstream>
#include <string>
#include <type_traits>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#define __CGUID_H__
#include "combaseapi.h"
#include "guiddef.h"
#include "minwindef.h"
#else
#include "sys/capability.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/ACE.h"
#include "ace/Global_Macros.h"

#include "common.h"
#include "common_configuration.h"
#include "common_iinitialize.h"

// forward declaration(s)
class ACE_Event_Handler;

class Common_Tools
 : public Common_SInitializeFinalize_T<Common_Tools>
{
 public:
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  static void initialize (bool,  // initialize COM ?
                          bool); // initialize random number generator ?
#else
  static void initialize (bool); // initialize random number generator ?
#endif // ACE_WIN32 || ACE_WIN64
  static void finalize ();

  // --- platform ---

  // limits
  template <typename ValueType>
  static std::enable_if_t<std::is_integral<ValueType>::value, ValueType> min (unsigned int, // number of bytes
                                                                              bool);        // signed ? : unsigned
  template <typename ValueType>
  static std::enable_if_t<std::is_integral<ValueType>::value, ValueType> max (unsigned int, // number of bytes
                                                                              bool);        // signed ? : unsigned
  template <typename ValueType>
  static std::enable_if_t<!std::is_integral<ValueType>::value, ValueType> min (unsigned int); // number of bytes
  template <typename ValueType>
  static std::enable_if_t<!std::is_integral<ValueType>::value, ValueType> max (unsigned int); // number of bytes

  // endianness
  template <typename ValueType>
  static ValueType byteSwap (ValueType);

  // RTTI
  template <typename Type,
            typename Type_2>
  static bool equalType (Type*,    // type 1 pointer
                         Type_2*); // type 2 pointer

  // architecture
  static unsigned int getNumberOfCPUs (bool = true); // consider logical cores (i.e. 'hyperthreading') ?
  static std::string getPlatformName ();
  static enum Common_OperatingSystemType getOperatingSystem ();
  inline static bool is (enum Common_OperatingSystemType operatingSytem_in) { return (Common_Tools::getOperatingSystem () == operatingSytem_in); }
#if defined (ACE_LINUX)
  static enum Common_OperatingSystemDistributionType getDistribution (unsigned int&,  // return value: major version
                                                                      unsigned int&,  // return value: minor version
                                                                      unsigned int&); // return value: micro version
#endif // ACE_LINUX
  static std::string getHostName (); // return value: host name (see: man hostname(2))

  // --- compiler ---
  // *NOTE*: these (currently) return static (i.e. compile-time) information
  inline static std::string compilerName_ACE () { return ACE_TEXT_ALWAYS_CHAR (ACE::compiler_name ()); }
  static std::string compilerName ();
  static std::string compilerPlatformName ();
  static std::string compilerVersion ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // --- COM ---
  // *IMPORTANT NOTE*: iff this returns 'true', invoke finalizeCOM()
  static bool initializeCOM (DWORD = COMMON_WIN32_COM_INITIALIZATION_DEFAULT_FLAGS);
  inline static void finalizeCOM () { CoUninitialize (); }
#endif // ACE_WIN32 || ACE_WIN64

  // --- randomization ---
  template <typename ValueType>
  static std::enable_if_t<std::is_integral<ValueType>::value, ValueType> getRandomNumber (ValueType,  // start (inclusive)
                                                                                          ValueType); // end   (inclusive)
  template <typename ValueType>
  static std::enable_if_t<!std::is_integral<ValueType>::value, ValueType> getRandomNumber (ValueType,  // start (inclusive)
                                                                                           ValueType); // end   (inclusive)
  template <typename ValueType>
  inline static ValueType getRandomNumber (std::uniform_int_distribution<ValueType>& distribution_in) { return distribution_in (Common_Tools::randomEngine); }
  template <typename ValueType>
  inline static ValueType getRandomNumber (std::uniform_real_distribution<ValueType>& distribution_in) { return distribution_in (Common_Tools::randomEngine); }
  template <typename ValueType>
  inline static ValueType getRandomNumber (std::normal_distribution<ValueType>& distribution_in) { return distribution_in (Common_Tools::randomEngine); }

  // *IMPORTANT NOTE*: relies on the uniformity of distribution of the RNG's values
  static bool testRandomProbability (float); // between 0.0f (0%) and 1.0f (100%)

  static unsigned int                  randomSeed;

  // --- libraries ---
  inline static std::string compiledVersion_ACE () { std::ostringstream converter; converter << ACE_MAJOR_VERSION; converter << ACE_TEXT_ALWAYS_CHAR ("."); converter << ACE_MINOR_VERSION; converter << ACE_TEXT_ALWAYS_CHAR ("."); converter << ACE_MICRO_VERSION; return converter.str (); }

 private:
  ACE_UNIMPLEMENTED_FUNC (Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (Common_Tools (const Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (Common_Tools& operator= (const Common_Tools&))

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *TODO*: thread-specific --> place this in TSS
  static bool                          COMInitialized;
#endif // ACE_WIN32 || ACE_WIN64

  // --- randomization ---
  //typedef std::array<COMMON_APPLICATION_RNG_ENGINE::result_type,
  //                   COMMON_APPLICATION_RNG_ENGINE::state_size> PRNG_SEED_ARRAY_T;
  //typedef PRNG_SEED_ARRAY_T::iterator_type PRNG_SEED_ARRAY_ITERATOR_T;

  static COMMON_APPLICATION_RNG_ENGINE randomEngine;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  static char                          randomStateBuffer[BUFSIZ];
#endif // ACE_WIN32 || ACE_WIN64
};

// include template definition
#include "common_tools.inl"

#endif
