#include <cctype>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
#include <string>
#include <sys/select.h>
#include <unistd.h>
#include <unordered_map>
#include <utility>

// TODO: for detecting input events
// 1:
// #include <termios.h>
// 2:
// #include <xkbcommon/xkbcommon-keysyms.h>
// #include <xkbcommon/xkbcommon.h>
// maybe detecting input events to run script on key combination should be left
// to client

#define MAGIC_VENDOR 0XBEEB;
#define MAGIC_PROD 0XBEEB;

void emit(int fd, int type, int code, int val) {
  struct input_event ie;

  ie.type = type;
  ie.code = code;
  ie.value = val;

  // this means they are ignored
  ie.time.tv_sec = 0;
  ie.time.tv_usec = 0;

  write(fd, &ie, sizeof(ie));
}

void emit_burst(int fd, int type, int code) {
  bool fdopened = false;
  emit(fd, type, code, 1);
  emit(fd, EV_SYN, SYN_REPORT, 0);
  emit(fd, type, code, 0);
  emit(fd, EV_SYN, SYN_REPORT, 0);

  // if we sleep here we allow the user space to detect the input events
  // however we could just simply check if the file descriptor has been used
  // after our usage
  while (!fcntl(fd, F_GETFD) && fdopened == false) {
    fdopened = true;
  }
}

struct setup_pair {
  uinput_setup usetup;
  int fd;
};

std::unordered_map<char, int> CHAR_TO_UINPUT_MAP = {
    {'0', KEY_0}, {'1', KEY_1}, {'2', KEY_2},  {'3', KEY_3},
    {'4', KEY_4}, {'5', KEY_5}, {'6', KEY_6},  {'7', KEY_7},
    {'8', KEY_8}, {'9', KEY_9}, {'^', KEY_TAB}};

setup_pair get_numeric_setup() {
  struct uinput_setup usetup;
  struct setup_pair usetupPair;

  int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);

  ioctl(fd, UI_SET_EVBIT, EV_KEY);
  for (std::pair<char, int> p : CHAR_TO_UINPUT_MAP) {
    ioctl(fd, UI_SET_KEYBIT, p.second);
  }

  std::memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = MAGIC_VENDOR;
  usetup.id.product = MAGIC_PROD;
  strcpy(usetup.name, "Simulated Device: CRN Paster");

  ioctl(fd, UI_DEV_SETUP, &usetup);
  ioctl(fd, UI_DEV_CREATE);

  usetupPair.usetup = usetup;
  usetupPair.fd = fd;

  // sleep is needed here to allolw the kernel to create a device node for our
  // virtual keyboard
  // TODO: someway to detect this kernel event instead of sleeping
  sleep(1);
  return usetupPair;
}

void crn_actions() {
  std::ifstream crnFile("./CRNs");
  std::string line;

  struct setup_pair usetupPair = get_numeric_setup();
  if (crnFile.is_open()) {
    while (std::getline(crnFile, line)) {
      for (char c : line) {
        if (!std::isalnum(c)) {
          break;
        }
        emit_burst(usetupPair.fd, EV_KEY, CHAR_TO_UINPUT_MAP[c]);
      }
      emit_burst(usetupPair.fd, EV_KEY, KEY_TAB);
    }
  } else {
    std::cerr << "Unable to Open CRN File";
  }
  sleep(1);
  ioctl(usetupPair.fd, UI_DEV_DESTROY);
  close(usetupPair.fd);
}

int main(void) {

  std::cout << "initializing Setup\n";
  crn_actions();

  // Basic setup for making a key press
  // struct setup_pair usetupPair = get_setup(KEY_0);
  // emit_burst(usetupPair.fd, EV_KEY, KEY_0);
  // ioctl(usetupPair.fd, UI_DEV_DESTROY);
  // close(usetupPair.fd);
  // std::cout << "Sent Keystroke";

  return 0;
}
