//////////////////////////////////////////////////////////////////////////////////////
// This file is distributed under the University of Illinois/NCSA Open Source License.
// See LICENSE file in top directory for details.
//
// Copyright (c) 2021 QMCPACK developers.
//
// File developed by: Jaron T. Krogel, krogeljt@ornl.gov, Oak Ridge National Laboratory
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef QMCPLUSPLUS_SHOVERLAP_H
#define QMCPLUSPLUS_SHOVERLAP_H
#include <vector>

#include "Configuration.h"
#include "OperatorEstBase.h"
#include "Containers/OhmmsPETE/TinyVector.h"

#include "SHOverlapInput.h"


namespace qmcplusplus
{
/** Class that collects momentum distribution of electrons
 *  
 */
class SHOverlap : public OperatorEstBase
{
public:
  using LatticeType = PtclOnLatticeTraits::ParticleLayout;
  using RealType    = QMCTraits::RealType;
  using ComplexType = QMCTraits::ComplexType;
  using ValueType   = QMCTraits::ValueType;
  using PosType     = QMCTraits::PosType;

  //data members set only during construction
  const SHOverlapInput input_;

  /** @ingroup SHOverlap mutable data members
   */

public:
  /** Constructor for SHOverlapInput 
   */
  SHOverlap(SHOverlapInput&& inp,
                       DataLocality dl = DataLocality::crowd);

  /** Constructor used when spawing crowd clones
   *  needs to be public so std::make_unique can call it.
   *  Do not use directly unless you've really thought it through.
   */
  SHOverlap(const SHOverlap& sh, DataLocality dl);

  /** This allows us to allocate the necessary data for the DataLocality::queue 
   */
  void startBlock(int steps) override;

  /** standard interface
   */
  std::unique_ptr<OperatorEstBase> spawnCrowdClone() const override;

  /** accumulate 1 or more walkers of SHOverlap samples
   */
  void accumulate(const RefVector<MCPWalker>& walkers,
                  const RefVector<ParticleSet>& psets,
                  const RefVector<TrialWaveFunction>& wfns,
                  RandomGenerator& rng) override;

  /** this allows the EstimatorManagerNew to reduce without needing to know the details
   *  of SHOverlap's data.
   *
   *  can use base class default until crowd level SHOverlap
   *  estimators don't have a copy of the density grid.
   */
  void collect(const RefVector<OperatorEstBase>& operator_estimators) override;

  /** this allows the EstimatorManagerNew to reduce without needing to know the details
   *  of SHOverlap's data.
   *
   *  can use base class default until crowd level SHOverlap estimators don't have a copy of the density grid.
   */
  //void collect(const OperatorEstBase&  oeb);

  /** this gets us into the hdf5 file
   *
   *  Just parroting for now don't fully understand.
   *, needs to be unraveled and simplified the hdf5 output is another 
   *  big state big coupling design.
   */
  void registerOperatorEstimator(hdf_archive& file) override;

private:
  SHOverlap(const SHOverlap& md) = default;
};

} // namespace qmcplusplus

#endif /* QMCPLUSPLUS_SHOVERLAP_H */
