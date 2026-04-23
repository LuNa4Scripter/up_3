CXX = g++.exe
CXXFLAGS = -fdiagnostics-color=always -g -I./source/include -I./source/include/rlImGui-main -IC:/Users/me/Downloads/raylib-6.0_win64_mingw-w64/raylib-6.0_win64_mingw-w64/include
LDFLAGS = -LC:/Users/me/Downloads/raylib-6.0_win64_mingw-w64/raylib-6.0_win64_mingw-w64/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRC_DIRS = source/main source/data source/player source/physics source/weapon/Weapon source/weapon/Weapon_Use \
           source/Weapon_UI source/background source/particles source/Animation source/Animation/Loading_Screen_Animation \
           source/Animation/UI_Animation/Player_setting source/Score source/AI/Cs_AI_Manager source/AI/Cs_AI_Play \
           source/Camera/Cinematic_Camera source/UI/player_setting source/UI/Heart_UI source/Heart/AI_Heart \
           source/Heart/AI_Heart/AI_Heart_Manager source/Heart/Player_Heart source/Heart/Player_Heart/player_Heart_function_implementation \
           source/Heart/UI_call source/include source/include/rlImGui-main source/Server_Optimization/AI_Optimization

SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(SRCS:.cpp=.o)
TARGET = bin/cyber_sky.exe

$(TARGET): $(OBJS)
	if not exist bin mkdir bin
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	if exist bin del /q /s bin\*.exe
	del /q /s source\*.o