# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libzip
LOCAL_SRC_FILES := libzip.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libpng
LOCAL_SRC_FILES := libpng.a

include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE    := libnativepipboy
LOCAL_CFLAGS    := -Werror
LOCAL_CPPFLAGS += -fexceptions 
LOCAL_SRC_FILES := gl_code.cpp Character.cpp StatsView.cpp Image.cpp Text.cpp Box.cpp ViewList.cpp native-audio-jni.c Sound.cpp Listener.cpp Items.cpp itemdata.cpp ScrollBar.cpp ItemsView.cpp DataView.cpp GeneralConfig.cpp
LOCAL_LDLIBS    := -L$(SYSROOT)/usr/lib -lz -llog -lGLESv1_CM -lstdc++ -lOpenSLES
LOCAL_STATIC_LIBRARIES := libpng libzip
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../android-ndk-assets/project/jni/libpng  $(LOCAL_PATH)/../android-ndk-assets/project/jni/libzip
APP_PLATFORM := android-9

include $(BUILD_SHARED_LIBRARY)
