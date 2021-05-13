#include "octal/renderer/shader.h"
#include <fstream>

namespace octal {
  Shader::Shader(const VkDevice& dev, const std::string& filename):
    m_Device(dev)
  {
    m_Bytecode = BinRead(filename);
  }

  Shader::~Shader() {
    vkDestroyShaderModule(m_Device, module, nullptr);
  }

  bool Shader::createShaderModule() {
    VkShaderModuleCreateInfo create{};
    create.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create.codeSize = m_Bytecode.size();
    create.pCode = reinterpret_cast<const u32*>(m_Bytecode.data());
    
    return vkCreateShaderModule(m_Device, &create, nullptr, &module) == VK_SUCCESS;
  }

  std::vector<char> Shader::BinRead(const std::string& filename) {
    // open file and seek to the end so that we know how bit it is
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    if (!file.is_open()) {
      ERROR("Failed to open file: %s", filename.c_str());
      // close the file
      file.close();
      throw std::runtime_error("failed to open file");
    }

    // get the position which is the end
    auto size = file.tellg();
    // create the buffer
    std::vector<char> buffer(size);

    // seek to beginning and read into buffer
    file.seekg(0);
    file.read(buffer.data(), size);

    // close the file
    file.close();

    return buffer;
  }
}
