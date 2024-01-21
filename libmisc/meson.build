sources = files(
  'pps-page-selector.c',
)

sources += gnome.compile_resources(
  'pps-resources',
  'libmisc.gresource.xml',
  c_name: 'libmisc',
)

libppsmisc = static_library(
  'evmisc',
  sources: sources,
  include_directories: top_inc,
  dependencies: libppsview_dep,
)

libppsmisc_dep = declare_dependency(
  include_directories: include_directories('.'),
  dependencies: libppsview_dep,
  link_whole: libppsmisc,
)
