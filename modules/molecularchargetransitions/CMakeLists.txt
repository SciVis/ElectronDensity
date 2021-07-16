ivw_module(MolecularChargeTransitions)

set(HEADER_FILES
    include/inviwo/molecularchargetransitions/algorithm/chargetransfermatrix.h
    include/inviwo/molecularchargetransitions/algorithm/statistics.h
    include/inviwo/molecularchargetransitions/molecularchargetransitionsmodule.h
    include/inviwo/molecularchargetransitions/molecularchargetransitionsmoduledefine.h
    include/inviwo/molecularchargetransitions/processors/clusterstatistics.h
    include/inviwo/molecularchargetransitions/processors/computechargetransfer.h
    include/inviwo/molecularchargetransitions/processors/measureoflocality.h
    include/inviwo/molecularchargetransitions/processors/sumchargeinsegmentedregions.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/algorithm/chargetransfermatrix.cpp
    src/algorithm/statistics.cpp
    src/molecularchargetransitionsmodule.cpp
    src/processors/clusterstatistics.cpp
    src/processors/computechargetransfer.cpp
    src/processors/measureoflocality.cpp
    src/processors/sumchargeinsegmentedregions.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
	# Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/charge-transfer-matrix-test.cpp
    tests/unittests/molecularchargetransitions-unittest-main.cpp
    tests/unittests/statistics-test.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
