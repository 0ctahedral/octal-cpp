#pragma once

#include "octal/defines.h"
#include "octal/core/logger.h"
#include <vulkan/vulkan_core.h>
#include <vector>

namespace octal {
  class Shader {
    std::vector<char> m_Bytecode;
    const VkDevice& m_Device;

    public:
      VkShaderModule module;
      /// Basic Constructor
      /// @param device that 
      Shader(const VkDevice& device, const std::string& filename);

      /// Destructor
      /// destroys the shader module for us
      ~Shader();

      // TODO: this might need to go somewhere else
      /// Creates a shader module from this shader
      bool createShaderModule();
  //  protected:
      /// Read a binary file containing SPIR-V bytecode
      /// @param filename name of the file to open
      /// @returns a vector of bytes read from the file
      static std::vector<char> BinRead(const std::string& filename);
  };
}
