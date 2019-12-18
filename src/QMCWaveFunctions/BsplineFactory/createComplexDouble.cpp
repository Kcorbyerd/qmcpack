//////////////////////////////////////////////////////////////////////////////////////
// This file is distributed under the University of Illinois/NCSA Open Source License.
// See LICENSE file in top directory for details.
//
// Copyright (c) 2016 Jeongnim Kim and QMCPACK developers.
//
// File developed by:
//
// File created by: Jeongnim Kim, jeongnim.kim@intel.com, Intel Corp.
//////////////////////////////////////////////////////////////////////////////////////


#include "QMCWaveFunctions/BsplineFactory/createBsplineReader.h"
#include "Numerics/e2iphi.h"
#include "simd/vmath.hpp"
#include <Utilities/ProgressReportEngine.h>
#include "QMCWaveFunctions/EinsplineSetBuilder.h"
#include "QMCWaveFunctions/BsplineFactory/BsplineSet.h"
#include "QMCWaveFunctions/BsplineFactory/SplineC2RAdoptor.h"
#include "QMCWaveFunctions/BsplineFactory/SplineC2CAdoptor.h"
#if defined(ENABLE_OFFLOAD)
#include "QMCWaveFunctions/BsplineFactory/SplineC2ROMP.h"
#endif
#include "QMCWaveFunctions/BsplineFactory/HybridCplxAdoptor.h"
#include <fftw3.h>
#include <QMCWaveFunctions/einspline_helper.hpp>
#include "QMCWaveFunctions/BsplineFactory/BsplineReaderBase.h"
#include "QMCWaveFunctions/BsplineFactory/SplineAdoptorReaderP.h"
#include "QMCWaveFunctions/BsplineFactory/SplineHybridAdoptorReaderP.h"

namespace qmcplusplus
{
BsplineReaderBase* createBsplineComplexDouble(EinsplineSetBuilder* e, bool hybrid_rep, const std::string& useGPU)
{
  typedef OHMMS_PRECISION RealType;
  BsplineReaderBase* aReader = nullptr;

#if defined(QMC_COMPLEX)
  if (hybrid_rep)
    aReader = new SplineHybridAdoptorReader<HybridCplxSoA<SplineC2CSoA<double>>>(e);
  else
    aReader = new SplineAdoptorReader<SplineC2CSoA<double>>(e);
#else //QMC_COMPLEX
#if defined(ENABLE_OFFLOAD)
  if (useGPU == "yes")
  {
    aReader = new SplineAdoptorReader<SplineC2ROMP<double, RealType>>(e);
  }
  else
#endif
  {
    if (hybrid_rep)
      aReader = new SplineHybridAdoptorReader<HybridCplxSoA<SplineC2RSoA<double>>>(e);
    else
      aReader = new SplineAdoptorReader<SplineC2RSoA<double>>(e);
  }
#endif

  return aReader;
}
} // namespace qmcplusplus
