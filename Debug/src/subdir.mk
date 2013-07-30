################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cameras.cpp \
../src/main.cpp \
../src/stereo.cpp 

C_SRCS += \
../src/jpeg_camera.c \
../src/rs232.c 

OBJS += \
./src/cameras.o \
./src/jpeg_camera.o \
./src/main.o \
./src/rs232.o \
./src/stereo.o 

C_DEPS += \
./src/jpeg_camera.d \
./src/rs232.d 

CPP_DEPS += \
./src/cameras.d \
./src/main.d \
./src/stereo.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/opencv -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -pthread -c -fmessage-length=0 -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


