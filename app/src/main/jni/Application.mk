# Define desired ABI's
APP_ABI := armeabi armeabi-v7a

# Required by OpenCV :(
APP_CPPFLAGS := -frtti -fexception

# Turn debug on (only for testings)
APP_OPTIM := debug

# Change toolchain to the newer version for maximum c++11 support
NDK_TOOLCHAIN_VERSION := 4.9

# Enable c++11 support application wide
APP_CPPFLAGS := -std=c++11

# Set GNUSTL for supporting such features as "chrono", "features" etc
APP_STL := gnustl_static

# Set target platform to 4.4. Just trust me.
APP_PLATFORM := android-19
