//////////////////////////////////////////////////////////////////////////////////////
// This file is distributed under the University of Illinois/NCSA Open Source License.
// See LICENSE file in top directory for details.
//
// Copyright (c) 2020 QMCPACK developers.
//
// File developed by: Cody A. Melton, cmelton@sandia.gov, Sandia National Laboratories
//
// File created by: Cody A. Melton, cmelton@sandia.gov, Sandia National Laboratories
//////////////////////////////////////////////////////////////////////////////////////

#ifndef QMCPLUSPLUS_TOOLS_DIRAC_OUT_H
#define QMCPLUSPLUS_TOOLS_DIRAC_OUT_H

#include "QMCTools/QMCGaussianParserBase.h"
#include <string>
#include <map>
#include <cmath>

typedef std::pair<double, double> primBasis;

struct basisGroup
{
  int l;
  int n;
  std::vector<primBasis> radfuncs;
};

struct atBasisSet
{
  std::string elementType;
  std::vector<basisGroup> basisGroups;
};

struct fermIrrep
{
  std::string label;
  std::vector<double> energies;
  std::vector<std::vector<std::vector<double>>> spinor_mo_coeffs; //mo,ao,up_r,up_i,dn_r,dn_i
  fermIrrep(std::string label_in, int nSpinors, int numAO);
  fermIrrep generate_kramers_pair();
  int get_num_spinors() { return energies.size(); }
  int get_num_ao() { return spinor_mo_coeffs[0].size(); }
  std::string get_label() { return label; }
};

class DiracParser : public QMCGaussianParserBase, public OhmmsAsciiParser
{
  typedef std::map<std::string, double> normMapType;

public:
  DiracParser(int argc, char** argv);
  void parse(const std::string& fname) override;

private:
  void dumpHDF5(const std::string& fname);
  void getGeometry(std::istream& is);
  void getGaussianCenters(std::istream& is);
  void getSpinors(std::istream& is);
  std::streampos pivot_begin;
  int NumberOfSpecies;
  int version;
  std::string aline;
  std::vector<atBasisSet> basisset;
  normMapType normMap;

  std::vector<fermIrrep> irreps;
  std::vector<fermIrrep> kp_irreps;
};


#endif
