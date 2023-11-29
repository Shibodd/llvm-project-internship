#ifndef SHM_CONTROL_REGISTER_HPP
#define SHM_CONTROL_REGISTER_HPP

struct ShmDeviceControlRegister {
  int echo; // device writes 1, host pulls down to 0
};

#endif // !SHM_CONTROL_REGISTER_HPP