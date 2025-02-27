#!/usr/bin/env bash

# ReflexEngine configure script
# Detects OS, installs dependencies, and prepares build environment

# ----- STYLING VARIABLES -----
RESET="\033[0m"
BOLD="\033[1m"
DIM="\033[2m"
ITALIC="\033[3m"

# Colors
BLACK="\033[38;5;0m"
WHITE="\033[38;5;15m"
GRAY="\033[38;5;8m"
DGRAY="\033[38;5;240m"
ACCENT="\033[38;5;39m"
SUCCESS="\033[38;5;78m"
WARNING="\033[38;5;214m"
ERROR="\033[38;5;203m"
INFO="\033[38;5;75m"

# Background colors
BG_BLACK="\033[48;5;0m"
BG_DGRAY="\033[48;5;236m"
BG_ACCENT="\033[48;5;39m"

# ----- UTILITY FUNCTIONS -----

# Print section header
print_header() {
  local title="$1"
  local width=70
  local padding=$(( (width - ${#title} - 4) / 2 ))
  
  echo
  echo -e "${ACCENT}┏${DGRAY}${BOLD}$(printf '━%.0s' $(seq 1 $width))${ACCENT}┓${RESET}"
  echo -e "${ACCENT}┃ ${RESET}${BOLD}${WHITE}$(printf ' %.0s' $(seq 1 $padding))${title}$(printf ' %.0s' $(seq 1 $padding))${ACCENT} ┃${RESET}"
  echo -e "${ACCENT}┗${DGRAY}${BOLD}$(printf '━%.0s' $(seq 1 $width))${ACCENT}┛${RESET}"
  echo
}

# Print status message
print_status() {
  echo -e "${BG_DGRAY}${WHITE}${BOLD} $1 ${RESET} $2"
}

# Print success message
print_success() {
  echo -e " ${SUCCESS}${BOLD}✓${RESET} $1"
}

# Print error message
print_error() {
  echo -e " ${ERROR}${BOLD}✗${RESET} $1"
}

# Print info message
print_info() {
  echo -e " ${INFO}${BOLD}i${RESET} $1"
}

# Print warning message
print_warning() {
  echo -e " ${WARNING}${BOLD}!${RESET} $1"
}

# Check if command exists
command_exists() {
  command -v "$1" >/dev/null 2>&1
}

# Ask for confirmation
confirm() {
  echo -en " ${WARNING}${BOLD}?${RESET} $1 [Y/n] "
  read -r response
  case "$response" in
    [nN][oO]|[nN]) 
      return 1
      ;;
    *)
      return 0
      ;;
  esac
}

# Show spinner while executing a command
show_spinner() {
  local -r pid="$1"
  local -r delay=0.1
  local spinstr='⣾⣽⣻⢿⡿⣟⣯⣷'
  local temp
  local message="$2"

  echo -en " ${ACCENT}${spinstr:0:1}${RESET} $message"

  while kill -0 "$pid" 2>/dev/null; do
    temp="${spinstr#?}"
    printf "\r ${ACCENT}%c${RESET} $message" "${spinstr:0:1}"
    spinstr=$temp${spinstr%"$temp"}
    sleep $delay
  done
  
  wait "$pid"
  return $?
}

# ----- DETECT OPERATING SYSTEM -----
detect_os() {
  print_header "SYSTEM DETECTION"
  
  if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Detect Linux distribution
    if command_exists apt-get; then
      OS="debian"
      print_success "Detected Debian/Ubuntu-based system"
    elif command_exists pacman; then
      OS="arch"
      print_success "Detected Arch Linux-based system"
    elif command_exists apk; then
      OS="alpine"
      print_success "Detected Alpine Linux"
    else
      print_warning "Detected Linux but could not identify specific distribution"
      if confirm "Would you like to proceed with Debian/Ubuntu configuration?"; then
        OS="debian"
      else
        print_error "Unable to determine Linux distribution. Exiting."
        exit 1
      fi
    fi
  elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    print_success "Detected macOS"
  elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    print_info "Detected Windows environment"
    echo
    echo -e " ${BG_DGRAY}${WHITE} 1 ${RESET} MSVC (Visual Studio)"
    echo -e " ${BG_DGRAY}${WHITE} 2 ${RESET} MinGW"
    echo
    echo -en " ${WARNING}${BOLD}?${RESET} Select Windows build environment [1/2]: "
    read -r win_choice
    
    if [[ "$win_choice" == "1" ]]; then
      OS="windows"
      print_success "Selected MSVC (Visual Studio) environment"
    else
      OS="mingw"
      print_success "Selected MinGW environment"
    fi
  else
    print_error "Unsupported operating system: $OSTYPE"
    exit 1
  fi
}

# ----- INSTALL DEPENDENCIES -----
install_dependencies() {
  print_header "DEPENDENCIES"
  
  case $OS in
    debian)
      print_status "APT" "Updating package lists..."
      sudo apt-get update -q > /dev/null &
      show_spinner $! "Updating package lists..."
      print_success "Package lists updated"

      print_status "APT" "Installing build dependencies..."
      packages="build-essential gcc make liblua5.4-dev libuv1-dev"
      sudo apt-get install -y $packages > /dev/null 2>&1 &
      show_spinner $! "Installing: $packages"
      
      if command_exists gcc && command_exists lua5.4; then
        print_success "Dependencies installed successfully"
      else
        print_error "Failed to install all dependencies"
        print_info "Please try installing them manually:"
        echo -e "    ${DIM}sudo apt-get install $packages${RESET}"
        exit 1
      fi
      ;;
      
    arch)
      print_status "PACMAN" "Updating package database..."
      sudo pacman -Sy --noconfirm > /dev/null &
      show_spinner $! "Updating package database..."
      print_success "Package database updated"

      print_status "PACMAN" "Installing build dependencies..."
      packages="base-devel gcc make lua libuv"
      sudo pacman -S --needed --noconfirm $packages > /dev/null 2>&1 &
      show_spinner $! "Installing: $packages"
      
      if command_exists gcc && command_exists lua; then
        print_success "Dependencies installed successfully"
      else
        print_error "Failed to install all dependencies"
        print_info "Please try installing them manually:"
        echo -e "    ${DIM}sudo pacman -S --needed $packages${RESET}"
        exit 1
      fi
      ;;
      
    alpine)
      print_status "APK" "Updating package lists..."
      sudo apk update > /dev/null &
      show_spinner $! "Updating package lists..."
      print_success "Package lists updated"

      print_status "APK" "Installing build dependencies..."
      packages="build-base gcc make lua5.4-dev"
      sudo apk add $packages > /dev/null 2>&1 &
      show_spinner $! "Installing: $packages"
      
      if command_exists gcc && command_exists lua5.4; then
        print_success "Dependencies installed successfully"
      else
        print_error "Failed to install all dependencies"
        print_info "Please try installing them manually:"
        echo -e "    ${DIM}sudo apk add $packages${RESET}"
        exit 1
      fi
      ;;
      
    macos)
      if ! command_exists brew; then
        print_error "Homebrew is required but not installed"
        print_info "Please install Homebrew first:"
        echo -e "    ${DIM}/bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\"${RESET}"
        exit 1
      fi
      
      print_status "BREW" "Updating Homebrew..."
      brew update > /dev/null &
      show_spinner $! "Updating Homebrew..."
      print_success "Homebrew updated"

      print_status "BREW" "Installing build dependencies..."
      packages="lua libuv gcc make"
      brew install $packages > /dev/null 2>&1 &
      show_spinner $! "Installing: $packages"
      
      if command_exists gcc && command_exists lua; then
        print_success "Dependencies installed successfully"
      else
        print_error "Failed to install all dependencies"
        print_info "Please try installing them manually:"
        echo -e "    ${DIM}brew install $packages${RESET}"
        exit 1
      fi
      ;;
      
    windows)
      print_warning "For Windows MSVC, please ensure you have:"
      print_info "1. Visual Studio with C++ development tools installed"
      print_info "2. Lua development files in C:\\Program Files\\Lua"
      print_info "3. The Lua bin directory added to your PATH"
      
      if confirm "Have you installed these dependencies?"; then
        print_success "Ready to build with MSVC"
      else
        print_info "Please install the required dependencies and run this script again"
        exit 1
      fi
      ;;
      
    mingw)
      print_warning "For MinGW builds, please ensure you have:"
      print_info "1. MSYS2 installed from https://www.msys2.org/"
      print_info "2. Run the following command in MSYS2 shell:"
      echo -e "    ${DIM}pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-lua${RESET}"
      
      if confirm "Have you installed these dependencies?"; then
        print_success "Ready to build with MinGW"
      else
        print_info "Please install the required dependencies and run this script again"
        exit 1
      fi
      ;;
  esac
}

# ----- VERIFY INSTALLATION -----
verify_tools() {
  print_header "VERIFICATION"
  
  # Check for make
  if command_exists make; then
    make_version=$(make --version | head -n 1)
    print_success "make: $make_version"
  else
    print_error "make not found"
    return 1
  fi
  
  # Check for compiler
  case $OS in
    debian|arch|alpine|macos)
      if command_exists gcc; then
        gcc_version=$(gcc --version | head -n 1)
        print_success "gcc: $gcc_version"
      else
        print_error "gcc not found"
        return 1
      fi
      ;;
    windows)
      print_info "Please verify MSVC installation manually"
      ;;
    mingw)
      if command_exists x86_64-w64-mingw32-gcc; then
        mingw_version=$(x86_64-w64-mingw32-gcc --version | head -n 1)
        print_success "MinGW gcc: $mingw_version"
      else
        print_error "MinGW gcc not found"
        return 1
      fi
      ;;
  esac
  
  # Check for Lua
  case $OS in
    debian|alpine)
      if command_exists lua5.4; then
        lua_version=$(lua5.4 -v 2>&1)
        print_success "lua: $lua_version"
      else
        print_error "lua5.4 not found"
        return 1
      fi
      ;;
    arch|macos)
      if command_exists lua; then
        lua_version=$(lua -v 2>&1)
        print_success "lua: $lua_version"
      else
        print_error "lua not found"
        return 1
      fi
      ;;
    windows|mingw)
      print_info "Please verify Lua installation manually"
      ;;
  esac
  
  return 0
}

# ----- CREATE CONFIG FILE -----
create_config_file() {
  print_header "CONFIGURATION"
  
  config_file="config.mk"
  print_status "CONFIG" "Writing to $config_file"
  
  cat > "$config_file" << EOF
# Auto-generated by configure.sh
# ReflexEngine build configuration

# Detected platform: $OS
SELECTED_DISTRO = $OS

# Do not edit below this line
EOF

  print_success "Configuration saved to $config_file"
}

# ----- MAIN FUNCTION -----
main() {
  clear
  echo
  # Modern ASCII art logo with cleaner lines and monospace alignment
  echo -e "${ACCENT}┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓${RESET}"
  echo -e "${ACCENT}┃                                                                  ┃${RESET}"
  echo -e "${ACCENT}┃  ${INFO}██████  ${SUCCESS}██████  ${ITALIC}${WARNING}██████  ${ERROR}██      ${ACCENT}██████  ${INFO}██   ██   ${ACCENT}┃${RESET}"
  echo -e "${ACCENT}┃  ${INFO}██   ██ ${SUCCESS}██      ${ITALIC}${WARNING}██      ${ERROR}██      ${ACCENT}██      ${INFO}██   ██   ${ACCENT}┃${RESET}"
  echo -e "${ACCENT}┃  ${INFO}██████  ${SUCCESS}█████   ${ITALIC}${WARNING}█████   ${ERROR}██      ${ACCENT}█████   ${INFO}███████   ${ACCENT}┃${RESET}"
  echo -e "${ACCENT}┃  ${INFO}██   ██ ${SUCCESS}██      ${ITALIC}${WARNING}██      ${ERROR}██      ${ACCENT}██      ${INFO}██   ██   ${ACCENT}┃${RESET}"
  echo -e "${ACCENT}┃  ${INFO}██   ██ ${SUCCESS}██████  ${ITALIC}${WARNING}██      ${ERROR}███████ ${ACCENT}██      ${INFO}██   ██   ${ACCENT}┃${RESET}"
  echo -e "${ACCENT}┃                                                                  ┃${RESET}"
  echo -e "${ACCENT}┃${BG_DGRAY}${WHITE}${BOLD}                      ENGINE CONFIGURATION                       ${ACCENT}┃${RESET}"
  echo -e "${ACCENT}┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛${RESET}"
  echo

  # Detect OS
  detect_os
  
  # Install dependencies
  install_dependencies
  
  # Verify tools
  if ! verify_tools; then
    print_error "Tool verification failed"
    exit 1
  fi
  
  # Create config file
  create_config_file
  
  # Final success message
  print_header "BUILD READY"
  print_success "Configuration complete! You can now build ReflexEngine with:"
  echo
  echo -e "    ${ACCENT}${BOLD}make $OS${RESET}"
  echo
  
  print_info "For more build options, run:"
  echo
  echo -e "    ${ACCENT}${BOLD}make help${RESET}"
  echo
}

# Run the main function
main