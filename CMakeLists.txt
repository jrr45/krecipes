cmake_minimum_required(VERSION 3.3.0)
project(krecipes)

set (QT_MIN_VERSION "5.7.0")

cmake_minimum_required(VERSION 3.3.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake 
                      ${CMAKE_MODULE_PATH} )

# search packages used by KDE

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Widgets
    Sql
    WebEngine
    )
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Solid
    DocTools
    KDELibs4Support
    Config
    KHtml
    )

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)


if (Qt5_POSITION_INDEPENDENT_CODE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

add_subdirectory( src )
add_subdirectory( icons )
add_subdirectory( pics )
add_subdirectory( data )
add_subdirectory( layouts )
add_subdirectory( mimetypes )
add_subdirectory( doc )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
