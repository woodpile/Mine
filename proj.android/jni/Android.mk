LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

# 定义 all-cpp-files 返回当前路径和 Classes 路径想的所有 cpp 文件，注意：这里只考虑 cpp 而没有 c，如果需要自行添加
define all-cpp-files
$(patsubst jni/%,%, $(shell find $(LOCAL_PATH)/../../Classes/ $(LOCAL_PATH)/hellocpp -name "*.cpp" -o -name "*.c"))  
endef

# 这里使用新的方式替换换来的方式，以自动添加源文件
LOCAL_SRC_FILES := $(call all-cpp-files)

#LOCAL_SRC_FILES := hellocpp/main.cpp \
#                  ../../Classes/AppDelegate.cpp \
#                  ../../Classes/GameMenu.cpp \
#				   ../../Classes/config/Config.cpp \
#				   ../../Classes/util/UtilNet.cpp \
#				   ../../Classes/util/UtilRc4.c \
#				   ../../Classes/game/GameScene.cpp \
#				   ../../Classes/game/GameSceneNet.cpp \
#				   ../../Classes/game/HelpBoard.cpp \
#				   ../../Classes/game/MBox.cpp \
#				   ../../Classes/game/MMap.cpp \
#				   ../../Classes/game/MPage.cpp \
#				   ../../Classes/game/PageSelectScene.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
					$(LOCAL_PATH)/../../cocos2d/cocos/network/ \
					$(LOCAL_PATH)/../../cocos2d/external

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

# LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
# LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
 LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static


include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)

# $(call import-module,Box2D)
# $(call import-module,editor-support/cocosbuilder)
# $(call import-module,editor-support/spine)
# $(call import-module,editor-support/cocostudio)
 $(call import-module,network)
# $(call import-module,extensions)
