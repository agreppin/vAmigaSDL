#include <ctime>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <SDL3/SDL.h>

#include "VAmigaConfig.h"
#include "VAmiga.h"
#include "MediaFile.h"

#include "debug.h"
#include "microknight.h"
#include "scancodes.h"

using namespace vamiga;

enum STATE {
  INIT  = 0x00,
  PAUSE = 0x01,
  RUN   = 0x02,
};

static STATE state_ = STATE::INIT;

// Visible area
constexpr int xstart = (HBLANK_MAX + 1) * 4;
constexpr int xend   = HPIXELS;
constexpr int ystart = 0x1B;
constexpr int yend   = 0x137;
static_assert(xend - xstart <= HPIXELS);
static_assert(yend - ystart <= VPIXELS);

[[noreturn]] void throw_sdl_error(const std::string &what) { throw std::runtime_error{what + " failed: " + SDL_GetError()}; }

class sdl_init {
public:
  explicit sdl_init(uint32_t flags)
      : flags_{flags} {
    if (!SDL_InitSubSystem(flags_))
      throw_sdl_error("SDL_InitSubSystem " + std::to_string(flags_));
  }

  sdl_init(const sdl_init &)            = delete;
  sdl_init &operator=(const sdl_init &) = delete;

  ~sdl_init() { SDL_QuitSubSystem(flags_); }

private:
  const uint32_t flags_;
};

#define MAKE_SDL_PTR(type, destroyer) \
  struct type##_destroyer {           \
    void operator()(type *p) const {  \
      if (p)                          \
        destroyer(p);                 \
    }                                 \
  };                                  \
  using type##_ptr = std::unique_ptr<type, type##_destroyer>

MAKE_SDL_PTR(SDL_Window, SDL_DestroyWindow);
MAKE_SDL_PTR(SDL_Renderer, SDL_DestroyRenderer);
MAKE_SDL_PTR(SDL_Texture, SDL_DestroyTexture);

struct sdl_freer {
  void operator()(void *ptr) const {
    if (ptr)
      SDL_free(ptr);
  }
};

class driver {
public:
  static constexpr int audio_sample_rate = 48000;
  static constexpr int screen_width      = xend - xstart;
  static constexpr int screen_height     = 2 * (yend - ystart);

  explicit driver()
      : current_frame_(HPIXELS * VPIXELS),
        last_frame_(HPIXELS * VPIXELS) {}

  ~driver() {
    emulator_.powerOff();
    SDL_DestroyAudioStream(stream_);
  }

  int run(int argc, char *argv[]) {
    emulator_.set(ConfigScheme::A500_OCS_1MB);
    emulator_.set(Opt::HOST_SAMPLE_RATE, audio_sample_rate);
    for (int n = 0; n < 4; ++n)
      emulator_.set(Opt::HDC_CONNECT, false, n);

    bool auto_power_on = true;
    int drive = 0, hd = 0, scale = 1;
    FloppyDriveAPI *df[] = {&emulator_.df0, &emulator_.df1, &emulator_.df2, &emulator_.df3};
    HardDriveAPI *dh[]   = {&emulator_.hd0, &emulator_.hd1, &emulator_.hd2, &emulator_.hd3};
    fs::path hdpath[4]   = {};

    emulator_.launch(
      this, [](const void *ptr, Message msg) { reinterpret_cast<driver *>(const_cast<void *>(ptr))->msg_queue_callback(msg); });

    for (int i = 1; i < argc; ++i) {
      if (!strcmp(argv[i], "-bigbox")) { // TODO this can be done in .ini scripts
        emulator_.set(ConfigScheme::A500_ECS_1MB);
        emulator_.set(Opt::MEM_CHIP_RAM, 2048);
        emulator_.set(Opt::MEM_FAST_RAM, 8192);
        emulator_.set(Opt::MEM_SLOW_RAM, 0);
        emulator_.set(Opt::CPU_OVERCLOCKING, 14);
        emulator_.set(Opt::CPU_REVISION, (i64)CPURev::CPU_68EC020);
        continue;
      } else if (!strcmp(argv[i], "-a600")) {
        emulator_.set(ConfigScheme::A500_ECS_1MB);
        emulator_.set(Opt::MEM_CHIP_RAM, 1024);
        emulator_.set(Opt::MEM_SLOW_RAM, 0);
        continue;
      } else if (!strcmp(argv[i], "-2")) {
        scale = 2;
        continue;
      }

      const fs::path p{argv[i]};
      const fs::file_status status = fs::status(p);
      if (status == fs::file_status(fs::file_type::not_found)) {
        dbg("File not found: %s\n", argv[i]);
        return 1;
      }
      const fs::perms perms = status.permissions();
      const bool wp         = ((perms & fs::perms::owner_write) == fs::perms::none);
      FileType mt           = MediaFile::type(p);
      if (mt == FileType::UNKNOWN) {
        dbg("Unknown file type: %s\n", argv[i]);
        return 1;
      }
      auto media = std::unique_ptr<MediaFile>(MediaFile::make(p, mt));
      switch (mt) {
      case FileType::ADF:
      case FileType::ADZ:
      case FileType::EADF:
      case FileType::EXE:
      case FileType::DMS:
      case FileType::DIR:
      case FileType::IMG:
      case FileType::ST:
        if (drive >= 4) {
          dbg("Too many floppy drives\n");
          return 1;
        }
        dbg("Inserting DF%d (r%c): '%s'\n", drive, wp ? 'o' : 'w', argv[i]);
        if (drive)
          emulator_.set(Opt::DRIVE_CONNECT, true, drive);
        df[drive]->insertMedia(*media, wp);
        ++drive;
        break;
      case FileType::HDF:
      case FileType::HDZ:
        if (hd >= 4) {
          dbg("Too many hard drives\n"); // TODO dbg() => log()
          return 1;
        }
        dbg("Attaching DH%d (r%c): '%s'\n", hd, wp ? 'o' : 'w', argv[i]);
        emulator_.set(Opt::HDC_CONNECT, true, hd);
        dh[hd]->attach(*media);
        // dh[hd]->setFlag(DiskFlags::MODIFIED, false); // TODO FIXME
        if (!wp)
          hdpath[hd] = argv[i];
        ++hd;
        break;
      case FileType::ROM:
        emulator_.mem.loadRom(*media);
        break;
      case FileType::EXTENDED_ROM:
        emulator_.mem.loadExt(*media);
        break;
      case FileType::SNAPSHOT:
        dbg("Loading snapshot: %s\n", argv[i]);
        emulator_.powerOn();
        amiga_.loadSnapshot(*media);
        emulator_.run();
        auto_power_on = false;
        break;
      case FileType::SCRIPT: /* .retrosh */
        dbg("Executing script: %s\n", argv[i]);
        emulator_.retroShell.execScript(*media);
        auto_power_on = false;
        continue;
      default:
        dbg("Unknown file type: '%s' '%s'\n", argv[i], FileTypeEnum::help(mt));
        return 1;
      }
    }

    // SDL_HINT_WINDOWS_CLOSE_ON_ALT_F4     "1" is default
    // SDL_HINT_ALLOW_ALT_TAB_WHILE_GRABBED "1" is default
    int window_w          = scale * screen_width;
    int window_h          = scale * screen_height;
    SDL_WindowFlags flags = 0; // SDL_WINDOW_RESIZABLE;
    // config / script proposal:
    // # parse UI setup directives in RetroShell comments begining with '# UI: '
    // # UI: scale = 2
    // # UI: SDL_HINT_RENDER_VSYNC = 1	# easy to parse
    // # UI: bindkey SHIFT-F11 snapshot_save

    window_.reset(SDL_CreateWindow("vAmiga", window_w, window_h, flags));
    if (!window_)
      throw_sdl_error("SDL_CreateWindow");

    renderer_.reset(SDL_CreateRenderer(window_.get(), NULL));
    if (!renderer_)
      throw_sdl_error("SDL_CreateRenderer");

    texture_.reset(
      SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height));
    if (!texture_)
      throw_sdl_error("SDL_CreateTexture");

    overlay_.reset(
      SDL_CreateTexture(renderer_.get(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height));
    if (!overlay_)
      throw_sdl_error("SDL_CreateTexture");

    if (!SDL_SetTextureBlendMode(overlay_.get(), SDL_BLENDMODE_BLEND))
      throw_sdl_error("SDL_SetTextureBlendMode");

    SDL_AudioSpec want{};
    want.freq     = audio_sample_rate;
    want.format   = SDL_AUDIO_F32;
    want.channels = 2;

    stream_ = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &want, audio_callback, this);
    if (!stream_)
      throw_sdl_error("SDL_OpenAudioDeviceStream");

    if (!emulator_.mem.getInfo().hasRom) {
      const fs::path rom{"kick13.rom"};
      emulator_.mem.loadRom(rom);
    }

    if (auto_power_on) {
      emulator_.powerOn();
      emulator_.run();
    }

    int ret = 0;
    for (;;) {
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT)
          break;
        handle_event(e);
      }

      if (e.type == SDL_EVENT_QUIT)
        break;

      if (abort_) {
        ret = (int)~abort_;
        break;
      }

      if (!(state_ & STATE::RUN)) {
        SDL_Delay(100);
        continue;
      }

      update_viewport();
    }

    // write back hard drives
    for (int i = 0; i < hd; ++i) {
      const string p = hdpath[i].string();
      try {
        const HardDriveInfo &info = dh[i]->getInfo();
        if (info.writeProtected || p.empty()) { // TODO FIXME
          dbg("DH%d is write protected\n", i);
          continue;
        }
        if (info.modified) {
          dbg("Writing DH%d: '%s'\n", i, p.c_str());
          dh[i]->writeToFile(hdpath[i]);
        }
      } catch (const std::exception &e) {
        dbg("Error writing DH%d: '%s' '%s'\n", i, p.c_str(), e.what());
      }
    }

    return ret;
  }

private:
  sdl_init sdl_init_{SDL_INIT_VIDEO | SDL_INIT_AUDIO};
  SDL_Window_ptr window_;
  SDL_Renderer_ptr renderer_;
  SDL_Texture_ptr texture_;
  SDL_Texture_ptr overlay_;
  SDL_AudioStream *stream_;
  bool need_halt_      = false;
  bool mouse_captured_ = false;
#ifdef WSL2_MOUSE_HACK
  int last_mouse_x_ = 0;
  int last_mouse_y_ = 0;
#endif
  const u32 *last_buffer_pointer_ = nullptr;
  bool last_frame_type_           = false;
  std::vector<uint32_t> current_frame_;
  std::vector<uint32_t> last_frame_;
  bool overlay_active_         = false;
  bool overlay_dirty_          = true;
  bool overlay_blink_          = false;
  bool power_is_on_            = false;
  i64 abort_                   = 0;
  uint64_t last_overlay_blink_ = 0;
  std::string ser_buffer_;
  VAmiga emulator_;
  AmigaAPI &amiga_ = emulator_.amiga;

  void capture_mouse(bool enabled);
  void msg_queue_callback(Message msg);
  void handle_event(const SDL_Event &e);
  void update_viewport(void);
  void handle_overlay_key(const SDL_KeyboardEvent &e);
  void handle_overlay_mouse(const SDL_MouseButtonEvent &b);
  static void draw_char(void *pixels, int pitch, int x, int y, char c, uint32_t color);
  static void draw_string(void *pixels, int pitch, int x, int y, const char *str, uint32_t color);
  static void draw_cursor(void *pixels, int pitch, int x, int y, uint32_t color);
  void update_overlay();
  bool handle_joystick_key(const SDL_KeyboardEvent *e);
  static void SDLCALL audio_callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);
};

int main(int argc, char *argv[]) {
  std::ios::sync_with_stdio(true);
  try {
    driver d;
    return d.run(argc, argv);
  } catch (const std::exception &e) {
    dbg("Exception: %s\n", e.what());
    return 1;
  }
}

void driver::capture_mouse(bool enabled) {
  if (enabled == mouse_captured_)
    return;
  SDL_Window *window = window_.get();
  if (!SDL_SetWindowKeyboardGrab(window, enabled))
    dbg("SDL_SetWindowKeyboardGrab: %s\n", SDL_GetError());
  SDL_SetWindowRelativeMouseMode(window, enabled);
  std::string title = "vAmiga";
  if (enabled)
    title += " - mouse & kb captured";
  SDL_SetWindowTitle(window, title.c_str());
  mouse_captured_ = enabled;
}

void driver::handle_event(const SDL_Event &e) {
  switch (e.type) {
  case SDL_EVENT_KEY_DOWN:
    if (overlay_active_) {
      handle_overlay_key(e.key);
      break;
    }
    if (e.key.key == SDLK_F12) {
      overlay_active_ = true;
      overlay_dirty_  = true;
      capture_mouse(false);
      break;
    }
    if (e.key.key == SDLK_F11 && (e.key.mod & (SDL_KMOD_RSHIFT | SDL_KMOD_RALT))) {
      // TODO use PrintScreen, ScrollLock or Pause keys instead ?
      if (e.key.mod & SDL_KMOD_SHIFT) {
#ifdef SCREEN_RECORDER
        if (!amiga_.denise.screenRecorder.isRecording()) {
          amiga_.denise.screenRecorder.startRecording(0, 0, HPIXELS, VPIXELS, 100'000, 1, 2);
          std::cout << "Recording started\n";
        } else {
          amiga_.denise.screenRecorder.stopRecording();
        }
#endif
      } else {
        std::unique_ptr<MediaFile> snapshot{amiga_.takeSnapshot()};
        assert(snapshot);
        if (snapshot) {
          char filename[256];
          auto t    = std::time(nullptr);
          tm *local = std::localtime(&t);
          snprintf(filename, sizeof(filename), "snapshot_%d%02d%02d%02d%02d%02d.snp", 1900 + local->tm_year, 1 + local->tm_mon,
                   local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
          std::cout << "Saving snapshot to " << filename << "\n";
          snapshot->writeToFile(filename);
        }
      }
      break;
    }
    [[fallthrough]];
  case SDL_EVENT_KEY_UP:
    // TODO CapsLock state ?
    // TODO LGUI, waiting on SDL issue #1227
    {
      if (overlay_active_)
        break;
      if (e.key.repeat) /* let Amiga handles this */
        break;
      const bool up     = (e.type == SDL_EVENT_KEY_UP);
      const auto key    = convert_scancode(&e.key);
      SDL_Keymod kmods  = SDL_GetModState(); // TODO SDL_KMOD_LGUI vs SDL_KMOD_GUI issue
      const char *kname = SDL_GetKeyName(e.key.key);
      dbg("scancode: %3d UP %d key 0x%02X mod 0x%04x %s\n", e.key.scancode, up, key, kmods, kname);
      if (handle_joystick_key(&e.key))
        break;
      if (key != 0xFF) {
        if (up)
          emulator_.keyboard.release((KeyCode)key);
        else
          emulator_.keyboard.press((KeyCode)key);
      }
    }
    break;
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
  case SDL_EVENT_MOUSE_BUTTON_UP:
    if (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT) {
      if (!mouse_captured_) {
        if (overlay_active_)
          handle_overlay_mouse(e.button);
        else
          capture_mouse(true);
      } else {
        const bool pressed = e.type == SDL_EVENT_MOUSE_BUTTON_DOWN; // TODO middle ?
        bool left          = (e.button.button == SDL_BUTTON_LEFT);
        auto &mouse        = emulator_.controlPort1.mouse;
        if (pressed)
          mouse.trigger(left ? GamePadAction::PRESS_LEFT : GamePadAction::PRESS_RIGHT);
        else
          mouse.trigger(left ? GamePadAction::RELEASE_LEFT : GamePadAction::RELEASE_RIGHT);
      }
    }
    break;
  case SDL_EVENT_MOUSE_MOTION:
    if (mouse_captured_ && (e.motion.xrel || e.motion.yrel)) {
#ifdef WSL2_MOUSE_HACK
      // Probably only for WSL2: xrel/yrel are actually *not* relative (and x/y don't update)??
      amiga_.controlPort1.mouse.setDxDy(e.motion.xrel - last_mouse_x_, e.motion.yrel - last_mouse_y_);
      last_mouse_x_ = e.motion.xrel;
      last_mouse_y_ = e.motion.yrel;
#else
      emulator_.controlPort1.mouse.setDxDy(e.motion.xrel, e.motion.yrel);
      // Make sure mouse doesn't end up on the window border
      SDL_WarpMouseInWindow(window_.get(), screen_width / 2, screen_height / 2);
#endif
    }
    break;
  case SDL_EVENT_WINDOW_FOCUS_LOST:
  case SDL_EVENT_WINDOW_MOUSE_LEAVE:
    capture_mouse(false);
    break;
  }
}

void driver::msg_queue_callback(Message msg) {
  switch (msg.type) {
  case Msg::RSH_UPDATE:
  case Msg::RSH_DEBUGGER:
  case Msg::DRIVE_SELECT:
  case Msg::DRIVE_STEP:
  case Msg::DRIVE_POLL:
  case Msg::DISK_INSERT:
  case Msg::DISK_EJECT:
  case Msg::DRIVE_LED:
  case Msg::DRIVE_MOTOR:
  case Msg::SER_IN:
  case Msg::HDR_READ:
  case Msg::HDR_WRITE:
  case Msg::HDR_IDLE:
  case Msg::HDR_STEP:
  case Msg::HDC_STATE:
  case Msg::HDC_CONNECT:
  case Msg::POWER_LED_ON:
  case Msg::POWER_LED_OFF:
  case Msg::POWER_LED_DIM:
  case Msg::MEM_LAYOUT:
  case Msg::DMA_DEBUG:
    return;
  case Msg::VIEWPORT:
    if constexpr (1) {
      const ViewportMsg &vp = msg.viewport;
      // dbg("ViewportMsg: hstrt %d hstop %d vstrt %d vstop %d\n"
      dbg("MsgQueue: type=%2ld (%s) %d,%d,%d,%d\n", (long)msg.type, MsgEnum::key(msg.type), vp.hstrt, vp.hstop, vp.vstrt,
          vp.vstop);
      return;
    }
    break;
  case Msg::CONFIG:
  case Msg::DRIVE_CONNECT:
  case Msg::OVERCLOCKING:
  case Msg::RESET:
    break;
  case Msg::VIDEO_FORMAT:
    // ntsc = (msg.value == TV::NTSC);
    break;
  case Msg::CTRL_AMIGA_AMIGA:
#if 1 // TODO softReset()
    emulator_.hardReset();
#else
    emulator_.softReset();
#endif
    break;
  case Msg::MUTE:
    if (msg.value)
      SDL_PauseAudioStreamDevice(stream_);
    else
      SDL_ResumeAudioStreamDevice(stream_);
    break;
  case Msg::PAUSE:
    state_ = STATE::PAUSE;
    break;
  case Msg::RUN:
    state_ = STATE::RUN;
    SDL_ResumeAudioStreamDevice(stream_);
    break;
  case Msg::ABORT:
    abort_ = ~msg.value;
    break;
  case Msg::POWER:
    power_is_on_ = msg.value;
    if (msg.value) {
    }
    break;

  case Msg::RECORDING_STOPPED:
#ifdef SCREEN_RECORDER
    amiga_.denise.screenRecorder.exportAs("test.mp4");
#endif
    std::cout << "Recording exported\n";
    break;
    // case Msg::SER_OUT:
    //     if ((data1 & 0xff) != '\n') {
    //         ser_buffer_.push_back(static_cast<char>(data1 & 0xff));
    //         return;
    //     }
    //     while (!ser_buffer_.empty() && ser_buffer_.back() == '\r')
    //         ser_buffer_.pop_back();
    //     std::cout << "Serial data: \"" << ser_buffer_.c_str() << "\"\n";
    //     ser_buffer_.clear();
    //     return;
  default:
    break;
  }
  dbg("MsgQueue: type=%2ld value=%lld (%s)\n", (long)msg.type, (i64)msg.value, MsgEnum::key(msg.type));
}

// TODO SDL3 API Gamepad/Joystick
// TODO there are more actions: PRESS/RELEASE/FIRE/2/3, RELEASE_XY
static constexpr int sdlkp2action[10][2] = {
  {                              -1,                             -1}, // 1
  {   (int)GamePadAction::RELEASE_Y,  (int)GamePadAction::PULL_DOWN}, // 2
  {                              -1,                             -1}, // 3
  {   (int)GamePadAction::RELEASE_X,  (int)GamePadAction::PULL_LEFT}, // 4
  {(int)GamePadAction::RELEASE_FIRE, (int)GamePadAction::PRESS_FIRE}, // 5
  {   (int)GamePadAction::RELEASE_X, (int)GamePadAction::PULL_RIGHT}, // 6
  {                              -1,                             -1}, // 7
  {   (int)GamePadAction::RELEASE_Y,    (int)GamePadAction::PULL_UP}, // 8
  {                              -1,                             -1}, // 9
  {(int)GamePadAction::RELEASE_FIRE, (int)GamePadAction::PRESS_FIRE}, // 0
};

// numpad mode when NumLock if off
bool driver::handle_joystick_key(const SDL_KeyboardEvent *e) {
  if (e->mod & SDL_KMOD_NUM)
    return false;
  auto &joystick = emulator_.controlPort2.joystick;
  if (e->key < SDLK_KP_1 || e->key > SDLK_KP_0)
    return false;
  const int action = sdlkp2action[e->key - SDLK_KP_1][e->down];
  if (action >= 0) {
    dbg("%s: %s\n", __func__, GamePadActionEnum::_key((GamePadAction)action));
    joystick.trigger((GamePadAction)action);
  }
  return true;
}

void driver::handle_overlay_key(const SDL_KeyboardEvent &e) {
  // TODO: Shift+Enter
  assert(overlay_active_);
  auto &rs = emulator_.retroShell;
  switch (e.key) {
  case SDLK_ESCAPE:
  case SDLK_F12:
    overlay_active_      = false;
    last_buffer_pointer_ = nullptr; // Force update (dirty)
    break;
  case SDLK_UP:
    rs.press(RetroShellKey::UP);
    break;
  case SDLK_DOWN:
    rs.press(RetroShellKey::DOWN);
    break;
  case SDLK_LEFT:
    rs.press(RetroShellKey::LEFT);
    break;
  case SDLK_RIGHT:
    rs.press(RetroShellKey::RIGHT);
    break;
  case SDLK_HOME:
    rs.press(RetroShellKey::HOME);
    break;
  case SDLK_END:
    rs.press(RetroShellKey::END);
    break;
  case SDLK_TAB:
    rs.press(RetroShellKey::TAB);
    break;
  case SDLK_BACKSPACE:
    rs.press(RetroShellKey::BACKSPACE);
    break;
  case SDLK_DELETE:
    rs.press(RetroShellKey::DEL);
    break;
  case SDLK_RETURN:
    rs.press(RetroShellKey::RETURN);
    break;
  default:
    SDL_Keycode keycode = SDL_GetKeyFromScancode(e.scancode, e.mod, false);
    if (keycode < 0x7f)
      rs.press((char)keycode);
    break;
  }
}

void driver::handle_overlay_mouse(const SDL_MouseButtonEvent &b) {
  if (b.button == SDL_BUTTON_RIGHT && !b.down) {
    // Paste
    std::unique_ptr<char, sdl_freer> text{SDL_GetClipboardText()};
    if (text)
      emulator_.retroShell.press(text.get());
  }
}

// VIEWPORT RENDERING
void driver::update_viewport(void) {
  // TODO: Implement new long frame logic
  bool update      = overlay_active_ && overlay_dirty_;
  VideoPortAPI &vp = emulator_.videoPort;
  vp.lockTexture();
  static isize last_frame_number = 0;
  isize nr;
  bool lof, prevlof;
  const u32 *ptr = vp.getTexture(&nr, &lof, &prevlof);
  if (nr != last_frame_number /*|| ptr != last_buffer_pointer_ */) { // HACK: Don't update if not a new frame
    std::memcpy(&current_frame_[0], ptr, HPIXELS * VPIXELS * sizeof(uint32_t));
    void *pixels;
    int pitch;
    if (!SDL_LockTexture(texture_.get(), nullptr, &pixels, &pitch))
      throw_sdl_error("SDL_LockTexture");
    uint8_t *dest1       = reinterpret_cast<uint8_t *>(pixels) + !lof * pitch;
    uint8_t *dest2       = reinterpret_cast<uint8_t *>(pixels) + lof * pitch;
    const uint32_t *src1 = &current_frame_[0];
    const uint32_t *src2 = (lof == last_frame_type_) ? &current_frame_[0] : &last_frame_[0];

    src1 += HPIXELS * ystart + HBLANK_MAX * 4; // xstart;
    src2 += HPIXELS * ystart + HBLANK_MAX * 4; // xstart;
    for (uint32_t y = 0; y < screen_height / 2; ++y) {
      std::memcpy(dest1, src1, screen_width * sizeof(uint32_t));
      std::memcpy(dest2, src2, screen_width * sizeof(uint32_t));
      dest1 += 2 * pitch;
      dest2 += 2 * pitch;
      src1 += HPIXELS;
      src2 += HPIXELS;
    }
    SDL_UnlockTexture(texture_.get());
    // SDL_RenderClear(renderer_.get());

    std::swap(current_frame_, last_frame_);

    last_frame_type_     = lof;
    last_buffer_pointer_ = ptr;
    last_frame_number    = nr;
    update               = true;
  }
  vp.unlockTexture();

  if (overlay_active_)
    update_overlay();

  if (update) {
    SDL_RenderTexture(renderer_.get(), texture_.get(), nullptr, nullptr);
    if (overlay_active_)
      SDL_RenderTexture(renderer_.get(), overlay_.get(), nullptr, nullptr);
    SDL_RenderPresent(renderer_.get());
    emulator_.wakeUp();
  }
  SDL_Delay(5);
}

// OVERLAY RENDERING
static constexpr int char_scale  = 1;
static constexpr int char_width  = 8 * char_scale;
static constexpr int char_height = 8 * char_scale;

void driver::draw_char(void *pixels, int pitch, int x, int y, char c, uint32_t color) {
  assert(x >= 0 && x + char_width <= screen_width);
  assert(y >= 0 && y + char_height <= screen_height);
  if (c < 0x20 || c > 0x7f)
    c = 0x20;

  const uint8_t *chdata = &microknight[c - 0x20][0];

  for (int cy = 0; cy < char_height; ++cy) {
    uint32_t *dst = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(pixels) + x * 4 + (y + cy) * pitch);
    uint8_t bits  = *chdata;
    for (int cx = 0; cx < char_width; ++cx) {
      if (bits & 0x80)
        dst[cx] = color;
      if ((cx + 1) % char_scale == 0)
        bits <<= 1;
    }
    if ((cy + 1) % char_scale == 0)
      ++chdata;
  }
}

void driver::draw_string(void *pixels, int pitch, int x, int y, const char *str, uint32_t color) {
  assert(x >= 0);
  assert(y >= 0 && y + char_height <= screen_width);
  while (*str && x + char_width * 2 <= screen_height) {
    draw_char(pixels, pitch, x, y, *str++, color);
    x += char_width;
  }
}

void driver::draw_cursor(void *pixels, int pitch, int x, int y, uint32_t color) {
  for (int cy = 0; cy < char_height; ++cy) {
    uint32_t *dst = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(pixels) + x * 4 + (y + cy) * pitch);
    for (int cx = 0; cx < char_width; ++cx)
      if ((cx ^ cy) & 1)
        dst[cx] = color;
  }
}

void driver::update_overlay() {
  if (!overlay_dirty_) {
    const auto now = SDL_GetTicks();
    if (now - last_overlay_blink_ < 250)
      return;
    last_overlay_blink_ = now;
  }

  // Hmm...
  std::vector<std::string> lines;
  std::istringstream iss{emulator_.retroShell.text()};
  for (std::string line; std::getline(iss, line);)
    lines.push_back(line);

  void *pixels;
  int pitch;
  if (!SDL_LockTexture(overlay_.get(), nullptr, &pixels, &pitch))
    throw_sdl_error("SDL_LockTexture");

  const uint32_t alpha = 192U << 24;
  for (int y = 0; y < screen_height; ++y) {
    auto row = reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(pixels) + y * pitch);
    for (int x = 0; x < screen_width; ++x)
      row[x] = alpha | 0;
  }

  int y               = char_height;
  const int max_lines = screen_height / char_height - 2;
  for (size_t linecnt = lines.size() > max_lines ? lines.size() - max_lines : 0; linecnt < lines.size(); ++linecnt) {
    const auto &l = lines[linecnt];
    draw_string(pixels, pitch, char_width, y, l.c_str(), alpha | 0xffffff);
    y += char_height;
  }

  if (!overlay_blink_ && !lines.empty()) {
    const auto cpos = static_cast<int>(emulator_.retroShell.cursorRel() + lines.back().length());
    if ((cpos + 1) * char_width < screen_width)
      draw_cursor(pixels, pitch, cpos * char_width, y - char_height, 0xffffffff);
  }

  SDL_UnlockTexture(overlay_.get());
  overlay_dirty_ = false;
  overlay_blink_ = !overlay_blink_;
}

// AUDIO
void SDLCALL driver::audio_callback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {
  driver *d       = static_cast<driver *>(userdata);
  auto &audioPort = d->emulator_.audioPort;
  if (additional_amount > 0) {
    isize n     = additional_amount / sizeof(float);
    float *data = SDL_stack_alloc(float, n);
    if (data) {
      n = audioPort.copyInterleaved(data, n / 2);
      SDL_PutAudioStreamData(stream, data, n * (2 * sizeof(float)));
      SDL_stack_free(data);
    }
  }
}
