#include <cstdlib>

#include "utility.hpp"

using namespace llvm;

static void ProcessModule(Module &M) {
  size_t loads = 0;
  size_t stores = 0;
  size_t returns = 0;

  // TODO: do something meaningful here
  for (Function &F : M.functions()) {
    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        if (auto Load = dyn_cast<LoadInst>(&I)) {
          loads++;
        }
        if (auto Store = dyn_cast<StoreInst>(&I)) {
          stores++;
        }
        if (auto Return = dyn_cast<ReturnInst>(&I)) {
          returns++;
        }
      }
    }
  }
  outs() << "Loads: " << loads << "\n";
  outs() << "Stores: " << stores << "\n";
  outs() << "Returns: " << returns << "\n";
}

int main(int argc, char **argv) {
  // Parse arguments
  if (argc < 2) {
    printf("Usage: %s in.bc\n", std::filesystem::path(argv[0]).filename().c_str());
    return EXIT_FAILURE;
  }
  auto inFile = argv[1];

  // Load module
  LLVMContext C;
  auto M = LoadModule(C, inFile);

  // Process module
  try {
    ProcessModule(*M);
  } catch (const std::exception &x) {
    outs() << x.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
