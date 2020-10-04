cmake_minimum_required(VERSION 3.5)
project(asio)

install(DIRECTORY asio/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.ipp")
