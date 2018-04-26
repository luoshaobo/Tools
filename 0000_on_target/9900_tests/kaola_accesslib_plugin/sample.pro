TEMPLATE = subdirs

SUBDIRS = \
  kaola_accesslib_plugin \
  hmi_app_sample

sample.depends = kaola_accesslib_plugin hmi_app_sample
