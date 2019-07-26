#include "stdafx.h"

#include "common_math_pidcontrol_zieglernichols.h"

#include <algorithm>

#include "ace/Log_Msg.h"

Common_PIDControl_ZieglerNichols::Common_PIDControl_ZieglerNichols (double Ku_in,
                                                                    double Tu_in,
                                                                    double uMax_in,
                                                                    double uMin_in)
 : inherited (0.8 * Ku_in,                 // Kp
              0.0,                         // Ki
              0.1 * (0.8 * Ku_in) * Tu_in, // Kd
              uMax_in,                     // uMax
              uMin_in)                     // uMin
 , actual_ (1.0) // tau
{
  COMMON_TRACE (ACE_TEXT ("Common_PIDControl_ZieglerNichols::Common_PIDControl_ZieglerNichols"));

  actual_.reset (0.03);

//  controllerLongitudinal(  0.32, 0.0, 0.16, 1.0 ), // kU =  0.4, TU = 5.0
//  controllerLateral(       0.32, 0.0, 0.16, 1.0 ), // kU =  0.4, TU = 5.0
//  controllerVertical(      5.20, 0.0, 1.11, 1.0 ), // kU =  6.5, TU = 1.7
//  controllerYaw(          13.60, 0.0, 1.14, 1.0 ), // kU = 17.0, TU = 0.6

//  // later, after manual tuning of height control...

//  // ...enter your measured critical gain and period
//  const double kPCritVertical =  6.5 ; // STUD_SET_NAN
//  const double TCritVertical  =  1.7 ; // STUD_SET_NAN

//  // ...use Ziegler-Nichols equations to find parameters for integrated PID controller
//  this->controllerVertical.kP = STUD_NAN( 0.800 ) * kPCritVertical                 ; // STUD_TODO enter factors from script
//  this->controllerVertical.kI = STUD_NAN( 0.000 ) * kPCritVertical / TCritVertical ; // STUD_TODO
//  this->controllerVertical.kD = STUD_NAN( 0.100 ) * kPCritVertical * TCritVertical ; // STUD_TODO

//  // later, after manual tuning of heading control...

//  // ...enter your measured critical gain and period
//  const double kPCritYaw = 12.0 ; // STUD_SET_NAN
//  const double TCritYaw  =  0.5 ; // STUD_SET_NAN

//  // ...use Ziegler-Nichols equations to find parameters for integrated PID controller
//  this->controllerYaw.kP = STUD_NAN( 0.800 ) * kPCritYaw            ; // STUD_TODO enter factors from script
//  this->controllerYaw.kI = STUD_NAN( 0.000 ) * kPCritYaw / TCritYaw ; // STUD_TODO
//  this->controllerYaw.kD = STUD_NAN( 0.100 ) * kPCritYaw * TCritYaw ; // STUD_TODO
}
