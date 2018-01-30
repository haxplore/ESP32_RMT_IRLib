#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := esp32_rmt_irlib

# find rmtlib here
EXTRA_COMPONENT_DIRS += $(PROJECT_PATH)/src
 
include $(IDF_PATH)/make/project.mk

