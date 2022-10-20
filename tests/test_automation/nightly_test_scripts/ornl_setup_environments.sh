#!/bin/bash

echo --- START environment setup `date`

# serial   : single install
# 8up      : 8 installs
# makefile : make -j
parallelmode=makefile

install_environment () {
case "$parallelmode" in
    serial )
	echo --- Serial install
	spack install
	;;
    8up )
	echo --- Running 8 installs simultaneously
        spack install & spack install & spack install & spack install & spack install & spack install & spack install & spack install
	;;
    makefile )
	echo --- Install via parallel make
	spack concretize
	spack env depfile >Makefile
	make -j
	;;
    * )
	echo Unknown parallelmode
	exit 1
	;;
esac
}

here=`pwd`

if [ -e `dirname "$0"`/ornl_versions.sh ]; then
    echo --- Contents of ornl_versions.sh
    cat `dirname "$0"`/ornl_versions.sh
    echo --- End of contents of ornl_versions.sh
    source `dirname "$0"`/ornl_versions.sh
else
    echo Did not find version numbers script ornl_versions.sh
    exit 1
fi

plat=`lscpu|grep Vendor|sed 's/.*ID:[ ]*//'`
case "$plat" in
    GenuineIntel )
	ourplatform=Intel
	;;
    AuthenticAMD )
	ourplatform=AMD
	;;
    * )
	# ARM support should be trivial, but is not yet done
	echo Unknown platform
	exit 1
	;;
esac

echo --- Installing for $ourplatform architecture
ourhostname=`hostname|sed 's/\..*//g'`
echo --- Host is $ourhostname

theenv=envgccnewmpi
echo --- Setting up $theenv `date`
spack env create $theenv
sed -i "s/unify: false/unify: true/g" $HOME/apps/spack/var/spack/environments/$theenv/spack.yaml
spack env activate $theenv

spack add gcc@${gcc_vnew}%gcc@${gcc_vnew}
spack add git
spack add ninja
spack add cmake@${cmake_vnew}
spack add libxml2@${libxml2_v}%gcc@${gcc_vnew}
spack add boost@${boost_vnew}%gcc@${gcc_vnew}
spack add util-linux-uuid%gcc@${gcc_vnew}
spack add python%gcc@${gcc_vnew}
spack add openmpi@${ompi_vnew}%gcc@${gcc_vnew}
spack add hdf5@${hdf5_vnew}%gcc@${gcc_vnew} +fortran +hl +mpi
spack add fftw@${fftw_vnew}%gcc@${gcc_vnew} -mpi #Avoid MPI for simplicity
spack add openblas%gcc@${gcc_vnew} threads=openmp
#spack add blis%gcc@${gcc_vnew} threads=openmp
#spack add libflame%gcc@${gcc_vnew} threads=openmp

spack add py-lxml
spack add py-matplotlib
spack add py-pandas
spack add py-mpi4py
spack add py-scipy
spack add py-h5py ^hdf5@${hdf5_vnew}%gcc@${gcc_vnew} +fortran +hl +mpi
spack add quantum-espresso +mpi +qmcpack
spack add py-pyscf@2.0.1
spack add rmgdft
install_environment
spack env deactivate

theenv=envgccnewnompi
echo --- Setting up $theenv `date`
spack env create $theenv
sed -i "s/unify: false/unify: true/g" $HOME/apps/spack/var/spack/environments/$theenv/spack.yaml
spack env activate $theenv

spack add gcc@${gcc_vnew}%gcc@${gcc_vnew}
spack add git
spack add ninja
spack add cmake@${cmake_vnew}
spack add libxml2@${libxml2_v}%gcc@${gcc_vnew}
spack add boost@${boost_vnew}%gcc@${gcc_vnew}
spack add util-linux-uuid%gcc@${gcc_vnew}
spack add python%gcc@${gcc_vnew}
#spack add openmpi@${ompi_vnew}%gcc@${gcc_vnew}
spack add hdf5@${hdf5_vnew}%gcc@${gcc_vnew} +fortran +hl ~mpi
spack add fftw@${fftw_vnew}%gcc@${gcc_vnew} -mpi #Avoid MPI for simplicity
spack add openblas%gcc@${gcc_vnew} threads=openmp
#spack add blis%gcc@${gcc_vnew} threads=openmp
#spack add libflame%gcc@${gcc_vnew} threads=openmp

spack add py-lxml
spack add py-matplotlib
spack add py-pandas
#spack add py-mpi4py
spack add py-scipy
spack add py-h5py ^hdf5@${hdf5_vnew}%gcc@${gcc_vnew} +fortran +hl ~mpi
install_environment
spack env deactivate

theenv=envgccoldnompi
echo --- Setting up $theenv `date`
spack env create $theenv
sed -i "s/unify: false/unify: true/g" $HOME/apps/spack/var/spack/environments/$theenv/spack.yaml
spack env activate $theenv

spack add gcc@${gcc_vold}
spack add git
spack add ninja
spack add cmake@${cmake_vold}
spack add libxml2@${libxml2_v}%gcc@${gcc_vold}
spack add boost@${boost_vold}%gcc@${gcc_vold}
spack add util-linux-uuid%gcc@${gcc_vold}
spack add python%gcc@${gcc_vold}
#spack add openmpi@${ompi_vnew}%gcc@${gcc_vold}
spack add hdf5@${hdf5_vold}%gcc@${gcc_vold} +fortran +hl ~mpi
spack add fftw@${fftw_vold}%gcc@${gcc_vold} -mpi #Avoid MPI for simplicity
spack add openblas%gcc@${gcc_vold} threads=openmp
#spack add blis%gcc@${gcc_vold} threads=openmp
#spack add libflame%gcc@${gcc_vold} threads=openmp

spack add py-lxml
spack add py-matplotlib
spack add py-pandas
#spack add py-mpi4py
spack add py-scipy
spack add py-h5py ^hdf5@${hdf5_vnew}%gcc@${gcc_vold} +fortran +hl ~mpi
install_environment
spack env deactivate

theenv=envgccoldmpi
echo --- Setting up $theenv `date`
spack env create $theenv
sed -i "s/unify: false/unify: true/g" $HOME/apps/spack/var/spack/environments/$theenv/spack.yaml
spack env activate $theenv

spack add gcc@${gcc_vold}
spack add git
spack add ninja
spack add cmake@${cmake_vold}
spack add libxml2@${libxml2_v}%gcc@${gcc_vold}
spack add boost@${boost_vold}%gcc@${gcc_vold}
spack add util-linux-uuid%gcc@${gcc_vold}
spack add python%gcc@${gcc_vold}
spack add openmpi@${ompi_vnew}%gcc@${gcc_vold}
spack add hdf5@${hdf5_vold}%gcc@${gcc_vold} +fortran +hl +mpi
spack add fftw@${fftw_vold}%gcc@${gcc_vold} -mpi #Avoid MPI for simplicity
spack add openblas%gcc@${gcc_vold} threads=openmp
#spack add blis%gcc@${gcc_vold} threads=openmp
#spack add libflame%gcc@${gcc_vold} threads=openmp

spack add py-lxml
spack add py-matplotlib
spack add py-pandas
spack add py-mpi4py
spack add py-scipy
spack add py-h5py ^hdf5@${hdf5_vnew}%gcc@${gcc_vold} +fortran +hl +mpi
install_environment
spack env deactivate

theenv=envclangnewmpi
echo --- Setting up $theenv `date`
spack env create $theenv
sed -i "s/unify: false/unify: true/g" $HOME/apps/spack/var/spack/environments/$theenv/spack.yaml
spack env activate $theenv

spack add gcc@${gcc_vnew}%gcc@${gcc_vnew}
spack add llvm@${llvm_vnew}%gcc@${gcc_vnew}
spack add git
spack add ninja
spack add cmake@${cmake_vnew}
spack add libxml2@${libxml2_v}%gcc@${gcc_vnew}
spack add boost@${boost_vnew}%gcc@${gcc_vnew}
spack add util-linux-uuid%gcc@${gcc_vnew}
spack add python%gcc@${gcc_vnew}
spack add openmpi@${ompi_vnew}%gcc@${gcc_vnew}
spack add hdf5@${hdf5_vnew}%gcc@${gcc_vnew} +fortran +hl +mpi
spack add fftw@${fftw_vnew}%gcc@${gcc_vnew} -mpi #Avoid MPI for simplicity
spack add openblas%gcc@${gcc_vnew} threads=openmp
#spack add blis%gcc@${gcc_vnew} threads=openmp
#spack add libflame%gcc@${gcc_vnew} threads=openmp

spack add py-lxml
spack add py-matplotlib
spack add py-pandas
spack add py-mpi4py
spack add py-scipy
spack add py-h5py ^hdf5@${hdf5_vnew}%gcc@${gcc_vnew} +fortran +hl +mpi
install_environment
spack env deactivate

theenv=envinteloneapinompi
echo --- Setting up $theenv `date`
spack env create $theenv
sed -i "s/unify: false/unify: true/g" $HOME/apps/spack/var/spack/environments/$theenv/spack.yaml
spack env activate $theenv

spack add gcc@${gcc_vintel}
spack add git
spack add ninja
spack add cmake@${cmake_vnew}
spack add libxml2@${libxml2_v}%gcc@${gcc_vintel}
spack add boost@${boost_vnew}%gcc@${gcc_vintel}
spack add util-linux-uuid%gcc@${gcc_vintel}
spack add python%gcc@${gcc_vintel}
spack add hdf5@${hdf5_vnew}%gcc@${gcc_vintel} +fortran +hl ~mpi
spack add fftw@${fftw_vnew}%gcc@${gcc_vintel} -mpi #Avoid MPI for simplicity

spack add py-lxml
spack add py-matplotlib
spack add py-pandas
spack add py-scipy
spack add py-h5py ^hdf5@${hdf5_vnew}%gcc@${gcc_vintel} +fortran +hl ~mpi
install_environment
spack env deactivate

theenv=envinteloneapimpi
echo --- Setting up $theenv `date`
spack env create $theenv
sed -i "s/unify: false/unify: true/g" $HOME/apps/spack/var/spack/environments/$theenv/spack.yaml
spack env activate $theenv

spack add gcc@${gcc_vintel}
spack add git
spack add ninja
spack add cmake@${cmake_vnew}
spack add libxml2@${libxml2_v}%gcc@${gcc_vintel}
spack add boost@${boost_vnew}%gcc@${gcc_vintel}
spack add util-linux-uuid%gcc@${gcc_vintel}
spack add python%gcc@${gcc_vintel}
spack add hdf5@${hdf5_vnew}%gcc@${gcc_vintel} +fortran +hl ~mpi
spack add fftw@${fftw_vnew}%gcc@${gcc_vintel} -mpi #Avoid MPI for simplicity

spack add py-lxml
spack add py-matplotlib
spack add py-pandas
spack add py-scipy
spack add py-h5py ^hdf5@${hdf5_vnew}%gcc@${gcc_vintel} +fortran +hl ~mpi
install_environment
spack env deactivate

echo --- Removing build deps
for f in `spack env list`
do
    spack env activate $f
    spack gc --yes-to-all
    echo --- Software for environment $f
    spack env status
    spack find
    spack env deactivate
done

echo --- Making loads files
for f in `spack env list`
do
    spack env activate $f
    spack module tcl refresh -y
    spack env loads
    spack env deactivate
done

echo --- FINISH environment setup `date`