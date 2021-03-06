// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
/*****************************************************************************
 *
 *     Author: Xilinx, Inc.
 *
 *     This text contains proprietary, confidential information of
 *     Xilinx, Inc. , is distributed by under license from Xilinx,
 *     Inc., and may be used, copied and/or disclosed only pursuant to
 *     the terms of a valid license agreement with Xilinx, Inc.
 *
 *     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
 *     AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
 *     SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
 *     OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
 *     APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
 *     THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
 *     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
 *     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
 *     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
 *     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
 *     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
 *     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE.
 *
 *     Xilinx products are not intended for use in life support appliances,
 *     devices, or systems. Use in such applications is expressly prohibited.
 *
 *     (c) Copyright 2014 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

#ifndef HLS_SQRT_CORDIC_H
#define HLS_SQRT_CORDIC_H

#if defined(__cplusplus) && (__cplusplus >= 201402L)
#define HLS_DEPRECATED_FUNC(removal, replacement) \
   [[deprecated("It will be removed from HLS in Release " #removal ", please consider using " #replacement " instead.")]]
#elif defined(__GNUC__) || defined(__clang__)
#define HLS_DEPRECATED_FUNC(removal, replacement) \
   __attribute__((deprecated("It will be removed from HLS in Release " #removal ", please consider using " #replacement " instead.")))
#elif defined(_MSC_VER)
#define HLS_DEPRECATED_FUNC(removal, replacement) \
   __declspec(deprecated("It will be removed from HLS in Release " #removal ", please consider using " #replacement " instead."))
#else
#error("Unsupported compiler")
#endif

#include "hls/dsp/utils/hls_cordic.h"

namespace hls {

  // ===================================================================================================================
  // Input and output interfaces
  template <int InputWidth, int DataFormat> struct sqrt_input {
    typedef cordic_inputs<InputWidth,CORDIC_F_SQRT,DataFormat> in;
  };

  template <int OutputWidth, int DataFormat> struct sqrt_output {
    typedef cordic_outputs<OutputWidth,CORDIC_F_SQRT,DataFormat> out;
  };

  // ===================================================================================================================
  // SQRT: Entry point function.
  // o Template parameters:
  //  - DataFormat       : Selects between unsigned fraction (with integer width of 1 bit) and unsigned integer formats
  //  - InputWidth       : Defines overall input data width
  //  - OutputWidth      : Defines overall output data width
  //  - RoundMode        : Selects the rounding mode to apply to the output data
  // o Arguments:
  //  - x                : Input data
  //  - sqrtX            : Square root of input data
  // o The internal CORDIC function applies its own rounding, therefore the interface
  //   ap_fixed types need not specify rounding and saturation modes
  template <
    int DataFormat,
    int InputWidth,
    int OutputWidth,
    int RoundMode
    > 
    HLS_DEPRECATED_FUNC(2021.1, hls::sqrt from HLS Math)
    void sqrt(const typename sqrt_input<InputWidth, DataFormat>::in &x,
                typename sqrt_output<OutputWidth, DataFormat>::out &sqrtX) {

    Function_sqrt_cordic:;

    cordic_base<
      CORDIC_F_SQRT,
      CORDIC_FALSE,
      DataFormat,
      CORDIC_FORMAT_RAD,
      InputWidth,
      OutputWidth,
      CORDIC_ITER_AUTO,
      CORDIC_PREC_AUTO,
      RoundMode,
      CORDIC_SCALE_NONE>(x, sqrtX);
  }

} // end namespace hls

#endif // HLS_SQRT_CORDIC_H


