#!/bin/ash
apt-get update

# Install necessary packages
apk add --no-cache cmake curl-dev make g++ linux-headers

# Download and extract gtest
wget https://github.com/google/googletest/archive/release-1.14.0.tar.gz
tar xf release-1.14.0.tar.gz
cd googletest-release-1.14.0

# Build gtest
cmake .
make
mv googletest/include/gtest /usr/include
mv googlemock/gtest/libgtest*.a /usr/lib

# Clean up
cd ..
rm -rf googletest-release-1.10.0 release-1.10.0.tar.gz