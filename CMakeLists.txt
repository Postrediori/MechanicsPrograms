project(MechanicsPrograms)
cmake_minimum_required(VERSION 2.8)

# 3rd party
include(cmake/3rdparty.txt)

# common properties
include(cmake/config.txt)

#build
add_subdirectory(FEMSolve)
add_subdirectory(FinitElem)
add_subdirectory(FinitElem2)
add_subdirectory(FluidView)
add_subdirectory(HazeDyn)
add_subdirectory(MediaWave)
add_subdirectory(SimplexView)
add_subdirectory(WaveView)
