cmake_minimum_required(VERSION 3.15)
project(fluid_sim VERSION 1.0 LANGUAGES CXX)

# Configuração do C++ (SFML 2.6+ beneficia de C++17 ou superior)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Encontrar os componentes necessários do SFML 2.6.2
find_package(SFML 2.6.2 COMPONENTS graphics window system REQUIRED)

# Definir o executável e incluir os ficheiros de código-fonte
add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
# Associar as bibliotecas do SFML ao teu executável
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)