#!/usr/bin/env bash

sudo apt update
sudo apt install -y libpng-dev
sudo apt install -y libfftw3-dev
sudo apt install -y build-essential cmake pkg-config git
sudo apt install -y libportaudio2 libportaudio-dev
sudo apt install -y mosquitto mosquitto-clients
sudo systemctl enable --now mosquitto
sudo apt install -y libssl-dev
sudo apt install -y libpaho-mqtt1.3 libpaho-mqtt-dev
sudo apt install -y libcjson-dev

cd ../third_party || exit 1
git clone https://github.com/mborgerding/kissfft.git
