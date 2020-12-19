cmake_minimum_required (VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR}/asio/asio)
include_directories(${PROJECT_SOURCE_DIR}/asio)
include_directories(${PROJECT_SOURCE_DIR}/aidi_libs/334/aidi_vision_client334/include)
include_directories(${PROJECT_SOURCE_DIR}/aidi_libs/334/opencv3.3.0.0.1/include)
include_directories(${PROJECT_SOURCE_DIR}/aidi_libs/334/boost.1.59.0.0/include)
include_directories(${PROJECT_SOURCE_DIR}/aidi_libs/334/jsoncpp/include)
include_directories(${PROJECT_SOURCE_DIR}/VCameraCore/include)

link_directories(${PROJECT_SOURCE_DIR}/VCameraCore/lib/x64)
link_directories(${PROJECT_SOURCE_DIR}/aidi_libs/334/opencv3.3.0.0.1/build/Release/x64)
link_directories(${PROJECT_SOURCE_DIR}/aidi_libs/334/aidi_vision_client334/Build/Release/x64)

aux_source_directory(. SRC_LIST)

add_library(JujubeClassification4Weisong SHARED ${SRC_LIST})

target_link_libraries(JujubeClassification4Weisong CoreLib opencv_world300 aidi_vision334)
