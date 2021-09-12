# this is for files that need the vrapi libs in order to compile

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../../../contrib
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../GHOculus
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../../GHOculusAvatar

LOCAL_SRC_FILES += ../../../../GHAndroid/GHOvrGoAvatarPoser.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOvrGoHMDServices.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOvrGoHMDRenderDevice.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOvrGoRenderTarget.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOvrGoHMDTransformApplier.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOvrGoSystemController.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOvrGoUtil.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOvrGoInputState.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOvrGoInputTranslator.cpp
LOCAL_SRC_FILES += ../../../../GHAndroid/GHOvrGoTextureLoader.cpp

LOCAL_SRC_FILES += ../../../../GHOculusAvatar/GHOculusAvatar.cpp
LOCAL_SRC_FILES += ../../../../GHOculusAvatar/GHOculusAvatarComponentOverride.cpp
LOCAL_SRC_FILES += ../../../../GHOculusAvatar/GHOculusAvatarCustomGestureSetter.cpp
LOCAL_SRC_FILES += ../../../../GHOculusAvatar/GHOculusAvatarDebugDrawHands.cpp
LOCAL_SRC_FILES += ../../../../GHOculusAvatar/GHOculusAvatarUtil.cpp
LOCAL_SRC_FILES += ../../../../GHOculusAvatar/GHOculusAvatarXMLLoader.cpp
LOCAL_SRC_FILES += ../../../../GHOculus/GHOculusGameScale.cpp