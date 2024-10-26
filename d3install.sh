apt update
apt install -y --no-install-recommends git ninja-build g++ ca-certificates gpg wget curl pkg-config autoconf automake libtool libltdl-dev make python3-jinja2 libx11-dev libxft-dev libxext-dev libwayland-dev libxkbcommon-dev libegl1-mesa-dev libibus-1.0-dev libasound2-dev libpulse-dev libaudio-dev libjack-dev libsndio-dev curl zip unzip tar ca-certificates gpg wget
test -f /usr/share/doc/kitware-archive-keyring/copyright || wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null
apt update
apt install cmake
git clone https://github.com/microsoft/vcpkg
export VCPKG_ROOT="$PWD/vcpkg"
git clone https://github.com/DescentDevelopers/Descent3
cd Descent3