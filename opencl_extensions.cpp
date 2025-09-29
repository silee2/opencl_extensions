#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>
#include <CL/cl_ext.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main() {
  // http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clGetPlatformIDs.html
  cl_uint platformIdCount = 0;
  cl_platform_id intelGraphicsId = NULL;
  clGetPlatformIDs(0, nullptr, &platformIdCount);

  if (platformIdCount == 0) {
    std::cerr << "No OpenCL platform found" << std::endl;
    return 1;
  } else {
    std::cout << "Found " << platformIdCount << " platform(s)" << std::endl;
  }

  std::vector<cl_platform_id> platformIds(platformIdCount);
  clGetPlatformIDs(platformIdCount, platformIds.data(), nullptr);

  size_t infoSize = 0;
  char *info = NULL;
  for (cl_uint i = 0; i < platformIdCount; ++i) {
    // get platform attribute value size
    clGetPlatformInfo(platformIds[i], CL_PLATFORM_NAME, 0, NULL, &infoSize);
    info = (char *)std::malloc(infoSize);

    // get platform attribute value
    clGetPlatformInfo(platformIds[i], CL_PLATFORM_NAME, infoSize, info, NULL);
    std::string platformName(info);
    std::cout << "Platform(" << i << "): " << platformName << std::endl;
    std::free(info);
    info = NULL;
    infoSize = 0;
    if (0 == platformName.compare("Intel(R) OpenCL Graphics") ||
        0 == platformName.compare("Intel(R) OpenCL HD Graphics")) {
      std::cout << "Found Intel Graphics: Platform(" << i << ")" << std::endl;
      intelGraphicsId = platformIds[i];
      // get platform attribute value size
      clGetPlatformInfo(platformIds[i], CL_PLATFORM_VERSION, 0, NULL,
                        &infoSize);
      info = (char *)std::malloc(infoSize);

      // get platform attribute value
      clGetPlatformInfo(platformIds[i], CL_PLATFORM_VERSION, infoSize, info,
                        NULL);
      std::string platformVersion(info);
      std::cout << "OpenCL Version: " << platformVersion << std::endl;
      std::free(info);
      info = NULL;
      infoSize = 0;
      // get platform attribute value size
      clGetPlatformInfo(platformIds[i], CL_PLATFORM_EXTENSIONS, 0, NULL,
                        &infoSize);
      info = (char *)std::malloc(infoSize);

      // get platform attribute value
      clGetPlatformInfo(platformIds[i], CL_PLATFORM_EXTENSIONS, infoSize, info,
                        NULL);
      std::string platformExtensions(info);
      std::cout << "CL_PLATFORM_EXTENSIONS: " << platformExtensions
                << std::endl;
      std::free(info);
      info = NULL;
      infoSize = 0;
    }
  }

  // http://www.khronos.org/registry/cl/sdk/1.1/docs/man/xhtml/clGetDeviceIDs.html
  cl_uint deviceIdCount = 0;
  clGetDeviceIDs(intelGraphicsId, CL_DEVICE_TYPE_GPU, 0, nullptr,
                 &deviceIdCount);

  if (deviceIdCount == 0) {
    std::cerr << "No OpenCL GPU devices found" << std::endl;
    return 1;
  } else {
    std::cout << "Found " << deviceIdCount << " device(s)" << std::endl;
  }

  std::vector<cl_device_id> deviceIds(deviceIdCount);
  clGetDeviceIDs(intelGraphicsId, CL_DEVICE_TYPE_GPU, deviceIdCount,
                 deviceIds.data(), nullptr);

  for (cl_uint i = 0; i < deviceIdCount; ++i) {
    // get platform attribute value size
    clGetDeviceInfo(deviceIds[i], CL_DEVICE_NAME, 0, NULL, &infoSize);
    info = (char *)std::malloc(infoSize);

    // get platform attribute value
    clGetDeviceInfo(deviceIds[i], CL_DEVICE_NAME, infoSize, info, NULL);
    std::string deviceName(info);
    std::cout << "Device(" << i << "): " << deviceName << std::endl;

    std::free(info);
    info = NULL;
    infoSize = 0;

    clGetDeviceInfo(deviceIds[i], CL_DRIVER_VERSION, 0, NULL,
        &infoSize);
    info = (char *)std::malloc(infoSize);
    clGetDeviceInfo(deviceIds[i], CL_DRIVER_VERSION, infoSize, info,
        NULL);
    std::string driverVersion(info);
    std::cout << "Driver Version: "  << driverVersion << std::endl;
    std::free(info);
    info = NULL;
    infoSize = 0;

    // get platform attribute value size
    clGetDeviceInfo(deviceIds[i], CL_DEVICE_EXTENSIONS, 0, NULL, &infoSize);
    info = (char *)std::malloc(infoSize);

    // get platform attribute value
    clGetDeviceInfo(deviceIds[i], CL_DEVICE_EXTENSIONS, infoSize, info, NULL);
    std::string deviceExtensions(info);
    std::cout << "CL_DEVICE_EXTENSIONS: " << deviceExtensions << std::endl;
    std::free(info);
    info = NULL;
    infoSize = 0;

    // All queries below depends on cl_khr_spirv_queries extension
    if(deviceExtensions.find("cl_khr_spirv_queries") == std::string::npos) {
      continue;
    }

    char **infoPtr = NULL;

    // get CL_DEVICE_SPIRV_EXTENDED_INSTRUCTION_SETS_KHR
    // get size
    clGetDeviceInfo(deviceIds[i], CL_DEVICE_SPIRV_EXTENDED_INSTRUCTION_SETS_KHR, 0, NULL, &infoSize);
    infoPtr = (char **)std::malloc(infoSize);

    // get value
    clGetDeviceInfo(deviceIds[i], CL_DEVICE_SPIRV_EXTENDED_INSTRUCTION_SETS_KHR, infoSize, infoPtr, NULL);
    std::cout << "CL_DEVICE_SPIRV_EXTENDED_INSTRUCTION_SETS_KHR: " << std::endl;
    for(int i = 0; i < infoSize / sizeof(char **) ; i++) {
      std::string inst{infoPtr[i]};
      std::cout << "\t" << inst << "," << std::endl;
    }

    std::free(infoPtr);
    infoPtr = NULL;
    infoSize = 0;

    // get CL_DEVICE_SPIRV_EXTENSIONS_KHR
    clGetDeviceInfo(deviceIds[i], CL_DEVICE_SPIRV_EXTENSIONS_KHR, 0, NULL, &infoSize);
    infoPtr = (char **)std::malloc(infoSize);

    // get platform attribute value
    clGetDeviceInfo(deviceIds[i], CL_DEVICE_SPIRV_EXTENSIONS_KHR, infoSize, infoPtr, NULL);
    std::cout << "CL_DEVICE_SPIRV_EXTENSIONS_KHR: " << std::endl;
    for(int i = 0; i < infoSize / sizeof(char **) ; i++) {
      std::string ext{infoPtr[i]};
      std::cout << "\t" << ext << "," << std::endl;
    }

    std::free(infoPtr);
    infoPtr = NULL;
    infoSize = 0;
    // get CL_DEVICE_SPIRV_CAPABILITIES_KHR
    clGetDeviceInfo(deviceIds[i], CL_DEVICE_SPIRV_CAPABILITIES_KHR, 0, NULL, &infoSize);
    cl_uint * infoUint = (cl_uint *)std::malloc(infoSize);

    // get platform attribute value
    clGetDeviceInfo(deviceIds[i], CL_DEVICE_SPIRV_CAPABILITIES_KHR, infoSize, infoUint, NULL);
    std::cout << "CL_DEVICE_SPIRV_CAPABILITIES_KHR: " << std::endl;
    for(int i = 0; i < infoSize / sizeof(cl_uint) ; i++) {
      std::cout << "\t" << infoUint[i] << "," << std::endl;
    }


    std::free(infoUint);
    infoUint = NULL;
    infoSize = 0;
  }

  return 0;
}
