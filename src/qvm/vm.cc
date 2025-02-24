#include "vm.h"

#include <stdio.h>
#include <string.h>

#include <format>
#include <typeinfo>

#include "inst/cmp.h"
#include "inst/jmp.h"
#include "inst/lod_byte.h"
#include "inst/lod_dword.h"
#include "inst/lod_word.h"
#include "inst/mov.h"
#include "inst/pop.h"
#include "inst/push.h"
#include "qvm.h"
#include "util.h"

VM::VM() {
  memory = new byte[MEMORY_SIZE];
  memset(memory, 0, MEMORY_SIZE);

  memset(&registers, 0, sizeof(registers));
  registers.sp = MEMORY_SIZE - 1;
  registers.bp = registers.sp;
  registers.ip = 0;
  debug_state = DebugState::NONE;
  log_instructions = false;
  printf("VM initalize\n");
}

std::vector<byte> VM::getForward(int n) {
  std::vector<byte> res = {};
  for (int i = 0; i < n; i++) {
    if (i >= MEMORY_SIZE) {
      return {};
    }
    res.push_back(memory[registers.ip + i]);
  }
  return res;
}

dword *VM::getRegisterFromIndex(int index) {
  switch (index) {
  case R0:
    return &registers.r0;
  case R1:
    return &registers.r1;
  case R2:
    return &registers.r2;
  case R3:
    return &registers.r3;
  case R4:
    return &registers.r4;
  case SP:
    return &registers.sp;
  case BP:
    return &registers.bp;
  case IP:
    return &registers.ip;
  default:
    break;
  }
  return &registers.r0;
}

std::string VM::getRegisterNameFromIndex(int index) {
  switch (index) {
  case R0:
    return "r0";
  case R1:
    return "r1";
  case R2:
    return "r2";
  case R3:
    return "r3";
  case R4:
    return "r4";
  case SP:
    return "sp";
  case BP:
    return "bp";
  case IP:
    return "ip";
  default:
    return "???";
  }
  return "r0";
}

InstConvResult VM::convertIntoInstruction() {
  InstConvResult result;
  result.success = false;
  result.output = nullptr;

  if (memory[registers.ip] >= MOVC_R0 &&
      memory[registers.ip] <= MOVC_IP) { // movc
    std::vector<byte> bytes = getForward(5);
    if (bytes.empty())
      return result;

    MovcInstruction *instruction = new MovcInstruction();
    instruction->dest = getRegisterFromIndex(bytes[0] - MOVC_R0);
    instruction->source = convertQEndian(bytes.data() + 1);
    result.output = instruction;

    result.disassembly = std::format(
        "mov {} 0x{:x}", getRegisterNameFromIndex(bytes[0] - MOVC_R0),
        instruction->source);
    result.success = true;
    registers.ip += 5;
  } else if (memory[registers.ip] >= MOV_R0 &&
             memory[registers.ip] <= MOV_IP) { // mov
    std::vector<byte> bytes = getForward(2);
    if (bytes.empty())
      return result;

    MovInstruction *instruction = new MovInstruction();
    instruction->dest = getRegisterFromIndex(bytes[0] - MOV_R0);
    instruction->source = getRegisterFromIndex(bytes[1]);
    result.output = instruction;

    result.disassembly =
        std::format("mov {} {}", getRegisterNameFromIndex(bytes[0] - MOV_R0),
                    getRegisterNameFromIndex(bytes[1]));
    result.success = true;
    registers.ip += 2;
  } else if (memory[registers.ip] == HLT) { // hlt
    result.disassembly = "hlt";
    result.success = true;
    result.output = nullptr;
    registers.ip = MEMORY_SIZE + 1;
  } else if (memory[registers.ip] == NOP) { // nop
    result.disassembly = "nop";
    result.success = true;
    result.output = nullptr;
    registers.ip++;
  } else if (memory[registers.ip] >= LOD_R0 &&
             memory[registers.ip] <= LOD_IP) { // lod
    std::vector<byte> bytes = getForward(3);
    if (bytes.empty())
      return result;

    LodInstruction *instruction;
    std::string size_name = "???";
    switch (bytes[1]) {
    case 0x0: // dword
      size_name = "dword";
      instruction = new LodDwordInstruction();
      break;
    case 0x1: // word
      size_name = "word";
      instruction = new LodWordInstruction();
      break;
    case 0x2: // byte
      size_name = "byte";
      instruction = new LodByteInstruction();
      break;
    default:
      result.output = nullptr;
      result.disassembly = "???";
      result.success = false;

      return result;
    }
    instruction->source = getRegisterFromIndex(bytes[0] - LOD_R0);
    instruction->dest = getRegisterFromIndex(bytes[2]);
    result.output = instruction;

    result.disassembly =
        std::format("lod {} {} {}", getRegisterNameFromIndex(bytes[2]),
                    size_name, getRegisterNameFromIndex(bytes[0] - LOD_R0));
    result.success = true;
    registers.ip += 3;
  } else if (memory[registers.ip] >= CMP_R0 &&
             memory[registers.ip] <= CMP_IP) { // cmp
    std::vector<byte> bytes = getForward(2);
    if (bytes.empty()) {
      return result;
    }

    CmpInstruction *instruction = new CmpInstruction();
    instruction->left = getRegisterFromIndex(bytes[0] - CMP_R0);
    instruction->right = getRegisterFromIndex(bytes[1]);
    result.output = instruction;

    result.disassembly =
        std::format("cmp {} {}", getRegisterNameFromIndex(bytes[0] - CMP_R0),
                    getRegisterNameFromIndex(bytes[1]));
    result.success = true;
    registers.ip += 2;
  } else if (memory[registers.ip] == PUSH) { // push
    std::vector<byte> bytes = getForward(2);
    if (bytes.empty())
      return result;

    PushInstruction *instruction = new PushInstruction();
    instruction->source = getRegisterFromIndex(bytes[1]);
    result.output = instruction;

    result.disassembly =
        std::format("push {}", getRegisterNameFromIndex(bytes[1]));
    result.success = true;
    registers.ip += 2;

    return result;
  } else if (memory[registers.ip] == PUSHC) { // pushc
    std::vector<byte> bytes = getForward(5);
    if (bytes.empty())
      return result;

    PushcInstruction *instruction = new PushcInstruction();
    instruction->source = convertQEndian(bytes.data() + 1);
    result.output = instruction;

    result.disassembly = std::format("pushc 0x{:x}", instruction->source);
    result.success = true;
    registers.ip += 5;

    return result;
  } else if (memory[registers.ip] == POP) { // pop
    std::vector<byte> bytes = getForward(2);
    if (bytes.empty())
      return result;

    PopInstruction *instruction = new PopInstruction();
    instruction->dest = getRegisterFromIndex(bytes[1]);
    result.output = instruction;

    result.disassembly =
        std::format("pop {}", getRegisterNameFromIndex(bytes[1]));
    result.success = true;
    registers.ip += 2;

    return result;
  } else if (memory[registers.ip] == JE) { // jmps
    std::vector<byte> bytes = getForward(6);
    if (bytes.empty())
      return result;

    JmpInstruction *instruction = nullptr;
    result.disassembly = "jmp ";

    switch (bytes[0]) {
    case JE:
      instruction = new JeInstruction();
      result.disassembly = "je ";
      break;
    }

    if (bytes[1] == 0xff) { // constant jump
      instruction->dest = convertQEndian(bytes.data() + 2);
      result.disassembly += std::format("0x{:x}", instruction->dest);
      registers.ip += 6;
    } else { // assume a register jump
      instruction->dest = *getRegisterFromIndex(bytes[1]);
      result.disassembly +=
          std::format("{}", getRegisterNameFromIndex(bytes[1]));
      registers.ip += 2;
    }

    result.output = instruction;

    result.success = true;

    return result;
  }

  return result;
}

void VM::execute() {
  while (registers.ip < MEMORY_SIZE) {
    InstConvResult result = convertIntoInstruction();
    if (!result.success) {
      printf("[qvm] invalid instruction. execution stopped\n");
      break;
    } else {
      if (result.disassembly != "mov r0 r0" && log_instructions) {
        printf("%s\n", result.disassembly.c_str());
      }

      if (result.output != nullptr) {
        if (result.disassembly.find("???") != std::string::npos) {
          handleException(Exception::ILLEGAL_REGISTER);
          break;
        }

        Exception exception = result.output->execute(this);

        delete result.output;

        if (!handleException(exception)) {
          break;
        }
      }
    }
  }
}

void VM::fprintState(FILE *descriptor) {
  fprintf(descriptor, "R0: 0x%x R1: 0x%x R2: 0x%x R3: 0x%x R4: 0x%x\n",
          registers.r0, registers.r1, registers.r2, registers.r3, registers.r4);
  fprintf(descriptor, "SP: 0x%x BP: 0x%x IP: 0x%x\n", registers.sp,
          registers.bp, registers.ip);
  fprintf(descriptor, "ZF: 0x%x CF: 0x%x\n", flags.ZF, flags.CF);
}

bool VM::handleException(Exception exception) {
  if (debug_state == DebugState::EXCEPTION && exception != Exception::NONE) {
    fprintState(stdout);
  }

  switch (exception) {
  case Exception::MEMORY_NOT_IN_BOUNDS:
    printf("[qvm] exception: MEMORY_NOT_IN_BOUNDS\n");
    return false;
  case Exception::ILLEGAL_REGISTER:
    printf("[qvm] exception: ILLEGAL_REGISTER\n");
    return false;
  default:
    break;
  }
  return true;
}

VM::~VM() {
  delete[] memory;
  printf("VM uninitalize\n");
}
