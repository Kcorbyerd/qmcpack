/////////////////////////////////////////////////////////////////
// (c) Copyright 2003-  by Jeongnim Kim
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//   National Center for Supercomputing Applications &
//   Materials Computation Center
//   University of Illinois, Urbana-Champaign
//   Urbana, IL 61801
//   e-mail: jnkim@ncsa.uiuc.edu
//   Tel:    217-244-6319 (NCSA) 217-333-3324 (MCC)
//
// Supported by 
//   National Center for Supercomputing Applications, UIUC
//   Materials Computation Center, UIUC
//////////////////////////////////////////////////////////////////
// -*- C++ -*-
/**@file WaveFunctionFactory.cpp
 *@brief Definition of a WaveFunctionFactory 
 */
#include "QMCWaveFunctions/WaveFunctionFactory.h"
#include "QMCWaveFunctions/JastrowBuilder.h"
#include "QMCWaveFunctions/MolecularOrbitals/MolecularOrbitalBuilder.h"
#include "QMCWaveFunctions/AtomicOrbitals/HeSTOClementiRottie.h"
#include "QMCWaveFunctions/PlaneWaveOrbitalBuilder.h"
#include "QMCWaveFunctions/JAABuilder.h"
#include "QMCWaveFunctions/JABBuilder.h"
#include "QMCWaveFunctions/NJAABuilder.h"
#include "QMCWaveFunctions/NJABBuilder.h"
#include "QMCWaveFunctions/JAAPBCBuilder.h"
#include "QMCWaveFunctions/WaveFunctionFactory.h"
#if defined(QMC_COMPLEX)
#include "QMCWaveFunctions/ElectronGasComplexOrbitalBuilder.h"
#else
#include "QMCWaveFunctions/ElectronGasOrbitalBuilder.h"
#include "QMCWaveFunctions/ThreeBodyGeminalBuilder.h"
#include "QMCWaveFunctions/ThreeBodyPadeBuilder.h"
#include "QMCWaveFunctions/AGPDeterminantBuilder.h"
#endif
#include "OhmmsData/AttributeSet.h"
namespace qmcplusplus {
  WaveFunctionFactory::WaveFunctionFactory(ParticleSet* qp, PtclPoolType& pset): 
    targetPtcl(qp),ptclPool(pset),targetPsi(0), myNode(NULL) {
      myName="psi0";
    }

  bool WaveFunctionFactory::build(xmlNodePtr cur, bool buildtree) {

    if(cur == NULL) return false;

    bool attach2Node=false;
    if(buildtree) {
      if(myNode == NULL) {
        myNode = xmlCopyNode(cur,1);
      } else {
        attach2Node=true;
      }
    }


    if(targetPsi==0) {//allocate targetPsi and set the name
      targetPsi  = new TrialWaveFunction;
      targetPsi->setName(myName);
      app_log() << "  Creating a trial wavefunction " << myName << endl;
    }

    cur = cur->children;
    bool success=true;
    while(cur != NULL) {
      string cname((const char*)(cur->name));
      if (cname == OrbitalBuilderBase::detset_tag) {
        success |= addFermionTerm(cur);
      } else if (cname ==  OrbitalBuilderBase::jastrow_tag) {
        success |= addJastrowTerm(cur);
      }
      if(attach2Node) xmlAddChild(myNode,xmlCopyNode(cur,1));
      cur = cur->next;
    }

    return success;
  }

  bool WaveFunctionFactory::addFermionTerm(xmlNodePtr cur) {

    OrbitalBuilderBase* detbuilder=0;

    string orbtype("MolecularOrbital");
    string nuclei("i");
    OhmmsAttributeSet oAttrib;
    oAttrib.add(orbtype,"type");
    oAttrib.add(nuclei,"source");
    oAttrib.put(cur);

    app_log() << "  Slater determinant terms using " << orbtype << endl;
#if defined(QMC_COMPLEX)
    if(orbtype == "electron-gas") {
      detbuilder = new ElectronGasComplexOrbitalBuilder(*targetPtcl,*targetPsi);
    } else if (orbtype == "PWBasis") {
      detbuilder = new PlaneWaveOrbitalBuilder(*targetPtcl,*targetPsi);
    } else {
      app_log() << "QMC_COMPLEX==1  Cannot create " << orbtype << endl;
    }
#else
    if(orbtype == "MolecularOrbital") {
      detbuilder = new MolecularOrbitalBuilder(*targetPtcl,*targetPsi,ptclPool);
    } else if(orbtype == "STO-He-Optimized") {

      PtclPoolType::iterator pit(ptclPool.find(nuclei));
      if(pit != ptclPool.end()) {
        detbuilder = new HePresetHFBuilder(*targetPtcl,*targetPsi,*((*pit).second));
      }
      //} else if(orbtype == "NumericalOrbital") {
      //  NumericalOrbitalSetBuilder a(*qp,*psi,ptclPool->getPool());
      //  a.put(cur);
    } else if(orbtype == "electron-gas") {
      detbuilder = new ElectronGasOrbitalBuilder(*targetPtcl,*targetPsi);
    } else if(orbtype == "PWBasis") {
      detbuilder = new PlaneWaveOrbitalBuilder(*targetPtcl,*targetPsi);
    } else if(orbtype == "AGP") {
      app_log() << "  Creating AGPDeterminant centers at " << nuclei << endl;
      PtclPoolType::iterator pit(ptclPool.find(nuclei));
      if(pit != ptclPool.end()) {
        detbuilder = new AGPDeterminantBuilder(*targetPtcl,*targetPsi,*((*pit).second));
      }
    }
#endif

    if(detbuilder) {//valid determinant set
      detbuilder->put(cur);
      addNode(detbuilder,cur);
      return true;
    } else {
      return false;
    }
  }

  bool WaveFunctionFactory::addJastrowTerm(xmlNodePtr cur) {

    string jasttype((const char*)(xmlGetProp(cur, (const xmlChar *)"type")));
    string jastname((const char*)(xmlGetProp(cur, (const xmlChar *)"name")));
    bool useSpline=false;
    const xmlChar* gptr=xmlGetProp(cur,(const xmlChar*)"transform");
    if(gptr != NULL) {
      if(xmlStrEqual(gptr,(const xmlChar*)"yes")) {
        useSpline=true;
      } 
    }

    OrbitalBuilderBase* jbuilder=0;

    if(jasttype == "Two-Body-Spin" || jasttype == "Two-Body") {
      if(useSpline) {
        app_log() << "  Using NJAABuilder for two-body jatrow with spline functions" << endl;
        jbuilder = new NJAABuilder(*targetPtcl,*targetPsi);
      } else {
        app_log() << "  Using JAABuilder for two-body jatrow with analytic functions" << endl;
        jbuilder = new JAABuilder(*targetPtcl,*targetPsi);
      }
    } else if(jasttype == "Long-Range") {
      app_log() << "  Using JAAPBCBuilder for two-body jatrow TESTING ONLY" << endl;
      jbuilder = new JAAPBCBuilder(*targetPtcl,*targetPsi);
    } else if(jasttype == "One-Body") {
      if(useSpline) {
        app_log() << "  Using NJABBuilder for one-body jatrow with spline functions" << endl;
        jbuilder = new NJABBuilder(*targetPtcl,*targetPsi,ptclPool);
      } else {
        app_log() << "  Using JABBuilder for one-body jatrow with analytic functions" << endl;
        jbuilder = new JABBuilder(*targetPtcl,*targetPsi,ptclPool);
      }
    } 
#if !defined(QMC_COMPLEX)
    else if(jasttype == "Three-Body-Geminal") {
      app_log() << "  creating Three-Body-Germinal Jastrow function " << endl;
      string source_name("i");
      const xmlChar* iptr = xmlGetProp(cur, (const xmlChar *)"source");
      if(iptr != NULL) source_name=(const char*)iptr;
      PtclPoolType::iterator pit(ptclPool.find(source_name));
      if(pit != ptclPool.end()) {
        cout << "Creating three body with " << source_name << endl;
        jbuilder = new ThreeBodyGeminalBuilder(*targetPtcl,*targetPsi,*((*pit).second));
      }
    } else if (jasttype == "Three-Body-Pade") {
      app_log() << "  creating Three-Body-Pade Jastrow function " << endl;
      string source_name("i");
      const xmlChar* iptr = xmlGetProp(cur, (const xmlChar *)"source");
      //if(iptr != NULL) source_name=(const char*)iptr;
      PtclPoolType::iterator pit(ptclPool.find(source_name));
      if(pit != ptclPool.end()) {
        jbuilder = new ThreeBodyPadeBuilder(*targetPtcl,*targetPsi,*((*pit).second));
      }
    }
#endif

    if(jbuilder) {
      jbuilder->put(cur);
      addNode(jbuilder,cur);
      return true;
    } else {
      app_warning() << "    " << jasttype << " is not valid." << endl;
      return false;
    }
  }

  bool WaveFunctionFactory::addNode(OrbitalBuilderBase* b, xmlNodePtr cur) {
    psiBuilder.push_back(b);
    ///if(myNode != NULL) {
    ///  cout << ">>>> Adding " << (const char*)cur->name << endl;
    ///  xmlAddChild(myNode,xmlCopyNode(cur,1));
    ///}
    return true;
  }

  void WaveFunctionFactory::setCloneSize(int np) {
    myClones.resize(np,0);
  }

  WaveFunctionFactory*
  WaveFunctionFactory::clone(ParticleSet* qp, int ip, const string& aname) {
    WaveFunctionFactory* aCopy= new WaveFunctionFactory(qp,ptclPool);
    aCopy->setName(aname);
    aCopy->build(myNode,false);
    myClones[ip]=aCopy;
    return aCopy;
  }

  WaveFunctionFactory::~WaveFunctionFactory() {
    //clean up everything
  }

  bool WaveFunctionFactory::get(std::ostream& ) const {
    return true;
  }

  bool WaveFunctionFactory::put(std::istream& ) {
    return true;
  }

  bool WaveFunctionFactory::put(xmlNodePtr cur) {
    return build(cur,true);
  }

  void WaveFunctionFactory::reset() { }
}
/***************************************************************************
 * $RCSfile$   $Author$
 * $Revision$   $Date$
 * $Id$ 
 ***************************************************************************/
