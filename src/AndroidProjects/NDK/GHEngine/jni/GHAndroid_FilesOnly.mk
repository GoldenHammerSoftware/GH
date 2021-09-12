# This is only the files part of ghandroid Android.mk.
# It is used for including this project in another mk for gradle without building a library.

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../Base
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../Game
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../GHAndroid
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../GHBullet
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../GUI
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../NullPlatform
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../OpenGLES
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../Render
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../Sim
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../Sim/Physics
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../Contrib/bullet3-master/src/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../GHPlatform/android

LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidAdFactory.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidAppRunner.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidAdHandler.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidBitmapLoader.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidEGLFence.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidEGLInfo.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidEGLThread.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidExitHandler.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidExitHandlerXMLLoader.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidGameServices.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidGLESContext.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidIAPStore.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidIAPStoreFactory.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidInterstitialAd.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidKeyboardTransition.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidKeyboardTransitionXMLLoader.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidMediaPlayer.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidRatingListener.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidRenderServices.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidRewardsInterstitial.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidSoundDevice.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidSoundEmitter.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidSoundHandle.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidSoundLoader.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidSoundPool.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidSoundPoolHandle.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidSystemServices.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidTexture.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidTextureFactory.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidURLOpenerTransition.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHAndroidURLOpenerTransitionLoader.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOpenSLDevice.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOpenSLEmitter.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOpenSLLoader.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOpenSLHandle.cpp

