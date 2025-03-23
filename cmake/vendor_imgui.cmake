set(IMGUI_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/imgui")
set(IMGUI_PREFIX "${CMAKE_BINARY_DIR}/vendor_imgui")
set(IMGUI_INSTALL_PREFIX "${IMGUI_PREFIX}/install/$<CONFIG>")

set(IMGUI_SOURCES
"${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx9.h"
"${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.h"
"${IMGUI_SOURCE_DIR}/imconfig.h"
"${IMGUI_SOURCE_DIR}/imgui.h"
"${IMGUI_SOURCE_DIR}/imgui_internal.h"
"${IMGUI_SOURCE_DIR}/imstb_rectpack.h"
"${IMGUI_SOURCE_DIR}/imstb_textedit.h"
"${IMGUI_SOURCE_DIR}/imstb_truetype.h"
"${IMGUI_SOURCE_DIR}/misc/cpp/imgui_stdlib.h"
"${IMGUI_SOURCE_DIR}/backends/imgui_impl_dx9.cpp"
"${IMGUI_SOURCE_DIR}/backends/imgui_impl_win32.cpp"
"${IMGUI_SOURCE_DIR}/imgui.cpp"
"${IMGUI_SOURCE_DIR}/imgui_demo.cpp"
"${IMGUI_SOURCE_DIR}/imgui_draw.cpp"
"${IMGUI_SOURCE_DIR}/imgui_tables.cpp"
"${IMGUI_SOURCE_DIR}/imgui_widgets.cpp"
"${IMGUI_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp"
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC "${IMGUI_SOURCE_DIR}")
target_include_directories(imgui PUBLIC "${IMGUI_SOURCE_DIR}/backends")
target_include_directories(imgui PUBLIC "${IMGUI_SOURCE_DIR}/misc/cpp")
set_target_properties(imgui PROPERTIES FOLDER "Vendor")
