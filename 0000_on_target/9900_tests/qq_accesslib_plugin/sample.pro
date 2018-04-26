TEMPLATE = subdirs

SUBDIRS = \
  qq_accesslib_plugin \
  hmi_app_sample

sample.depends = qq_accesslib_plugin hmi_app_sample
