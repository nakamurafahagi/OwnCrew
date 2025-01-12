# Copyright (c) 2024-present The OwnCrew Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://opensource.org/license/mit/.

set(CMAKE_AUTOMOC_MOC_OPTIONS "-p${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(test_owncrew-qt
  apptests.cpp
  optiontests.cpp
  rpcnestedtests.cpp
  test_main.cpp
  uritests.cpp
  util.cpp
  ../../init/owncrew-qt.cpp
)

target_link_libraries(test_owncrew-qt
  core_interface
  owncrewqt
  test_util
  owncrew_node
  Boost::headers
  Qt5::Test
)

import_plugins(test_owncrew-qt)

if(ENABLE_WALLET)
  target_sources(test_owncrew-qt
    PRIVATE
      addressbooktests.cpp
      wallettests.cpp
      ../../wallet/test/wallet_test_fixture.cpp
  )
endif()

add_test(NAME test_owncrew-qt
  COMMAND test_owncrew-qt
)
if(WIN32 AND VCPKG_TARGET_TRIPLET)
  # On Windows, vcpkg configures Qt with `-opengl dynamic`, which makes
  # the "minimal" platform plugin unusable due to internal Qt bugs.
  set_tests_properties(test_owncrew-qt PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=windows"
  )
endif()

install(TARGETS test_owncrew-qt
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
